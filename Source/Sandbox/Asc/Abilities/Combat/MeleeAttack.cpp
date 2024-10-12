// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/MeleeAttack.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Sandbox/Asc/Tasks/AbilityTask_TargetOverlap.h"
#include "Logging/StructuredLog.h"

#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Data/Enums/ArmamentTypes.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
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
	if (!SetComboAndArmamentInformation())
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
	AddStaminaCostEffect();
	
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


void UMeleeAttack::OnAttackFrameEvent(const FGameplayEventData EventData)
{
	/**
		One hand / Two hand
			- Attack frames
				- Begin attack frames
				- End attack frames

		At the beginning of the attack we're just searching for frames
		The overlap component mediates attacks without notifying the weapon
		The weapon checks for traces and sends the data to the server before it gets deleted
		Once it's deleted, the replicated data is sent to the server regardless of the current combat logic
		This data is used for combat for the entirety of the attack



			Dual wield attack
				- Attack frames
					- Left hand
						- Begin attack frames
						- End attack frames
					- Right hand
						- Begin attack frames
						- End attack frames

		At the beginning of the attack we handle both weapons, and activate when we find attack frames for one of the weapons
		All the values persist, and we do traces specific to each weapon using only one overlap component (There's a limit to these, and it's okay to be inefficient because this is more fun)
		The weapon checks for traces, calculates combat logic, and sends it to each armament to handle individual attack calculations specific to the trace frame

	*/
		

	if (CurrentStance == EArmamentStance::OneHanding || CurrentStance == EArmamentStance::TwoHanding)
	{
		if (EventData.EventTag.MatchesTag(AttackFramesEndTag))
		{
			OnEndAttackFrames(IsRightHandAbility());
		}
		else if (EventData.EventTag.MatchesTag(AttackFramesBeginTag))
		{
			OnBeginAttackFrames(IsRightHandAbility());
		}
	}
	else
	{
		// Two weapons that aren't dual wielding don't normally attack together, however combat is much more intricate than that, and it's easy to prioritize the logic properly

		
		// End attack frames (war is never over)
		if (EventData.EventTag.MatchesTag(AttackFramesEndTag))
		{
			if (bDebug) UE_LOGFMT(AbilityLog, Log, "{0}::AttackFrames End, {1}", GetOwningActorFromActorInfo()->HasAuthority() ? FString("Server") : FString("Client"), *EventData.EventTag.ToString());
			
			if (EventData.EventTag.MatchesTag(LeftHandAttackFramesEndTag) && SecondaryAttackState == EAttackFramesState::Enabled)
			{
				OnEndAttackFrames(false);
			}
			else if (EventData.EventTag.MatchesTag(RightHandAttackFramesEndTag) && PrimaryAttackState == EAttackFramesState::Enabled)
			{
				OnEndAttackFrames(true);
			}
			// This shouldn't happen, however some auxiliary attacks (like crouching and other ones non specific to attack patterns) could cause it to happen
			else if (EventData.EventTag.MatchesTagExact(AttackFramesEndTag))
			{
				UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Something broke with the attack frame logic! {2}, Stance: {3}, AtkP: {4}, Montage: {5}, EventTag: {6}",
					*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()),
					*UEnum::GetValueAsString(CurrentStance), *UEnum::GetValueAsString(AttackPattern), *GetNameSafe(GetCurrentMontage()), *EventData.EventTag.ToString()
				);
				
				OnEndAttackFrames(IsRightHandAbility());
			}
		}
		// Begin attack frames (I'm always ready)
		else if (EventData.EventTag.MatchesTag(AttackFramesBeginTag))
		{
			if (bDebug) UE_LOGFMT(AbilityLog, Log, "{0}::AttackFrames Begin, {1}", GetOwningActorFromActorInfo()->HasAuthority() ? FString("Server") : FString("Client"), *EventData.EventTag.ToString());
			
			// We're only enabling the frames for one attack, if you have animations with multiple frames you'll have to adjust this logic, specifically for enemies
			if (EventData.EventTag.MatchesTag(LeftHandAttackFramesBeginTag) && SecondaryAttackState == EAttackFramesState::Disabled)
			{
				OnBeginAttackFrames(false);
			}
			else if (EventData.EventTag.MatchesTag(RightHandAttackFramesBeginTag) && PrimaryAttackState == EAttackFramesState::Disabled)
			{
				OnBeginAttackFrames(true);
			}
			// This shouldn't happen, however some auxiliary attacks (like crouching and other ones non specific to attack patterns) could cause it to happen
			else if (EventData.EventTag.MatchesTagExact(AttackFramesBeginTag))
			{
				UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Something broke with the attack frame logic! {2}, Stance: {3}, AtkP: {4}, Montage: {5}, EventTag: {6}",
					*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()),
					*UEnum::GetValueAsString(CurrentStance), *UEnum::GetValueAsString(AttackPattern), *GetNameSafe(GetCurrentMontage()), *EventData.EventTag.ToString()
				);

				OnBeginAttackFrames(IsRightHandAbility());
			}
		}
	}
}


