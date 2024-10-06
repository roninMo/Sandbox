// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Tasks/AbilityTask_WaitGameplayTagState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"


void UAbilityTask_WaitGameplayTagState::Activate()
{
	Super::Activate();

	if (UAbilitySystemComponent* ASC = GetTargetASC())
	{
		// Start the query state to the opposite of the triggered condition so it can be triggered immediately
		bQueryState = false;

		TArray<FGameplayTag> QueryTags;
		TagQuery.GetGameplayTagArray(QueryTags);

		for (const FGameplayTag& Tag : QueryTags)
		{
			if (!TagHandleMap.Contains(Tag))
			{
				UpdateTargetTags(Tag, ASC->GetTagCount(Tag));
				TagHandleMap.Add(Tag, ASC->RegisterGameplayTagEvent(Tag).AddUObject(this, &UAbilityTask_WaitGameplayTagState::UpdateTargetTags));
			}
		}

		EvaluateTagQuery();

		bRegisteredCallbacks = true;
	}
}

void UAbilityTask_WaitGameplayTagState::UpdateTargetTags(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount <= 0)
	{
		TargetTags.RemoveTag(Tag);
	}
	else
	{
		TargetTags.AddTag(Tag);
	}

	if (bRegisteredCallbacks)
	{
		EvaluateTagQuery();
	}
}

void UAbilityTask_WaitGameplayTagState::EvaluateTagQuery()
{
	if (TagQuery.IsEmpty())
	{
		return;
	}

	const bool bMatchesQuery = !TargetTags.IsEmpty() && TagQuery.Matches(TargetTags);
	const bool bStateChanged = bMatchesQuery != bQueryState;
	bQueryState = bMatchesQuery;

	bool bTriggerDelegate = false;
	if (bStateChanged)
	{
		bTriggerDelegate = true;
	}

	if (bTriggerDelegate)
	{
		UpdatedState.Broadcast(bMatchesQuery);
		if (bOnlyTriggerOnce)
		{
			EndTask();
		}
	}
}

void UAbilityTask_WaitGameplayTagState::OnDestroy(bool AbilityIsEnding)
{
	UAbilitySystemComponent* ASC = bRegisteredCallbacks ? GetTargetASC() : nullptr;
	if (ASC != nullptr)
	{
		for (TPair<FGameplayTag, FDelegateHandle> Pair : TagHandleMap)
		{
			if (Pair.Value.IsValid())
			{
				ASC->UnregisterGameplayTagEvent(Pair.Value, Pair.Key);
			}
		}
	}

	TagHandleMap.Empty();
	TargetTags.Reset();

	Super::OnDestroy(AbilityIsEnding);
}

UAbilitySystemComponent* UAbilityTask_WaitGameplayTagState::GetTargetASC()
{	
	return bUseExternalTarget ? ToRawPtr(OptionalExternalTarget) : AbilitySystemComponent.Get();
}

void UAbilityTask_WaitGameplayTagState::SetExternalTarget(const AActor* Actor)
{
	if (Actor != nullptr)
	{
		bUseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

UAbilityTask_WaitGameplayTagState* UAbilityTask_WaitGameplayTagState::WaitGameplayTagState(UGameplayAbility* OwningAbility, 
																						   const FGameplayTagQuery TagQuery, 
																						   const AActor* InOptionalExternalTarget /*= nullptr*/, 
																						   const bool bOnlyTriggerOnce /*= false*/)
{
	UAbilityTask_WaitGameplayTagState* MyObj = NewAbilityTask<UAbilityTask_WaitGameplayTagState>(OwningAbility);
	MyObj->TagQuery = TagQuery;
	MyObj->SetExternalTarget(InOptionalExternalTarget);
	MyObj->bOnlyTriggerOnce = bOnlyTriggerOnce;

	return MyObj;
}

