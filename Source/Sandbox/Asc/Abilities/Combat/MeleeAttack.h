// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"
#include "MeleeAttack.generated.h"

class UAbilityTask_WaitGameplayTagQuery;
class UAbilityTask_WaitGameplayTagAdded;
class UAbilityTask_WaitGameplayTagRemoved;
class UAbilityTask_WaitInputRelease;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_TargetOverlap;


/**
 * 
 */
UCLASS()
class SANDBOX_API UMeleeAttack : public UCombatAbility
{
	GENERATED_BODY()

protected: // TODO: Either adjust the ability task limit, or create additional tasks that handle everything together
	/** The attack montage handle */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_PlayMontageAndWait* AttackMontageHandle;

	/** The input released handle */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitInputRelease* InputReleasedHandle;

	/**** Attack trace logic ****/
	/** The handle that traces for overlaps during the attack animation */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_TargetOverlap* MeleeOverlapHandle;

	/** The handle for when attack frames end during an attack */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayTagRemoved* AttackFramesEndHandle;

	/** The handle for when attack frames begin during an attack */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayTagAdded* AttackFramesBeginHandle;

	
	/**** Cached tags ****/
	/** When we should allow rotation movement during the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AllowMovementTag;
	
	/** When we should actually trace for enemies during an attack animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AttackFramesTag;

	
public:
	UMeleeAttack();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	/** Input released event for multiplayer replication */
	UFUNCTION() virtual void OnInputReleased(float TimeHeld);
	
	/** Begin tracing for targets during the attack frames. For attacks with multiple attack frames, this should either recreate the task or decide when overlap traces are valid */
	UFUNCTION(BlueprintCallable) virtual void OnBeginAttackFrames();
	
	/** Event for when the attack frames of the current attack are done */
	UFUNCTION(BlueprintCallable) virtual void OnEndAttackFrames();

	/** This is a delegate binding for attacks that's sent to this character during this task */
	UFUNCTION(BlueprintCallable) virtual void OnOverlappedTarget(const FGameplayAbilityTargetDataHandle& TargetData, UAbilitySystem* TargetAsc);
	
	/** This is a delegate binding for gameplay event information that's sent to this character during this task */
	UFUNCTION(BlueprintCallable) virtual void OnEndOfMontage();

	/** Retrieves the attack montage from the armament based on different conditions */
	virtual void SetAttackMontage(AArmament* Weapon) override;

	
};