void UMeleeAttack::OnBeginAttackFrames(bool bRightHand)
{
	// Update the attack frames state (and capture the previous attack frame state to properly capture frame one calculations
	const bool bOverlapPreviouslyDisabled = PrimaryAttackState == EAttackFramesState::Disabled && SecondaryAttackState == EAttackFramesState::Disabled;
	if (!bRightHand) SecondaryAttackState = EAttackFramesState::Enabled;
	else PrimaryAttackState = EAttackFramesState::Enabled;
	
	
	// Initialize the attack frames if it hasn't been activated yet
	if (bOverlapPreviouslyDisabled)
	{
		if (MeleeOverlapHandle && !MeleeOverlapHandle->IsActive())
		{
			MeleeOverlapHandle->ReadyForActivation();
		}
	}


	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component to check for already traced enemies the beginning of the attack frames!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return;
	}
	
	if (!bRightHand)
	{
		CheckAndAttackIfAlreadyOverlappingAnything(CombatComponent->GetArmament(false), SecondaryHitActors);
	}
	else
	{
		CheckAndAttackIfAlreadyOverlappingAnything(CombatComponent->GetArmament(), PrimaryHitActors);
	}
}


void UMeleeAttack::OnEndAttackFrames(bool bRightHand)
{
	if (!bRightHand) SecondaryAttackState = EAttackFramesState::Finished;
	else PrimaryAttackState = EAttackFramesState::Finished;

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




void UMeleeAttack::OnOverlappedTarget(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc)
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while overlapping a target! Defaulting to one of the armament's hitboxes!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
	}

	if (bDebug)
	{
		UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} OnOverlapped event, checking if they've already attacked the player!",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(OverlappedArmament));
	}

	
	const bool bRightHandArmament = CombatComponent && OverlappedArmament == CombatComponent->GetArmament(false) ? false : true;
	TArray<AActor*>& ArmamentHitActors = !bRightHandArmament ? SecondaryHitActors : PrimaryHitActors;
	if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons)
	{
		// Check if there's attack frames for the current attack
		const EAttackFramesState& AttackFramesState = bRightHandArmament ? PrimaryAttackState : SecondaryAttackState;
		if ((!bRightHandArmament && AttackFramesState == EAttackFramesState::Enabled) ||
			(bRightHandArmament && AttackFramesState == EAttackFramesState::Enabled))
		{
			if (ArmamentHitActors.Contains(TargetAsc->GetAvatarActor()))
			{
				if (bDebug)
				{
					UE_LOGFMT(AbilityLog, Log, "{0} already overlapping with {1}", *GetNameSafe(OverlappedArmament), *GetNameSafe(TargetAsc->GetAvatarActor()));
				}
				
				return;
			}

			// Handle the attack logic
			if (TargetAsc->GetAvatarActor())
			{
				ArmamentHitActors.AddUnique(TargetAsc->GetAvatarActor());
				HandleMeleeAttack(TargetData, OverlappedArmament, TargetAsc);
			}
		}
		else if (bDebug)
		{
			UE_LOGFMT(AbilityLog, Log, "{0} invalid attack frame state: {1}, rightHandArmament({2}): {3}", *GetNameSafe(OverlappedArmament),
				*UEnum::GetValueAsString(AttackFramesState), bRightHandArmament ? *FString("true") : *FString("false"), GetNameSafe(CombatComponent->GetArmament(bRightHandArmament)));
		}
	}
	else
	{
		// Don't attack twice, just continue // TODO: this should be handled on the overlap task to prevent extra replication
		if (ArmamentHitActors.Contains(TargetAsc->GetAvatarActor()))
		{
			return;
		}

		// Handle the attack logic
		if (TargetAsc->GetAvatarActor())
		{
			ArmamentHitActors.AddUnique(TargetAsc->GetAvatarActor());
			HandleMeleeAttack(TargetData, OverlappedArmament, TargetAsc);
		}
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




bool UMeleeAttack::SetComboAndArmamentInformation()
{
	bool bSetComboAndArmamentInformation = Super::SetComboAndArmamentInformation();
	if (!Armament || !bSetComboAndArmamentInformation)
	{
		CrouchingAttackInformation = F_ComboAttacks();
		RunningAttackInformation = F_ComboAttacks();
		return bSetComboAndArmamentInformation;
	}


	const F_ArmamentInformation& ArmamentInformation = Armament->GetArmamentInformation();
	for (const auto& [Ability, InputId, InvalidStances, Level] : ArmamentInformation.MeleeAbilities)
	{
		if (AttackPattern != InputId || InvalidStances.Contains(CurrentStance)) continue;

		// crouching and running attacks
		CrouchingAttackInformation = Armament->GetComboAttacks(EInputAbilities::Crouch);
		RunningAttackInformation = Armament->GetComboAttacks(EInputAbilities::Sprint);
		return bSetComboAndArmamentInformation;
	}

	return bSetComboAndArmamentInformation;
}


void UMeleeAttack::InitCombatInformation()
{
	// Reset state using handle
	PrimaryAttackState = EAttackFramesState::Disabled;
	SecondaryAttackState = EAttackFramesState::Disabled;
	bCrouchingAttack = false;
	bRunningAttack = false;

	
	// Handle running / crouching attack variations
	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	UAdvancedMovementComponent* MovementComponent = Character ? Character->GetMovementComp<UAdvancedMovementComponent>() : nullptr;
	if (Character && MovementComponent)
	{
		// Check if it's a crouching attack
		if (MovementComponent->IsCrouching() && bUseCrouchingAttacks)
		{
			bCrouchingAttack = true;
		}

		// Check if it's a running attack
		if (MovementComponent->IsWalking() && bUseRunningAttacks &&
			MovementComponent->GetLastUpdateVelocity().Length() >= MovementComponent->GetMaxWalkSpeed() * MovementComponent->SprintSpeedMultiplier)
		{
			bRunningAttack = true;
		}
	}
	
	
	Super::InitCombatInformation();
	// Attack information
	// Attack Montage
	// Montage start section
	// Damage calculations
}


void UMeleeAttack::SetComboIndex()
{
	if (!Armament || (!bRunningAttack && !bCrouchingAttack))
	{
		Super::SetComboIndex();
		return;
	}
	
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while adjusting the combo index",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return;
	}

	
	// Either have separate combo indexing for normal / strong attacks, or when transitioning from one to the other have that finish the combo, both combined would be interesting I just think it'd be tough to balance
	const TArray<F_ComboAttack>& Attacks = bRunningAttack ? RunningAttackInformation.ComboAttacks : CrouchingAttackInformation.ComboAttacks; 
	if (ComboIndex + 1 >= Attacks.Num())
	{
		CombatComponent->SetComboIndex(0);
	}
	else
	{
		CombatComponent->SetComboIndex(ComboIndex + 1);
	}
}


