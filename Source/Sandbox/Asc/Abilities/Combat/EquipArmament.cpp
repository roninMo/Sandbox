// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/EquipArmament.h"

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

UEquipArmament::UEquipArmament()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_Equip));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Equipping));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_HitStun));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Unequipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Equipping));

	EquipEventTag = FGameplayTag::RequestGameplayTag(Tag_Event_Montage_Action);
}


bool UEquipArmament::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while unsheathing the weapons!",
			UEnum::GetValueAsString(ActorInfo->OwnerActor.Get()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(ActorInfo->OwnerActor.Get()));
		return false;
	}

	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while unsheathing the weapons!",
			UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		return false;
	}

	UAdvancedMovementComponent* MovementComponent = Character->GetMovementComp<UAdvancedMovementComponent>();
	if (!MovementComponent || MovementComponent->IsWallClimbing() || MovementComponent->IsMantling() || MovementComponent->IsLedgeClimbing())
	{
		if (!MovementComponent)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the movement component while unsheathing the weapons!",
				UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		}
		return false;
	}

	if (!CombatComponent->GetArmament() && !CombatComponent->GetArmament(false))
	{
		return false;
	}

	bool bCanActivateAbility = false;
	if (CombatComponent->GetArmament() && CombatComponent->GetArmament()->GetEquipStatus() != EEquipStatus::Equipped)
	{
		bCanActivateAbility = true;
	}
	else if (CombatComponent->GetArmament(false) && CombatComponent->GetArmament(false)->GetEquipStatus() != EEquipStatus::Equipped)
	{
		bCanActivateAbility = true;
	}

	return bCanActivateAbility;
}


void UEquipArmament::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent || (!CombatComponent->GetArmament(true) && !CombatComponent->GetArmament(false)))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	
	GetEquipMontage();
	
	// Equip weapon montage
	// ANPC* NPC = Cast<ANPC>(BaseCharacter); // Multicast to clients, call before the ability task
	// if (NPC)
	// {
	// 	NPC->NetMulticast_PlayMontage(CurrentMontage);
	// }

	MontageTaskHandle = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		*GetNameSafe(this),
		GetCurrentMontage(),
		1.f,
		Montage_EquipSection
	);
	MontageTaskHandle->OnCompleted.AddDynamic(this, &UEquipArmament::OnEndMontage);
	MontageTaskHandle->OnCancelled.AddDynamic(this, &UEquipArmament::OnEndMontage);
	MontageTaskHandle->OnBlendOut.AddDynamic(this, &UEquipArmament::OnEndMontage);
	MontageTaskHandle->OnInterrupted.AddDynamic(this, &UEquipArmament::OnEndMontage);
	MontageTaskHandle->ReadyForActivation();

	EquipGameplayTaskHandle = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		EquipEventTag,
		nullptr,
		true,
		true
	);
	EquipGameplayTaskHandle->EventReceived.AddDynamic(this, &UEquipArmament::OnEquipEventReceived);
	EquipGameplayTaskHandle->ReadyForActivation();

	// Add the equip state information
	bEquippedArmaments = false;
}


void UEquipArmament::OnEquipEventReceived(FGameplayEventData EventData)
{
	UnSheatheArmament();
}


void UEquipArmament::OnEndMontage() { EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false); }
void UEquipArmament::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bEquippedArmaments)
	{
		UnSheatheArmament();
	}
	
	// Update the equip status of the armaments
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (CombatComponent)
	{
		if (AArmament* PrimaryArmament = CombatComponent->GetArmament(true))
		{
			PrimaryArmament->SetEquipStatus(EEquipStatus::Equipped);
		}
		
		if (AArmament* SecondaryArmament = CombatComponent->GetArmament(false))
		{
			SecondaryArmament->SetEquipStatus(EEquipStatus::Equipped);
		}
	}

	if (MontageTaskHandle) MontageTaskHandle->EndTask();
	if (EquipGameplayTaskHandle) EquipGameplayTaskHandle->EndTask();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UEquipArmament::UnSheatheArmament()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while unsheathing the weapons!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}

	bool bEquippedSecondary = false;
	bool bEquippedPrimary = false;

	if (AArmament* PrimaryArmament = CombatComponent->GetArmament(true))
	{
		bEquippedPrimary = PrimaryArmament->UnsheatheArmament();
	}

	if (AArmament* SecondaryArmament = CombatComponent->GetArmament(false))
	{
		bEquippedSecondary = SecondaryArmament->UnsheatheArmament();
	}

	bEquippedArmaments = bEquippedPrimary || bEquippedSecondary;
}


bool UEquipArmament::GetEquipMontage()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while equipping the weapon!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return false;
	}

	// TODO: add equip and unequip logic for both armaments
	if (AArmament* EquippedArmament = CombatComponent->GetArmament())
	{
		SetCurrentMontage(EquippedArmament->GetMontage(Montage_Equip));
	}

	return true;
}


UCombatComponent* UEquipArmament::GetCombatComponent() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the character while equipping the weapons!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return nullptr;
	}
	
	return Character->GetCombatComponent();
}
