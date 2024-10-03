// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/UnequipArmament.h"

#include "EquipArmament.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "Sandbox/Data/Structs/CombatInformation.h"

UUnequipArmament::UUnequipArmament()
{
}


bool UUnequipArmament::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
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
}


void UUnequipArmament::OnEndMontage() { EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false); }
void UUnequipArmament::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bUnequippedArmaments)
	{
		HandleUnequipArmamentLogic();
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
