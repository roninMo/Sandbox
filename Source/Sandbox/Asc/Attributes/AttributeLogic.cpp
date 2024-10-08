// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeLogic.h"

#include "GameplayEffectExtension.h"
#include "Sandbox/Asc/AbilitySystem.h"

UAttributeLogic::UAttributeLogic()
{
}


void UAttributeLogic::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FGAttributeSetExecutionData Props;
	GetExecutionData(Data, Props);
	
	OnPostGameplayEffectExecute.Broadcast(Props);
}


void UAttributeLogic::GetExecutionData(const FGameplayEffectModCallbackData& Data, FGAttributeSetExecutionData& OutExecutionData)
{
	OutExecutionData.Context = Data.EffectSpec.GetContext();
	OutExecutionData.SourceAsc = Cast<UAbilitySystem>(OutExecutionData.Context.GetOriginalInstigatorAbilitySystemComponent());
	OutExecutionData.SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	Data.EffectSpec.GetAllAssetTags(OutExecutionData.SpecAssetTags);

	OutExecutionData.TargetActor = Data.Target.AbilityActorInfo->AvatarActor.IsValid() ? Data.Target.AbilityActorInfo->AvatarActor.Get() : nullptr;
	OutExecutionData.TargetAsc = Data.Target.AbilityActorInfo->AbilitySystemComponent.IsValid() ? Cast<UAbilitySystem>(Data.Target.AbilityActorInfo->AbilitySystemComponent.Get()) : nullptr;
	OutExecutionData.TargetPawn = Cast<APawn>(OutExecutionData.TargetActor);
	OutExecutionData.TargetController = Data.Target.AbilityActorInfo->PlayerController.IsValid()
		? Data.Target.AbilityActorInfo->PlayerController.Get()
		: OutExecutionData.TargetPawn ? OutExecutionData.TargetPawn->GetController() : nullptr;
	
	if (OutExecutionData.SourceAsc && OutExecutionData.SourceAsc->AbilityActorInfo.IsValid())
	{
		// Get the Source actor, which should be the damage causer (instigator)
		if (OutExecutionData.SourceAsc->AbilityActorInfo->AvatarActor.IsValid())
		{
			// Set the source actor based on context if it's set
			if (OutExecutionData.Context.GetEffectCauser())
			{
				OutExecutionData.SourceActor = OutExecutionData.Context.GetEffectCauser();
			}
			else
			{
				OutExecutionData.SourceActor = OutExecutionData.SourceAsc->AbilityActorInfo->AvatarActor.IsValid()
					? OutExecutionData.SourceAsc->AbilityActorInfo->AvatarActor.Get()
					: nullptr;
			}
		}

		OutExecutionData.SourcePawn = Cast<APawn>(OutExecutionData.SourceActor);
		OutExecutionData.SourceController = OutExecutionData.SourceAsc->AbilityActorInfo->PlayerController.IsValid()
			? OutExecutionData.SourceAsc->AbilityActorInfo->PlayerController.Get()
			: OutExecutionData.SourcePawn ? OutExecutionData.SourcePawn->GetController() : nullptr;
	}

	OutExecutionData.SourceObject = Data.EffectSpec.GetEffectContext().GetSourceObject();

	// Compute the delta between old and new, if it is available
	OutExecutionData.DeltaValue = 0.f;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		OutExecutionData.DeltaValue = Data.EvaluatedData.Magnitude;
	}
}
