// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Player/GameplayAbility_Aim.h"

#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"


UGameplayAbility_Aim::UGameplayAbility_Aim()
{
	// Non instanced abilities for player aiming logic.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Aim")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Movement.Aiming")));
}


bool UGameplayAbility_Aim::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacterCameraLogic* Character = Cast<ACharacterCameraLogic>(ActorInfo->AvatarActor.Get());
	if (!Character) return false;

	UAdvancedMovementComponent* MovementComponent = Character->GetAdvancedMovementComp();
	if (!MovementComponent || !MovementComponent->AllowedToAim()) return false;
	
	return true;
}


void UGameplayAbility_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacterCameraLogic* Character = Cast<ACharacterCameraLogic>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UAdvancedMovementComponent* MovementComponent = GetMovementComponent(ActorInfo->AvatarActor.Get());
	if (!MovementComponent)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) 
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// Slow the player's movement during aiming // TODO: add different movespeed adjusments based on the player's weapon
	MovementComponent->StartAiming();

	// Adjust the camera settings
	Character->SetCameraFOVInterpSpeed(CameraFovInterpSpeed);
	Character->SetCameraZoom(CameraZoom);
}


void UGameplayAbility_Aim::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}


void UGameplayAbility_Aim::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacterCameraLogic* Character = Cast<ACharacterCameraLogic>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->SetCameraZoom(1);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
