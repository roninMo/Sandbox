// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeLogic.h"

#include "GameplayEffectExtension.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"

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
	OutExecutionData.SourceAbilitySystem = Cast<UAbilitySystem>(OutExecutionData.Context.GetOriginalInstigatorAbilitySystemComponent());
	OutExecutionData.SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	Data.EffectSpec.GetAllAssetTags(OutExecutionData.SpecAssetTags);

	OutExecutionData.TargetCharacter = Data.Target.AbilityActorInfo->AvatarActor.IsValid() ? Cast<ACharacterBase>(Data.Target.AbilityActorInfo->AvatarActor.Get()) : nullptr;
	OutExecutionData.TargetAbilitySystem = Data.Target.AbilityActorInfo->AbilitySystemComponent.IsValid() ? Cast<UAbilitySystem>(Data.Target.AbilityActorInfo->AbilitySystemComponent.Get()) : nullptr;
	OutExecutionData.TargetPawn = Cast<APawn>(OutExecutionData.TargetCharacter);
	OutExecutionData.TargetController = Data.Target.AbilityActorInfo->PlayerController.IsValid()
		? Data.Target.AbilityActorInfo->PlayerController.Get()
		: OutExecutionData.TargetPawn ? OutExecutionData.TargetPawn->GetController() : nullptr;
	
	if (OutExecutionData.SourceAbilitySystem && OutExecutionData.SourceAbilitySystem->AbilityActorInfo.IsValid())
	{
		// Get the Source actor, which should be the damage causer (instigator)
		if (OutExecutionData.SourceAbilitySystem->AbilityActorInfo->AvatarActor.IsValid())
		{
			// Set the source actor based on context if it's set
			if (OutExecutionData.Context.GetEffectCauser())
			{
				OutExecutionData.SourceCharacter = Cast<ACharacterBase>(OutExecutionData.Context.GetEffectCauser());
			}
			else
			{
				OutExecutionData.SourceCharacter = OutExecutionData.SourceAbilitySystem->AbilityActorInfo->AvatarActor.IsValid()
					? Cast<ACharacterBase>(OutExecutionData.SourceAbilitySystem->AbilityActorInfo->AvatarActor.Get())
					: nullptr;
			}
		}

		OutExecutionData.SourcePawn = Cast<APawn>(OutExecutionData.SourceCharacter);
		OutExecutionData.SourceController = OutExecutionData.SourceAbilitySystem->AbilityActorInfo->PlayerController.IsValid()
			? OutExecutionData.SourceAbilitySystem->AbilityActorInfo->PlayerController.Get()
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

	if (OutExecutionData.SourceCharacter) OutExecutionData.SourceCombatComponent = OutExecutionData.SourceCharacter->GetCombatComponent();
	if (OutExecutionData.TargetCharacter) OutExecutionData.TargetCombatComponent = OutExecutionData.TargetCharacter->GetCombatComponent();
}