void UMeleeAttack::SetComboAttack()
{
	// Standard attack information
	SecondaryHitActors.Empty();
	PrimaryHitActors.Empty();
	if (!Armament || (!bRunningAttack && !bCrouchingAttack))
	{
		Super::SetComboAttack();
		return;
	}


	// Retrieve the current attack
	const TArray<F_ComboAttack>& Attacks = bRunningAttack ? RunningAttackInformation.ComboAttacks : CrouchingAttackInformation.ComboAttacks; 
	if (ComboCount <= 1 || ComboIndex + 1 >= Attacks.Num())
	{
		if (!Attacks.IsEmpty())
		{
			CurrentAttack = Attacks[0];
		}
	}
	else
	{
		if (Attacks.IsValidIndex(ComboIndex))
		{
			CurrentAttack = Attacks[ComboIndex];
		}
		else
		{
			CurrentAttack = Attacks.Num() > 0 ? Attacks[0] : F_ComboAttack();
		}
	}
}


void UMeleeAttack::SetAttackMontage(AArmament* Weapon)
{
	if (!bRunningAttack && !bCrouchingAttack)
	{
		Super::SetAttackMontage(Weapon);
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

	if (bCrouchingAttack)
	{
		SetCurrentMontage(Weapon->GetCombatMontage(EInputAbilities::Crouch));
	}
	if (bRunningAttack)
	{
		SetCurrentMontage(Weapon->GetCombatMontage(EInputAbilities::Sprint));
	}
}


void UMeleeAttack::SetMontageStartSection(bool ChargeAttack)
{
	if (!bRunningAttack && !bCrouchingAttack)
	{
		Super::SetMontageStartSection(ChargeAttack);
		return;
	}
	
	if (bCrouchingAttack)
	{
		MontageStartSection = Montage_Section_CrouchAttack;
	}

	if (bRunningAttack)
	{
		MontageStartSection = Montage_Section_RunningAttack;
	}
}
