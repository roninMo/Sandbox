// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeCombatAbility.h"
#include "MeleeAttack.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;
class UAbilityTask_WaitInputRelease;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_TargetOverlap;


/**
 * 
 */
UCLASS()
class SANDBOX_API UMeleeAttack : public UMeleeCombatAbility
{
	GENERATED_BODY()

protected: // TODO: Either adjust the ability task limit, or create additional tasks that handle everything together -> Default task limit per game 1000 -> 10 for 100 characters / enemies
	/** The attack montage handle */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_PlayMontageAndWait* AttackMontageHandle;

	/** The input released handle */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitInputRelease* InputReleasedHandle;

	/**** Attack trace logic ****/
	/** The handle that traces for overlaps during the attack animation */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_TargetOverlap* MeleeOverlapHandle;

	// I don't want to add tags to the character's state, attack frames are specific to the attack
	/** The handle for the beginning and ending of attack frames logic */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayEvent* AttackFramesHandle;
	
	// /** The handle for when attack frames end during an attack */
	// UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayTagRemoved* AttackFramesEndHandle;
	//
	// /** The handle for when attack frames begin during an attack */
	// UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayTagAdded* AttackFramesBeginHandle;

	/** The handle for the beginning and ending of attack frames logic */
	// UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayTagState* AttackFramesHandle;
	
	
public:
	UMeleeAttack();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:
	/** Input released event for multiplayer replication */
	UFUNCTION() virtual void OnInputReleased(float TimeHeld);


	/**
	 * Handles begin tracing logic for targets for a specific weapon, and checks if we're already overlapping with any targets
	 *
	 * @remarks Blueprint functions need to add the melee overlap task right now, or subclass this function to activate the overlap logic (if it hasn't already been activated)
	 *
	 * @param bRightHand						Whether we should be tracing for the left or right hand armament
	 */
	virtual void OnBeginAttackFrames_Implementation(bool bRightHand) override;
	
	/**
	 * Handles end tracing for targets for a specific weapon
	 *
	 * @remarks Blueprint functions need to remove the melee overlap task right now, or subclass this function to end the task
	 * 
	 * @param bRightHand						Whether we should be tracing for the left or right hand armament
	 */
	virtual void OnEndAttackFrames_Implementation(bool bRightHand) override;

	
	/** This is a delegate binding for gameplay event information that's sent to this character during this task */
	UFUNCTION(BlueprintCallable) virtual void OnEndOfMontage();

	
};
