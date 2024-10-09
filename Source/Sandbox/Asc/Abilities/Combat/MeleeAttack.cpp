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
	AttackFramesEndTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_End);
	AttackFramesBeginTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_Begin);
	StaminaCostEffectTag = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Drain_Stamina);
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

	// If we don't have any stamina don't attack
	UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get();
	const UMMOAttributeSet* Attributes = Cast<UMMOAttributeSet>(AbilitySystem->GetAttributeSet(UMMOAttributeSet::StaticClass()));
	if (Attributes && Attributes->GetStamina() == 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Create a gameplay effect for the stamina cost of the current attack
	FName StaminaCostEffect = FName(UEnum::GetValueAsString(AttackPattern).Append("_StaminaCost"));
	UGameplayEffect* StaminaCost = NewObject<UGameplayEffect>(ActorInfo->OwnerActor.Get(), StaminaCostEffect);
	if (StaminaCost)
	{
		StaminaCost->DurationPolicy = EGameplayEffectDurationType::Instant;
		
		FGameplayModifierInfo StaminaDrain = FGameplayModifierInfo();
		StaminaDrain.Attribute = UMMOAttributeSet::GetStaminaAttribute();
		StaminaDrain.ModifierOp = EGameplayModOp::Additive;
		StaminaDrain.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-CurrentAttack.StaminaCost));
		StaminaCost->Modifiers.Add(StaminaDrain);
		
		// UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		// check(AbilitySystemComponent != nullptr);
		// if (!AbilitySystemComponent->CanApplyAttributeModifiers(StaminaCost, GetAbilityLevel(Handle, ActorInfo), MakeEffectContext(Handle, ActorInfo)))
		// {
		// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		// 	return;
		// }

		
		FGameplayEffectSpec* StaminaCostSpec = new FGameplayEffectSpec(StaminaCost, MakeEffectContext(Handle, ActorInfo), 1);
		FGameplayEffectSpecHandle StaminaCostHandle = FGameplayEffectSpecHandle(StaminaCostSpec);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, StaminaCostHandle);
	}

	
	
	// Attack State ->  (Allow movement, begin overlap trace logic, etc.)
	const FGameplayTagQuery AttackFramesQuery = FGameplayTagQuery::BuildQuery(FGameplayTagQueryExpression().AllTagsMatch().AddTag(AttackFramesTag));
	// AttackFramesHandle = UAbilityTask_WaitGameplayTagQuery::WaitGameplayTagQuery(
	// 	this,
	// 	AttackFramesQuery,
	// 	nullptr,
	// 	EWaitGameplayTagQueryTriggerCondition::WhenTrue
	// );
	
	// AttackFramesHandle = UAbilityTask_WaitGameplayTagState::WaitGameplayTagState(this, AttackFramesQuery);
	// AttackFramesHandle->UpdatedState.AddDynamic(this, &UMeleeAttack::OnAttackFramesStateUpdates);
	// AttackFramesHandle->ReadyForActivation();

	// AttackFramesBeginHandle = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, AttackFramesTag);
	// AttackFramesBeginHandle->Added.AddDynamic(this, &UMeleeAttack::OnBeginAttackFrames);
	// AttackFramesBeginHandle->ReadyForActivation();
	//
	// AttackFramesEndHandle = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, AttackFramesTag);
	// AttackFramesEndHandle->Removed.AddDynamic(this, &UMeleeAttack::OnEndAttackFrames);
	// AttackFramesEndHandle->ReadyForActivation();

	AttackFramesHandle = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, AttackFramesTag, nullptr, false, false);
	AttackFramesHandle->EventReceived.AddDynamic(this, &UMeleeAttack::OnAttackFrameEvent);
	AttackFramesHandle->ReadyForActivation();
	
	
	// Overlap Trace ->  Have the server handle the attack logic with client prediction
	MeleeOverlapHandle = UAbilityTask_TargetOverlap::CreateOverlapDataTask(this, Armament->GetArmamentHitboxes());
	MeleeOverlapHandle->OnValidOverlap.AddDynamic(this, &UMeleeAttack::OnOverlappedTarget);
	// MeleeOverlapHandle->ReadyForActivation(); // During attack frames

	
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
}


void UMeleeAttack::OnInputReleased(float TimeHeld)
{
	// Transition out of charging attack!
}


void UMeleeAttack::OnAttackFrameEvent(const FGameplayEventData EventData)
{
	if (EventData.EventTag == AttackFramesEndTag)
	{
		OnEndAttackFrames();
	}
	else if (EventData.EventTag == AttackFramesBeginTag)
	{
		OnBeginAttackFrames();
	}
}


void UMeleeAttack::OnAttackFramesStateUpdates(bool bQueryStateValid)
{
	// If the attack frames have begun
	if (bQueryStateValid)
	{
		OnBeginAttackFrames();

		// TODO: if there's an attack with multiple attack frame locations, add logic for handling recreating the task
	}

	// If the attack frames have ended
	if (!bQueryStateValid)
	{
		OnEndAttackFrames();
	}
}


void UMeleeAttack::OnBeginAttackFrames()
{
	if (MeleeOverlapHandle) MeleeOverlapHandle->ReadyForActivation();
	CheckAndAttackIfAlreadyOverlappingAnything(AlreadyHitActors);
}


void UMeleeAttack::OnEndAttackFrames()
{
	if (MeleeOverlapHandle) MeleeOverlapHandle->EndTask();
}


void UMeleeAttack::OnOverlappedTarget(const FGameplayAbilityTargetDataHandle& TargetData, UAbilitySystem* TargetAsc)
{
	if (AlreadyHitActors.Contains(TargetAsc->GetAvatarActor())) return;

	if (TargetAsc->GetAvatarActor())
	{
		AlreadyHitActors.AddUnique(TargetAsc->GetAvatarActor());
		HandleMeleeAttack(TargetData, TargetAsc);
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

	SetComboIndex();
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
	}

	return bSetComboAndArmamentInformation;
}


void UMeleeAttack::InitCombatInformation()
{
	// Reset state using handle
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


void UMeleeAttack::SetComboAttack()
{
	// Standard attack information
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

	// Charged attacks
	if (ChargeAttack)
	{
		MontageStartSection = FName(MontageStartSection.ToString() + Montage_Section_Charge);
	}
}
