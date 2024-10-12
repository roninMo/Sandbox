// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Movement/Roll/MovementAbility_Roll.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Data/Structs/CombatInformation.h"


UMovementAbility_Roll::UMovementAbility_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_Roll));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_Sprint));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_HitStun));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Dead));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Stunned));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));

	HitStunTag = FGameplayTag::RequestGameplayTag(Tag_State_HitStun);
	InvincibiltyFramesTag = FGameplayTag::RequestGameplayTag(Tag_State_Invincibility);
}


bool UMovementAbility_Roll::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	const ACharacterCameraLogic* Character = Cast<ACharacterCameraLogic>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return false;
	}

	if (!Character->GetAdvancedMovementComp() || !Character->GetRollMontage())
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
	const UMMOAttributeSet* Attributes = Cast<UMMOAttributeSet>(ActorInfo->AbilitySystemComponent->GetAttributeSet(UMMOAttributeSet::StaticClass()));
	if (Attributes && Attributes->GetStamina() == 0)
	{
		return false;
	}
	
	
	return true;
}


void UMovementAbility_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ACharacterCameraLogic* Character = Cast<ACharacterCameraLogic>(ActorInfo->AvatarActor.Get());
	if (!Character || !Character->GetAdvancedMovementComp())
	{
		if (!Character)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the character while trying to roll!",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		}
		else
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the movement component while trying to roll!",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	
	InvincibilityFramesHandle = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, InvincibiltyFramesTag);
	InvincibilityFramesHandle->EventReceived.AddDynamic(this, &UMovementAbility_Roll::InvincibilityFramesState);
	InvincibilityFramesHandle->ReadyForActivation();


	FVector2D Input = Character->GetAdvancedMovementComp()->GetPlayerInput();
	FName MontageSection = Montage_Section_Roll_Forward;
	FName CameraStyle = Character->Execute_GetCameraStyle(Character);

	if (CameraStyle == CameraStyle_FirstPerson)
	{
		MontageSection = GetRollDirection(Input);

	}
	if (CameraStyle == CameraStyle_ThirdPerson)
	{
		if (Character->IsRotationOrientedToCamera())
		{
			MontageSection = GetRollDirection(Input);
		}
		else
		{
			RotateCharacterTowardsMovement(Input, Character);
			MontageSection = Montage_Section_Roll_Forward;
		}
	}
	if (CameraStyle == CameraStyle_TargetLocking)
	{
		RotateCharacterTowardsMovement(Input, Character);
	}
	
	// Attack montage
	MontageHandle = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("RollMontageHandle"),
		Character->GetRollMontage(),
		1.f,
		MontageSection
	);
	MontageHandle->OnCompleted.AddDynamic(this, &UMovementAbility_Roll::OnEndOfMontage);
	MontageHandle->OnCancelled.AddDynamic(this, &UMovementAbility_Roll::OnEndOfMontage);
	MontageHandle->OnBlendOut.AddDynamic(this, &UMovementAbility_Roll::OnEndOfMontage);
	MontageHandle->OnInterrupted.AddDynamic(this, &UMovementAbility_Roll::OnEndOfMontage);
	MontageHandle->ReadyForActivation();
}


void UMovementAbility_Roll::InvincibilityFramesState(FGameplayEventData EventData)
{
	// if (EventData)
}


void UMovementAbility_Roll::OnEndOfMontage() { EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false); }
void UMovementAbility_Roll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UMovementAbility_Roll::RotateCharacterTowardsMovement(const FVector2D& Input, ACharacter* Character)
{
	const FRotator CharacterRotation = Character->GetControlRotation();
	const FVector InputRotation = CharacterRotation.RotateVector(FVector(Input.X, Input.Y, 0));
	FRotator TargetRotation = InputRotation.Rotation();
	TargetRotation.Pitch = 0.0f;
	Character->SetActorRotation(TargetRotation);
}


FName UMovementAbility_Roll::GetRollDirection(FVector2D Input)
{
	if (Input.X == 0 && Input.Y == 0)
	{
		return Montage_Section_Roll_Forward;
	}
	
	const int32 Forwards = GetInputInt(Input.X);
	const int32 Sideways = GetInputInt(Input.Y);

	FString RollDirection = FString();
	if (Forwards == -1) RollDirection.Append(FString("Backward"));
	else if (Forwards == 1) RollDirection.Append(FString("Forward"));

	if (Sideways == -1) RollDirection.Append(FString("Left"));
	else if (Sideways == 1) RollDirection.Append(FString("Right"));
	return FName(RollDirection);
}


int32 UMovementAbility_Roll::GetInputInt(const float VectorAxisValue)
{
	if (.3f <= VectorAxisValue) return 1;
	if (-.3f <= VectorAxisValue && VectorAxisValue <= .3f) return 0;
	if (VectorAxisValue <= -.3f) return -1;
	return 0;
}
