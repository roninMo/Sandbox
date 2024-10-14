// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_CombatInputReplication.generated.h"

enum class EInputAbilities : uint8;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatInputCombinationPressed, float, TimeWaited);


USTRUCT()
struct FCombatInputReplicationInformation
{
	GENERATED_BODY()

	FDelegateHandle Handle;
	bool InputPressed;
	
};


/**
 *	Input replication events for combat abilities to retrieve input events from the player's attack patterns.
 *	I only have this valid to be used on input abilities specific to attack patterns,
 *	However this is useful for things like certain special attacks that require an input combination, and you want to attack using the primary attack button
 *	TODO: add handling for input events that haven't been finished yet, if it's required
 */
UCLASS()
class SANDBOX_API UAbilityTask_CombatInputReplication : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	/** Retrieves the target's mouse input data from the client and replicates it to the server */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "Combat Input Replication Events", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_CombatInputReplication* CombatInputReplication(UGameplayAbility* OwningAbility, EInputAbilities InputPattern, bool bTestAlreadyPressed = true, bool bDebug = false);

	/** Replication event for when the player presses the input combination for the primary attack */
	UPROPERTY(BlueprintAssignable) FCombatInputCombinationPressed OnPrimaryAttackInputPressed;

	/** Replication event for when the player presses the input combination for the primary attack */
	UPROPERTY(BlueprintAssignable) FCombatInputCombinationPressed OnSecondaryAttackInputPressed;
	
	/** Replication event for when the player presses the input combination for the primary attack */
	UPROPERTY(BlueprintAssignable) FCombatInputCombinationPressed OnSpecialAttackInputPressed;
	
	/** Replication event for when the player presses the input combination for the primary attack */
	UPROPERTY(BlueprintAssignable) FCombatInputCombinationPressed OnStrongAttackInputPressed;


protected:
	float StartTime;

	/** Check if the input has already been pressed at the beginning of the task */
	bool bTestInitialState;
	
	/** Whether we should debug the task information */
	bool bDebugTask;

	/** The attack pattern of the current ability */
	EInputAbilities AttackPattern;
	
	FCombatInputReplicationInformation PrimaryAttackHandle;
	FCombatInputReplicationInformation SecondaryAttackHandle;
	FCombatInputReplicationInformation SpecialAttackHandle;
	FCombatInputReplicationInformation StrongAttackHandle;

	
protected:
	/** Called to trigger the actual task once the delegates have been set up */
	virtual void Activate() override;

	UFUNCTION() void OnPrimaryAttackCallback();
	UFUNCTION() void OnSecondaryAttackCallback();
	UFUNCTION() void OnSpecialAttackCallback();
	UFUNCTION() void OnStrongAttackCallback();


	
};
