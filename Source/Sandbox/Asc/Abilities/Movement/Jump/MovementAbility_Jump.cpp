// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Movement/Jump/MovementAbility_Jump.h"

#include "GameFramework/Character.h"
#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"


UMovementAbility_Jump::UMovementAbility_Jump()
{
	// Non instanced abilities for movement component logic
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_Jump));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Jumping));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
}


bool UMovementAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return false;
	return true;
}


void UMovementAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		if (Character)
		{
			Character->Jump();
			UAdvancedMovementComponent* MovementComponent = GetMovementComponent(Character);
			if (MovementComponent)
			{
				MovementComponent->StartWallJump();
			}
		}
	}
}


void UMovementAbility_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	if (ActorInfo != nullptr && ActorInfo->AvatarActor != nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}


void UMovementAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->StopJumping();
		UAdvancedMovementComponent* MovementComponent = GetMovementComponent(Character);
		if (MovementComponent)
		{
			MovementComponent->StopWallJump();
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

