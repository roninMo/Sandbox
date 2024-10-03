// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"
#include "MeleeAttack.generated.h"

class UAbilityTask_WaitInputRelease;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_TargetOverlap;
/**
 * 
 */
UCLASS()
class SANDBOX_API UMeleeAttack : public UCombatAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitInputRelease* InputReleasedHandle;
	UPROPERTY(BlueprintReadWrite) UAbilityTask_PlayMontageAndWait* AttackMontageTaskHandle;
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayEvent* HandleAttackStateTask;
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayEvent* HandleLandedAttackTask;
	UPROPERTY(BlueprintReadWrite) UAbilityTask_TargetOverlap* MeleeOverlapTaskHandle;

	
public:
	UMeleeAttack();
	
	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Input released event for multiplayer replication */
	UFUNCTION() virtual void OnInputReleased(float TimeHeld);
	
	/** This is a delegate binding that listens for gameplay events for transitioning between different states of the attack (Charging, Attacking) */
	UFUNCTION(BlueprintCallable) virtual void OnHandleAttackState(FGameplayEventData EventData);

	/** This is a delegate binding for attacks that's sent to this character during this task */
	UFUNCTION(BlueprintCallable) virtual void OnLandedAttack(const FGameplayAbilityTargetDataHandle& TargetData, UAbilitySystem* TargetAsc);
	
	/** This is a delegate binding for gameplay event information that's sent to this character during this task */
	UFUNCTION(BlueprintCallable) virtual void OnEndOfAttack();

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	
};
