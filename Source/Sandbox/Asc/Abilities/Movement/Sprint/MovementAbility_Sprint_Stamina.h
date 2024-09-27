// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/Movement/MovementAbility.h"
#include "MovementAbility_Sprint_Stamina.generated.h"

class UAbilityTask_WaitInputRelease;
class UAbilityTask_WaitAttributeChangeThreshold;


/**
 * 
 */
UCLASS()
class SANDBOX_API UMovementAbility_Sprint_Stamina : public UMovementAbility
{
	GENERATED_BODY()

protected:
	/** Sprint cost handle */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitAttributeChangeThreshold* StaminaDrainHandle;

	/** The input released replication event  */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitInputRelease* InputReleasedHandle;

	
public:
	UMovementAbility_Sprint_Stamina();

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Input released event for multiplayer replication */
	UFUNCTION() virtual void OnInputReleased(float TimeHeld);
	
	/** Function event for when the task detects the player is out of stamina */
	UFUNCTION() virtual void OnOutOfStamina(bool bMatchesComparison, float CurrentValue);
	
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

	
};
