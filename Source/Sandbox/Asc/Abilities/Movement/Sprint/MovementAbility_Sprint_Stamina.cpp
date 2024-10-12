// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Movement/Sprint/MovementAbility_Sprint_Stamina.h"

#include "Abilities/Tasks/AbilityTask_WaitAttributeChangeThreshold.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Sandbox/Asc/Attributes/DefaultAttributes.h"
#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"

UMovementAbility_Sprint_Stamina::UMovementAbility_Sprint_Stamina()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_Sprint));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Sprinting));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_HitStun));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));
}


bool UMovementAbility_Sprint_Stamina::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UAdvancedMovementComponent* MovementComponent = GetMovementComponent(ActorInfo->AvatarActor.Get());
	if (MovementComponent && MovementComponent->CanSprint()) return true;
	return false;
}


void UMovementAbility_Sprint_Stamina::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAdvancedMovementComponent* MovementComponent = GetMovementComponent(ActorInfo->AvatarActor.Get());
	if (!MovementComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	MovementComponent->StartSprinting();

	// Prevent sprinting if the player's stamina is drained
	StaminaDrainHandle = UAbilityTask_WaitAttributeChangeThreshold::WaitForAttributeChangeThreshold(
		this,
		UDefaultAttributes::GetStaminaAttribute(), 
		EWaitAttributeChangeComparison::LessThanOrEqualTo,
		0.0,
		true
	);
	StaminaDrainHandle->OnChange.AddDynamic(this, &UMovementAbility_Sprint_Stamina::OnOutOfStamina);
	StaminaDrainHandle->ReadyForActivation();

	// Add an input released replication event
	InputReleasedHandle = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	InputReleasedHandle->OnRelease.AddDynamic(this, &UMovementAbility_Sprint_Stamina::OnInputReleased);
	InputReleasedHandle->ReadyForActivation();

}


void UMovementAbility_Sprint_Stamina::OnInputReleased(float TimeHeld)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}


void UMovementAbility_Sprint_Stamina::OnOutOfStamina(bool bMatchesComparison, float CurrentValue)
{
	if (CurrentValue <= 0)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}


void UMovementAbility_Sprint_Stamina::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAdvancedMovementComponent* MovementComponent = GetMovementComponent(ActorInfo->AvatarActor.Get());
	if (MovementComponent)
	{
		MovementComponent->StopSprinting();
	}

	if (StaminaDrainHandle && StaminaDrainHandle->IsActive())
	{
		StaminaDrainHandle->EndTask();
	}

	if (InputReleasedHandle && InputReleasedHandle->IsActive())
	{
		InputReleasedHandle->EndTask();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
