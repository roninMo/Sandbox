// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TargetMouseData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityTask_TargetMouseDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);


/**
 * 
 */
UCLASS()
class SANDBOX_API UAbilityTask_TargetMouseData : public UAbilityTask
{
	GENERATED_BODY()

public:
	/** Retrieves the target's mouse input data from the client and replicates it to the server */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "Get Player Mouse Data", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_TargetMouseData* CreateTargetMouseData(UGameplayAbility* OwningAbility, bool bDebug = false);
	
	/** Delegate for when the target input data has been replicated to the server */
	UPROPERTY(BlueprintAssignable) FAbilityTask_TargetMouseDataSignature InputData;


protected:
	/** Whether we should debug the task information */
	bool bDebugTask;

	
protected:
	/** Called to trigger the actual task once the delegates have been set up */
	virtual void Activate() override;

	/** Retrieves the mouse data from the client and sends it to the server */
	void SendMouseData();

	/** Traces where the player is currently looking at for anything */
	FHitResult GetAimDirection(FVector_NetQuantize10 AimLocation, FVector_NetQuantize10 AimForwardVector, float Length) const;

	/** Once the input information has made it to the server, retrieves the input information */
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	
};
