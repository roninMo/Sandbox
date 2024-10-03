// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/EquipArmament.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"

UEquipArmament::UEquipArmament()
{
}


bool UEquipArmament::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}


void UEquipArmament::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
		CurrentMontage,
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
	// ApplyEquippedStateInformation(Armament);
	// ApplyEquippedStateInformation(SecondaryArmament);
}


void UEquipArmament::OnEquipEventReceived(FGameplayEventData EventData)
{
	HandleEquipArmamentLogic();
}


void UEquipArmament::OnEndMontage() { EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false); }
void UEquipArmament::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bEquippedArmaments)
	{
		HandleEquipArmamentLogic();
	}

	if (MontageTaskHandle) MontageTaskHandle->EndTask();
	if (EquipGameplayTaskHandle) EquipGameplayTaskHandle->EndTask();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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

	// TODO: add logic for both armaments
	if (AArmament* EquippedArmament = CombatComponent->GetArmament())
	{
		SetCurrentMontage(EquippedArmament->GetMontage(Montage_Equip));
	}

	return true;
}


void UEquipArmament::HandleEquipArmamentLogic()
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


bool UEquipArmament::ApplyEquippedStateInformation(AArmament* EquippedArmament)
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while applying equipped state information!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()),  *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return nullptr;
	}
	
	const UGameplayEffect* EquippedStateInformation = nullptr; // = EquippedArmament->GetEquippedStateInformation().GetDefaultObject();
	if (EquippedStateInformation)
	{
		const FActiveGameplayEffectHandle AppliedArmamentState = ApplyGameplayEffectToOwner(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			EquippedStateInformation,
			1.f
		);
		
		if (!AppliedArmamentState.WasSuccessfullyApplied())
		{
			UE_LOGFMT(AbilityLog, Error, "{0} failed to add the armament state on unequip! {1} {2}()", *GetNameSafe(GetOwningActorFromActorInfo()), *GetName(), *FString(__FUNCTION__));
			return false;
		}
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
