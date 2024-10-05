// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/MeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Sandbox/Asc/Tasks/AbilityTask_TargetOverlap.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Logging/StructuredLog.h"

#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "Sandbox/Data/Enums/ArmamentTypes.h"

UMeleeAttack::UMeleeAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_MeleeAttack));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Unequipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Equipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Sliding));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));

	AllowMovementTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AllowMovement);
	AttackFramesTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames);
}


bool UMeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacterBase* Character = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return nullptr;
	}
	
	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if (!CombatComponent)
	{
		return false;
	}

	// The ability still needs to retrieve the weapon information
	if (AttackPattern == EInputAbilities::None)
	{
		return true;
	}


	// Check if there's a valid weapon for this specific ability (This is here to prevent spamming replicated events) // TODO: Find out if this is actually beneficial
	bool bCanActivateAbility = false;
	if (IsRightHandAbilityInput(AttackPattern)
		&& CombatComponent->GetArmament()
		&& CombatComponent->GetArmament()->GetEquipStatus() == EEquipStatus::Equipped)
	{
		bCanActivateAbility = true;
	}
	else if (!IsRightHandAbilityInput(AttackPattern)
		&& CombatComponent->GetArmament(false)
		&& CombatComponent->GetArmament(false)->GetEquipStatus() == EEquipStatus::Equipped)
	{
		bCanActivateAbility = true;
	}

	return bCanActivateAbility;
}


void UMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	// Check for weapon updates
	if (!SetComboAndArmamentInformation())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	
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
	HandleAttackStateTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(Tag_State_Attacking),
		nullptr,
		false,
		false
	);
	HandleAttackStateTask->EventReceived.AddDynamic(this, &UMeleeAttack::OnHandleAttackState);
	HandleAttackStateTask->ReadyForActivation();
	
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
	// Allow movement logic
	// TODO: Prevent movement on the client, the ability doesn't need to handle this because the client accumulates the aggregated tags
	// The player controllers still can use gameplay tags to create their logic, just be careful to remove tags safely so it doesn't affect other characters

	// Attack logic
	if (EventData.EventTag.MatchesTag(AttackFramesTag))
	{
		MeleeOverlapTaskHandle->ReadyForActivation();
		CheckAndAttackIfAlreadyOverlappingAnything(AlreadyHitActors);
	}

	UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} is attacking, state: {3}",
		*UEnum::GetValueAsString(GetAvatarActorFromActorInfo()->GetLocalRole()), *GetNameSafe(GetAvatarActorFromActorInfo()), *EventData.EventTag.ToString());
}


void UMeleeAttack::OnLandedAttack(const FGameplayAbilityTargetDataHandle& TargetData, UAbilitySystem* TargetAsc)
{
	AlreadyHitActors.AddUnique(TargetAsc->GetAvatarActor());
	HandleMeleeAttack(TargetData, TargetAsc);
}


void UMeleeAttack::OnEndOfAttack() { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); }
void UMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (InputReleasedHandle) InputReleasedHandle->EndTask();
	if (AttackMontageTaskHandle) AttackMontageTaskHandle->EndTask();
	if (HandleLandedAttackTask) HandleLandedAttackTask->EndTask();
	if (HandleAttackStateTask) HandleAttackStateTask->EndTask();
	if (MeleeOverlapTaskHandle) MeleeOverlapTaskHandle->EndTask();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UMeleeAttack::SetAttackMontage(AArmament* Weapon)
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the character while retrieving the weapon's montage!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}
	
	if (!Weapon)
	{
		UCombatComponent* CombatComponent = GetCombatComponent();
		if (!CombatComponent)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component during {2} while retrieving the montage",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
			return;
		}
	
		Weapon = CombatComponent->GetArmament(IsRightHandAbility());
		if (!Weapon)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the armament during {2} while retrieving the montage",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
			return;
		}
	}

	// Primary Attack
	if (AttackPattern == EInputAbilities::PrimaryAttack || AttackPattern == EInputAbilities::StrongAttack)
	{
		// Handle running attack variations (Just add them to the montage)
		UAdvancedMovementComponent* MovementComponent = Character->GetMovementComp<UAdvancedMovementComponent>();
		if (MovementComponent && MovementComponent->IsRunning() &&
			MovementComponent->GetLastUpdateVelocity().Length() >= MovementComponent->GetMaxWalkSpeed() * MovementComponent->SprintSpeedMultiplier)
		{
			// Running attack
		}
		else
		{
			SetCurrentMontage(Weapon->GetCombatMontage(AttackPattern));
		}
	}
	else
	{
		SetCurrentMontage(Weapon->GetCombatMontage(AttackPattern));
	}
}
