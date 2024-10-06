// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitGameplayTagState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitGameplayTagQueryDelegate, bool, bWhenTrue);


/**
 * 
 */
UCLASS()
class SANDBOX_API UAbilityTask_WaitGameplayTagState : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	/**
	 * 	Wait until the given gameplay tag query has become true or false, based on TriggerCondition. 
	 *  By default this will look at the owner of this ability. OptionalExternalTarget can be set to
	 *  make this look at another actor's tags for changes.  If the the tag query already satisfies 
	 *  the query when this task is started, it will immediately broadcast the Triggered 
	 *  event. It will keep listening as long as bOnlyTriggerOnce = false.
	 *
	 *  Once the tag condition is true, it then checks if the condition is false, and vice versa
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitGameplayTagState* WaitGameplayTagState(UGameplayAbility* OwningAbility, 
																   const FGameplayTagQuery TagQuery, 
																   const AActor* InOptionalExternalTarget = nullptr, 
																   const bool bOnlyTriggerOnce = false);
	
	/** This delegate will be triggered when the trigger condition has been met. */
	UPROPERTY(BlueprintAssignable)
	FWaitGameplayTagQueryDelegate UpdatedState;
	
	
protected:
	/** Activates this AbilityTask. */
	virtual void Activate() override;
	
	/** Sets the external target that this node whould use for checking tags. */
	void SetExternalTarget(const AActor* Actor);

	/** This will update the tags in the TargetTags container to reflect that tags that are on the target ASC. */
	UFUNCTION()
	void UpdateTargetTags(const FGameplayTag Tag, int32 NewCount);

	/** This will evaluate the TargetTags using the given TagQuery, executing the Trigger delegate if needed. */
	void EvaluateTagQuery();

	/** This gets the ASC to use to listen to tag changed events. */
	UAbilitySystemComponent* GetTargetASC();

	/** This will handle cleaning up any registered delegates. */
	virtual void OnDestroy(bool AbilityIsEnding) override;

	/** This is the tag query to evaluate for triggering this node. */
	FGameplayTagQuery TagQuery;

	/** Indicates if the callbacks for tag changes have been registered. */
	bool bRegisteredCallbacks = false;

	/** This was the last result of evaluating the TagQuery. */
	bool bQueryState = false;

	/** This is the tag container of the targets the target ASC currently has. 
		NOTE: This will only contain the tags referenced in the TagQuery. */
	FGameplayTagContainer TargetTags;

	/** This is the handles to the tag changed delegate for each gameplay tag in the TagQuery. */
	TMap<FGameplayTag, FDelegateHandle> TagHandleMap;
	
	/** This is the optional external target to use when getting the ASC to get tags from. */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget = nullptr;

	/** This is true when the tag query has returned true, and set to false once the condition is then false. */
	bool bHasTagBeenAdded;

	/** This indicates if we should use the external target. */
	bool bUseExternalTarget = false;	

	/** This indicates if this node should only trigger once, or any number of times. */
	bool bOnlyTriggerOnce = false;
};