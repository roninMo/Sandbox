// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"
#include "MeleeAttack.generated.h"

enum class EAttackFramesState : uint8;
class UAbilityTask_ApplyRootMotionConstantForce;
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

protected: // TODO: Either adjust the ability task limit, or create additional tasks that handle everything together
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
	
	
	/**** Combat logic ****/
	UPROPERTY(Transient, BlueprintReadWrite) TArray<AActor*> PrimaryHitActors;
	UPROPERTY(Transient, BlueprintReadWrite) TArray<AActor*> SecondaryHitActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") bool bUseCrouchingAttacks = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") bool bUseRunningAttacks = true;
	
	/** Whether this attack is a crouching attack */
	UPROPERTY(Transient, BlueprintReadWrite) bool bCrouchingAttack;
	
	/** Whether this attack is a running attack */
	UPROPERTY(Transient, BlueprintReadWrite) bool bRunningAttack;

	UPROPERTY(Transient, BlueprintReadWrite) F_ComboAttacks CrouchingAttackInformation;
	UPROPERTY(Transient, BlueprintReadWrite) F_ComboAttacks RunningAttackInformation;

	
	/**** If we're dual wielding, we handle each weapon's attack frames independently ****/
	UPROPERTY(Transient, BlueprintReadWrite) EAttackFramesState PrimaryAttackState;
	UPROPERTY(Transient, BlueprintReadWrite) EAttackFramesState SecondaryAttackState;

	
	/**** Cached tags ****/
	/** The stamina cost's gameplay effect tag */
	UPROPERTY(Transient, BlueprintReadWrite) FGameplayTag StaminaCostEffectTag;
	
	/** When we should allow rotation movement during the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag HitStunTag;
	
	/** When we should allow rotation movement during the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AllowMovementTag;
	
	/** When we should actually trace for enemies during an attack animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AttackFramesTag;

	/** The tag to notify the end of attack frames for a specific animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AttackFramesEndTag;
	
	/** The tag to notify the end of attack frames for a specific left hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag LeftHandAttackFramesEndTag;

	/** The tag to notify the end of attack frames for a specific right hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag RightHandAttackFramesEndTag;

	/** The tag to notify the begin of attack frames for a specific animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AttackFramesBeginTag;

	/** The tag to notify the begin of attack frames for a specific left hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag LeftHandAttackFramesBeginTag;

	/** The tag to notify the begin of attack frames for a specific right hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag RightHandAttackFramesBeginTag;

	
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

	/** Function to retrieve gameplay events for handling attack frames during the attack montage */
	UFUNCTION(BlueprintCallable) virtual void OnAttackFrameEvent(FGameplayEventData EventData);

	/** Begin tracing for targets during the attack frames. For attacks with multiple attack frames, this should either recreate the task or decide when overlap traces are valid */
	UFUNCTION(BlueprintCallable) virtual void OnBeginAttackFrames(bool bRightHand);
	
	/** Event for when the attack frames of the current attack are done */
	UFUNCTION(BlueprintCallable) virtual void OnEndAttackFrames(bool bRightHand);

	/** This is a delegate binding for attacks that's sent to this character during this task */
	UFUNCTION(BlueprintCallable) virtual void OnOverlappedTarget(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc);
	
	/** This is a delegate binding for gameplay event information that's sent to this character during this task */
	UFUNCTION(BlueprintCallable) virtual void OnEndOfMontage();

protected:
	/**** Combat functions ****/
	/** Retrieves the armament's combat information for this ability. Only updates if the player switches weapons or updates his stance */
	virtual bool SetComboAndArmamentInformation() override;
	
	/** Retrieves all the necessary information for an attack. Call this during ActivateAbility to retrieve the attack information for the current attack */
	virtual void InitCombatInformation() override;

	/** Increments the combo index based on the combo attacks */
	virtual void SetComboIndex() override;
	
	/** Calculates the current attack information */
	virtual void SetComboAttack() override;

	/** Retrieves the attack montage from the armament based on different conditions */
	virtual void SetAttackMontage(AArmament* Weapon) override;
	
	/** Calculates the montage section for the current combo attack */
	virtual void SetMontageStartSection(bool ChargeAttack = false) override;

	
};
