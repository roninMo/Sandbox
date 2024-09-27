// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Movement/Sprint/MovementAbility_Sprint.h"

#include "AbilitySystemGlobals.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
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
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) 
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	MovementComponent->StartSprinting();
	if (StaminaDrainWhileSprinting)
	{
		const UGameplayEffect* SprintDrainEffect = StaminaDrainWhileSprinting->GetDefaultObject<UGameplayEffect>();
		StaminaDrainHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, SprintDrainEffect, 1);
	}
}


void UMovementAbility_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void UMovementAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (StaminaDrainHandle.IsValid())
	{
		UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		if (!AbilitySystemComponent)
		{
			AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(ActorInfo->OwnerActor.Get());
			if (!AbilitySystemComponent)
			{
				UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to find the ability system component to remove the sprint's stamina drain!",
					HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) ? FString("Authority") : FString("Client"), *FString(__FUNCTION__), *GetNameSafe(ActorInfo->OwnerActor.Get())
				);
			}
		}

		AbilitySystemComponent->RemoveActiveGameplayEffect(StaminaDrainHandle); // TODO: Find out whether this is safe
	}

	UAdvancedMovementComponent* MovementComponent = GetMovementComponent(ActorInfo->AvatarActor.Get());
	if (MovementComponent)
	{
		MovementComponent->StopSprinting();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
