// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TargetOverlap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityTask_TargetOverlapDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle, UAbilitySystem*, TargetAsc);


/**
 * TODO: This is ongoing target data that hasn't been tested on whether this sends multiple target data values, and that those values are getting mixed together in the AbilityTargetDataMap
 *		- Test that this sends multiple targets to an ability, and successfully applies damage to all the enemies (slow down the attack a bunch and check that prediction keys are staying valid and it's working
 *		- Idk how to test this, and we don't have TDD in place yet, but we need to make sure that multiple target data mappings are being uniquely added to the AbilityTargetDataMap
 *				(it's based on the ability and prediction key, which we're not making multiple of) -> this should be an easy fix though
 */
UCLASS()
class SANDBOX_API UAbilityTask_TargetOverlap : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	/** Retrieves the target's overlap data from the client and replicates it to the server */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "Get Target Overlap Data", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_TargetOverlap* CreateOverlapDataTask(UGameplayAbility* OwningAbility, TArray<UPrimitiveComponent*> CollisionComponents, bool bDebug = false);

	/** Delegate for when the overlap data has been replicated to the server */
	UPROPERTY(BlueprintAssignable)
	FAbilityTask_TargetOverlapDataSignature OnValidOverlap;

	
protected:
	/** The component that we're using for overlaps */
	UPROPERTY(BlueprintReadWrite) TArray<UPrimitiveComponent*> OverlapComponents;
	
	/** Whether we should debug the task information */
	bool bDebugTask;

	
protected:
	/** Called to trigger the actual task once the delegates have been set up */
	virtual void Activate() override;

	/** Overlap event function to capture a primitive object's overlap event */	
	UFUNCTION() void OnTraceOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Once the input information has made it to the server, retrieves the input information */
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	/** Unbind the component overlap of this armament */
	virtual void OnDestroy(bool AbilityEnded) override;
	
	
};
