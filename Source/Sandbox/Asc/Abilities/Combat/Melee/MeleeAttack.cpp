// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/Melee/MeleeAttack.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Sandbox/Asc/Tasks/AbilityTask_TargetOverlap.h"

#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Data/Enums/ArmamentTypes.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "Sandbox/Data/Enums/AttackFramesState.h"


UMeleeAttack::UMeleeAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_MeleeAttack));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Unequipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Equipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_HitStun));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Sliding));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));
	
	AllowMovementTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AllowMovement);
	AttackFramesTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames);
	HitStunTag = FGameplayTag::RequestGameplayTag(Tag_State_HitStun);
	StaminaCostEffectTag = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Drain_Stamina);
	
	AttackFramesEndTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_End);
	LeftHandAttackFramesEndTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_End_L);
	RightHandAttackFramesEndTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_End_R);
	
	AttackFramesBeginTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_Begin);
	LeftHandAttackFramesBeginTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_Begin_L);
	RightHandAttackFramesBeginTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_Begin_R);
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

	// This prevents clients from activating an ability on the server when a duration is still blocking it on the client. The only problem is that when they activate, the duration and the latency from when it receives the duration
	// Adding values you send across the network that are based on the server fix this, however I'd only doing for crucial information (like attack frames or hit stun)
	if (!ActorInfo->AbilitySystemComponent.Get() || ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(HitStunTag))
	{
		return false;
	}

	// If we don't have any stamina don't attack
	if (IsOutOfStamina(ActorInfo->AbilitySystemComponent.Get()))
	{
		return false;
	}
	
	// The ability still needs to retrieve the weapon information
	if (ShouldActivateAbilityToRetrieveArmament())
	{
		return true;
	}
	
	// Check if there's a valid weapon for this specific ability (This is here to prevent spamming replicated events)
	return IsWeaponEquipped(AttackPattern, CombatComponent);
}


void UMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Check for weapon updates
	if (!SetArmamentInformation())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Retrieve the combat component for overlap logic
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Add the combo information and attack calculations
	InitCombatInformation();

	// Add a gameplay effect for the stamina cost of the current attack
	AddStaminaCostEffect(CurrentAttack.StaminaCost);
	
	// Attack montage
	AttackMontageHandle = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("AttackMontageHandle"),
		GetCurrentMontage(),
		1.f,
		MontageStartSection
	);
	AttackMontageHandle->OnCompleted.AddDynamic(this, &UMeleeAttack::OnEndOfMontage);
	AttackMontageHandle->OnCancelled.AddDynamic(this, &UMeleeAttack::OnEndOfMontage);
	AttackMontageHandle->OnBlendOut.AddDynamic(this, &UMeleeAttack::OnEndOfMontage);
	AttackMontageHandle->OnInterrupted.AddDynamic(this, &UMeleeAttack::OnEndOfMontage);
	AttackMontageHandle->ReadyForActivation();
	
	if (IsPredictingClient() || HasAuthority(&ActivationInfo))
	{
		// Attack State ->  (Allow movement, begin overlap trace logic, etc.)
		AttackFramesHandle = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, AttackFramesTag, nullptr, false, false);
		AttackFramesHandle->EventReceived.AddDynamic(this, &UMeleeAttack::OnAttackFrameEvent);
		AttackFramesHandle->ReadyForActivation();

		
		// Overlap Trace ->  Have the server handle the attack logic with client prediction
		TArray<AArmament*> TracedWeapons = {};
		if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons)
		{
			TracedWeapons.Add(CombatComponent->GetArmament(false));
			TracedWeapons.Add(CombatComponent->GetArmament());
		}
		else if (!IsRightHandAbility()) TracedWeapons.Add(CombatComponent->GetArmament(false));
		else TracedWeapons.Add(CombatComponent->GetArmament());
		
		MeleeOverlapHandle = UAbilityTask_TargetOverlap::CreateOverlapDataTask(this, TracedWeapons);
		MeleeOverlapHandle->OnValidOverlap.AddDynamic(this, &UMeleeAttack::OnOverlappedTarget);
		// MeleeOverlapHandle->ReadyForActivation(); // During attack frames
	}
}


void UMeleeAttack::OnInputReleased(float TimeHeld)
{
	// Transition out of charging attack!
}


void UMeleeAttack::OnBeginAttackFrames_Implementation(bool bRightHand)
{
	// Initialize the attack frames if it hasn't been activated yet
	if (PrimaryAttackState == EAttackFramesState::Disabled && SecondaryAttackState == EAttackFramesState::Disabled)
	{
		if (MeleeOverlapHandle && !MeleeOverlapHandle->IsActive())
		{
			MeleeOverlapHandle->ReadyForActivation();
		}
	}

	Super::OnBeginAttackFrames_Implementation(bRightHand);
}


void UMeleeAttack::OnEndAttackFrames_Implementation(bool bRightHand)
{
	Super::OnEndAttackFrames_Implementation(bRightHand);

	// If the attack frames have finished for the weapons just end the overlap task
	if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons)
	{
		if (MeleeOverlapHandle && PrimaryAttackState == EAttackFramesState::Finished && SecondaryAttackState == EAttackFramesState::Finished)
		{
			MeleeOverlapHandle->EndTask();
		}
	}
	else if (MeleeOverlapHandle)
	{
		MeleeOverlapHandle->EndTask();
	}
}


void UMeleeAttack::OnEndOfMontage() { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); }
void UMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (InputReleasedHandle) InputReleasedHandle->EndTask();
	if (AttackMontageHandle) AttackMontageHandle->EndTask();
	// if (AttackFramesEndHandle) AttackFramesEndHandle->EndTask();
	// if (AttackFramesBeginHandle) AttackFramesBeginHandle->EndTask();
	if (AttackFramesHandle) AttackFramesHandle->EndTask();
	if (MeleeOverlapHandle) MeleeOverlapHandle->EndTask();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
