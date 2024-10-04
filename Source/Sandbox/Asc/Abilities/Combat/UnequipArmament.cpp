// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/UnequipArmament.h"

#include "EquipArmament.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "Sandbox/Data/Enums/ArmamentTypes.h"
#include "Sandbox/Data/Structs/CombatInformation.h"

UUnequipArmament::UUnequipArmament()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_Unequip));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Unequipping));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Unequipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Equipping));

	UnequipEventTag = FGameplayTag::RequestGameplayTag(Tag_Event_Montage_Action);
}


bool UUnequipArmament::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while sheathing the weapons!",
			UEnum::GetValueAsString(ActorInfo->OwnerActor.Get()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(ActorInfo->OwnerActor.Get()));
		return false;
	}

	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while sheathing the weapons!",
			UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		return false;
	}

	UAdvancedMovementComponent* MovementComponent = Character->GetMovementComp<UAdvancedMovementComponent>();
	if (!MovementComponent || MovementComponent->IsWallClimbing() || MovementComponent->IsMantling() || MovementComponent->IsLedgeClimbing())
	{
		if (!MovementComponent)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the movement component while sheathing the weapons!",
				UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		}
		return false;
	}

	
	if (!CombatComponent->GetArmament() && !CombatComponent->GetArmament(false))
	{
		return false;
	}

	bool bCanActivateAbility = false;
	if (CombatComponent->GetArmament() && CombatComponent->GetArmament()->GetEquipStatus() != EEquipStatus::Unequipped)
	{
		bCanActivateAbility = true;
	}
	else if (CombatComponent->GetArmament(false) && CombatComponent->GetArmament(false)->GetEquipStatus() != EEquipStatus::Unequipped)
	{
		bCanActivateAbility = true;
	}

	return bCanActivateAbility;
}


void UUnequipArmament::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent || (!CombatComponent->GetArmament(true) && !CombatComponent->GetArmament(false)))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	
	GetUnequipMontage();
	
	// Equip weapon montage
	// ANPC* NPC = Cast<ANPC>(BaseCharacter); // Multicast to clients, call before the ability task
	// if (NPC)
	// {
	// 	NPC->NetMulticast_PlayMontage(CurrentMontage);
	// }

	MontageTaskHandle = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		*GetNameSafe(this),
		CurrentMontage,
		1.f,
		Montage_Unequip
	);
	MontageTaskHandle->OnCompleted.AddDynamic(this, &UUnequipArmament::OnEndMontage);
	MontageTaskHandle->OnCancelled.AddDynamic(this, &UUnequipArmament::OnEndMontage);
	MontageTaskHandle->OnBlendOut.AddDynamic(this, &UUnequipArmament::OnEndMontage);
	MontageTaskHandle->OnInterrupted.AddDynamic(this, &UUnequipArmament::OnEndMontage);
	MontageTaskHandle->ReadyForActivation();

	EquipGameplayTaskHandle = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UnequipEventTag, nullptr, true, true);
	EquipGameplayTaskHandle->EventReceived.AddDynamic(this, &UUnequipArmament::OnUnequipEventReceived);
	EquipGameplayTaskHandle->ReadyForActivation();

	// Add the unequip state information
	bUnequippedArmaments = false;
	// ApplyEquippedStateInformation(Armament);
	// ApplyEquippedStateInformation(SecondaryArmament);
}


void UUnequipArmament::OnUnequipEventReceived(FGameplayEventData EventData)
{
	HandleUnequipArmamentLogic();
}


void UUnequipArmament::OnEndMontage() { EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false); }
void UUnequipArmament::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bUnequippedArmaments)
	{
		HandleUnequipArmamentLogic();
	}

	// Update the equip status of the armaments
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (CombatComponent)
	{
		if (AArmament* PrimaryArmament = CombatComponent->GetArmament(true))
		{
			PrimaryArmament->SetEquipStatus(EEquipStatus::Unequipped);
		}
		
		if (AArmament* SecondaryArmament = CombatComponent->GetArmament(false))
		{
			SecondaryArmament->SetEquipStatus(EEquipStatus::Unequipped);
		}
	}

	if (MontageTaskHandle) MontageTaskHandle->EndTask();
	if (EquipGameplayTaskHandle) EquipGameplayTaskHandle->EndTask();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


bool UUnequipArmament::GetUnequipMontage()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while unequipping!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return false;
	}

	// TODO: add logic for both armaments
	if (AArmament* EquippedArmament = CombatComponent->GetArmament())
	{
		SetCurrentMontage(EquippedArmament->GetMontage(Montage_Unequip));
	}

	return true;
}


void UUnequipArmament::HandleUnequipArmamentLogic()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while sheathing the weapons!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}

	bool bUnequippedSecondary = false;
	bool bUnequippedPrimary = false;

	if (AArmament* PrimaryArmament = CombatComponent->GetArmament(true))
	{
		bUnequippedPrimary = PrimaryArmament->SheatheArmament();
	}

	if (AArmament* SecondaryArmament = CombatComponent->GetArmament(false))
	{
		bUnequippedSecondary = SecondaryArmament->SheatheArmament();
	}

	bUnequippedArmaments = bUnequippedPrimary || bUnequippedSecondary;
}


bool UUnequipArmament::ApplyUnequippedStateInformation(AArmament* EquippedArmament)
{
	return true;
}


UCombatComponent* UUnequipArmament::GetCombatComponent() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the character while unequipping the weapons!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return nullptr;
	}
	
	return Character->GetCombatComponent();
}
