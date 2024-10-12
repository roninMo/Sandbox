// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/Movement/MovementAbility.h"
#include "MovementAbility_Roll.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
/**
 * 
 */
UCLASS()
class SANDBOX_API UMovementAbility_Roll : public UMovementAbility
{
	GENERATED_BODY()

protected:
	/** The roll montage */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_PlayMontageAndWait* MontageHandle;

	/** The handle for the beginning and ending of attack frames logic */
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayEvent* InvincibilityFramesHandle;

	/** Tag for retrieving the invincibility frames during the roll montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag InvincibiltyFramesTag;

	/** Tag for when the player has been attacked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag HitStunTag; 

	
public:
	UMovementAbility_Roll();

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Handle invincibility frames begin and end state */
	UFUNCTION() virtual void InvincibilityFramesState(FGameplayEventData EventData);

	/** End the ability at the end of the montage */
	UFUNCTION() virtual void OnEndOfMontage();

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	/** For third person / target locking movement, rotates the character towards their current movement input (this prevents problems with rm forward roll montages) */
	UFUNCTION(BlueprintCallable) virtual void RotateCharacterTowardsMovement(const FVector2D& Input, ACharacter* Character);

	/** Returns the roll direction */
	UFUNCTION(BlueprintCallable) virtual FName GetRollDirection(FVector2D Input);

	/** Returns the input direction */
	UFUNCTION(BlueprintCallable) virtual int32 GetInputInt(const float VectorAxisValue);
	
};
