// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/MeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Tasks/AbilityTask_TargetOverlap.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"

UMeleeAttack::UMeleeAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.MeleeAttack")));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Movement.Sliding")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Movement.Rolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking")));
}


bool UMeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		return false;
	}

	return CombatComponent->GetArmament(true) || CombatComponent->GetArmament(false);
}


void UMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	// access the armament information
	if (!SetComboAndArmamentInformation())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Equip the armament if the player doesn't already have one equipped
	// if (CheckIfShouldEquipArmament())
	// {
	// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	// 	return;
	// }
	
	// Add the combo information and attack calculations
	InitCombatInformation();
	AttackMontageTaskHandle = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("AttackMontageTaskHandle"),
		GetCurrentMontage(),
		1.f,
		MontageStartSection
	);
	AttackMontageTaskHandle->OnCompleted.AddDynamic(this, &UMeleeAttack::OnEndOfAttack);
	AttackMontageTaskHandle->OnCancelled.AddDynamic(this, &UMeleeAttack::OnEndOfAttack);
	AttackMontageTaskHandle->OnBlendOut.AddDynamic(this, &UMeleeAttack::OnEndOfAttack);
	AttackMontageTaskHandle->OnInterrupted.AddDynamic(this, &UMeleeAttack::OnEndOfAttack);
	AttackMontageTaskHandle->ReadyForActivation();

	// Attack Frames
	// HandleAttackStateTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
	// 	this,
	// 	FGameplayTag::RequestGameplayTag(Tag_State_Attacking),
	// 	nullptr,
	// 	false,
	// 	false
	// );
	// HandleAttackStateTask->EventReceived.AddDynamic(this, &UMeleeAttack::OnHandleAttackState);
	// HandleAttackStateTask->ReadyForActivation();
	
	// Overlap Trace
	// Have the server handle the attack logic with client prediction
	// MeleeOverlapTaskHandle = UAbilityTask_TargetOverlap::CreateOverlapDataTask(this, Armament->GetArmamentHitboxes());
	// MeleeOverlapTaskHandle->OnValidOverlap.AddDynamic(this, &UMeleeAttack::OnLandedAttack);
	// MeleeOverlapTaskHandle->ReadyForActivation(); // During attack frames
}


void UMeleeAttack::OnInputReleased(float TimeHeld)
{
	// Transition out of charging attack!
}


void UMeleeAttack::OnHandleAttackState(FGameplayEventData EventData)
{
	FGameplayTag AttackFramesTag;
	if (EventData.EventTag.MatchesTag(AttackFramesTag))
	{
		MeleeOverlapTaskHandle->ReadyForActivation();
		CheckAndAttackIfAlreadyOverlappingAnything(AlreadyHitActors);
	}
}


void UMeleeAttack::OnLandedAttack(const FGameplayAbilityTargetDataHandle& TargetData, UAbilitySystem* TargetAsc)
{
	AlreadyHitActors.AddUnique(TargetAsc->GetAvatarActor());
	HandleMeleeAttack(TargetData, TargetAsc);
}


void UMeleeAttack::OnEndOfAttack()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (InputReleasedHandle) InputReleasedHandle->EndTask();
	if (AttackMontageTaskHandle) AttackMontageTaskHandle->EndTask();
	if (HandleLandedAttackTask) HandleLandedAttackTask->EndTask();
	if (HandleAttackStateTask) HandleAttackStateTask->EndTask();
	if (MeleeOverlapTaskHandle) MeleeOverlapTaskHandle->EndTask();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
