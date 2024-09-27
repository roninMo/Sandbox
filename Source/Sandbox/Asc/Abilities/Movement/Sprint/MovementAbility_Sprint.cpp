// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Movement/Sprint/MovementAbility_Sprint.h"

#include "AbilitySystemGlobals.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChangeThreshold.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"

UMovementAbility_Sprint::UMovementAbility_Sprint()
{
	// Non instanced abilities for movement component logic
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Sprint")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Movement.Sprinting")));
}


bool UMovementAbility_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UAdvancedMovementComponent* MovementComponent = GetMovementComponent(ActorInfo->AvatarActor.Get());
	if (MovementComponent && MovementComponent->CanSprint()) return true;
	return false;
}


void UMovementAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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

	MovementComponent->StartSprinting();
}


void UMovementAbility_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	if (ActorInfo != nullptr && ActorInfo->AvatarActor != nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}


void UMovementAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAdvancedMovementComponent* MovementComponent = GetMovementComponent(ActorInfo->AvatarActor.Get());
	if (MovementComponent)
	{
		MovementComponent->StopSprinting();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
