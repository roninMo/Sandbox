// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterGameplayAbility.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameplayCue_Types.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"


UCharacterGameplayAbility::UCharacterGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Never stray from this config unless you're handling ai or it's a single player game. GA information persists between activations, so be mindful about how you save information
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}


void UCharacterGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	DebugActivateAbility();
	if (bHasBlueprintActivate)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbility();
	}
	else if (bHasBlueprintActivateFromEvent)
	{
		if (TriggerEventData)
		{
			// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
			K2_ActivateAbilityFromEvent(*TriggerEventData);
		}
		else
		{
			UE_LOG(LogAbilitySystem, Warning, TEXT("Ability %s expects event data but none is being supplied. Use Activate Ability instead of Activate Ability From Event."), *GetName());
			EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		}
	}

	if (!Asc)
	{
		UE_LOGFMT(AbilityLog, Warning, "{0}() {1} Ability system component pointer missing during %s activating the startup effects", *FString(__FUNCTION__), *GetNameSafe(ActorInfo->OwnerActor.Get()));
		ABILITY_LOG(Log, TEXT("%s() Ability system component pointer missing during %s activating the startup effects"), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Apply the OngoingEffectsToApplyOnStart values
	const FGameplayEffectContextHandle EffectContext = Asc->MakeEffectContext();
	for (auto GameplayEffect : OngoingEffectsToApplyOnStart)
	{
		if (!GameplayEffect.Get()) continue;

		FGameplayEffectSpecHandle SpecHandle = Asc->MakeOutgoingSpec(GameplayEffect, GetAbilityLevel(), EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = Asc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!ActiveGEHandle.WasSuccessfullyApplied()) ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
		}
	}

	// This is for instanced abilities, store the handle effect to the array
	if (IsInstantiated())
	{
		for (auto gameplayEffect : OngoingEffectsToRemoveOnEnd)
		{
			if (!gameplayEffect.Get()) continue;

			FGameplayEffectSpecHandle specHandle = Asc->MakeOutgoingSpec(gameplayEffect, GetAbilityLevel(), EffectContext);
			if (specHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = Asc->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());
				if (ActiveGEHandle.WasSuccessfullyApplied()) RemoveOnEndEffectHandles.Add(ActiveGEHandle);
				if (!ActiveGEHandle.WasSuccessfullyApplied()) ABILITY_LOG(Log, TEXT("Instanced Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(gameplayEffect));
			}
		}
	}
}


void UCharacterGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}


void UCharacterGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Remove the active gameplay effects for the OngoingEffectsToRemoveOnEnd array
	if (Asc)
	{
		for (const FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndEffectHandles)
		{
			Asc->RemoveActiveGameplayEffect(ActiveEffectHandle);
		}
	}

	// DebugEndAbility();
	OnAbilityEnded.Broadcast(this);
	ClearTasksAtEndOfAbility();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UCharacterGameplayAbility::HandleTagsAtEndOfAbility()
{
	// Handle any tags that you want to add/remove at the end of an ability here
}


void UCharacterGameplayAbility::ClearTasksAtEndOfAbility()
{
	EndTaskInstanceNames.Append(TrackedTasks);
	EndOrCancelTasksByInstanceName();
	TrackedTasks.Empty();
}


FGameplayEffectSpecHandle UCharacterGameplayAbility::PrepExecCalcForTarget(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	TSubclassOf<UGameplayEffect> ExecCalcClass, float GameplayEffectLevel) const
{
	if (HasAuthority(&ActivationInfo) == false && UAbilitySystemGlobals::Get().ShouldPredictTargetGameplayEffects() == false)
	{
		// Early out to avoid making effect specs that we can't apply
		return FGameplayEffectSpecHandle();
	}

	// This batches all created cues together
	FScopedGameplayCueSendContext GameplayCueSendContext;

	if (ExecCalcClass == nullptr)
	{
		ABILITY_LOG(Error, TEXT("PrepExecCalcForTarget called on ability %s with no GameplayEffect."), *GetName());
	}
	else if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, ExecCalcClass, GameplayEffectLevel);
	}

	return FGameplayEffectSpecHandle();
}


TArray<FActiveGameplayEffectHandle> UCharacterGameplayAbility::ApplyExecCalcToTarget(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayAbilityTargetDataHandle& Target, const FGameplayEffectSpecHandle& ExecCalcHandle, int32 Stacks) const
{
	TArray<FActiveGameplayEffectHandle> EffectHandles;

	// This batches all created cues together
	FScopedGameplayCueSendContext GameplayCueSendContext;

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (ExecCalcHandle.Data.IsValid())
		{
			ExecCalcHandle.Data->StackCount = Stacks;

			SCOPE_CYCLE_UOBJECT(Source, ExecCalcHandle.Data->GetContext().GetSourceObject());
			EffectHandles.Append(ApplyGameplayEffectSpecToTarget(Handle, ActorInfo, ActivationInfo, ExecCalcHandle, Target));
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UBaseGameplayAbility::ApplyGameplayEffectToTarget failed to create valid spec handle. Ability: %s"), *GetPathName());
		}
	}

	return EffectHandles;
}


FGameplayEffectSpecHandle UCharacterGameplayAbility::BP_PrepExecCalcForTarget(TSubclassOf<UGameplayEffect> ExecCalcClass, float GameplayEffectLevel)
{
	return PrepExecCalcForTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		ExecCalcClass,
		GameplayEffectLevel
	);
}


TArray<FActiveGameplayEffectHandle> UCharacterGameplayAbility::BP_ApplyExecCalcToTarget(const FGameplayAbilityTargetDataHandle& Target, const FGameplayEffectSpecHandle& ExecCalcHandle, int32 Stacks)
{
	return ApplyExecCalcToTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		Target,
		ExecCalcHandle,
		Stacks
	);
}




#pragma region Input Evocation Events
void UCharacterGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	K2_InputReleased(Handle, ActivationInfo);
}


void UCharacterGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	K2_InputPressed(Handle, ActivationInfo);
}
#pragma endregion 




#pragma region Utility
void UCharacterGameplayAbility::AddGameplayTag(const FGameplayTag& Tag) const
{
	Asc->AddLooseGameplayTag(Tag);
	Asc->AddReplicatedLooseGameplayTag(Tag);
}


void UCharacterGameplayAbility::RemoveGameplayTag(const FGameplayTag& Tag, const bool EveryTag) const
{
	if (!Asc) return;
	if (EveryTag)
	{
		while(Asc->HasMatchingGameplayTag(Tag))
		{
			Asc->RemoveLooseGameplayTag(Tag);
			Asc->RemoveReplicatedLooseGameplayTag(Tag);
		}
	}
	else
	{
		if (Asc->HasMatchingGameplayTag(Tag))
		{
			Asc->RemoveLooseGameplayTag(Tag);
			Asc->RemoveReplicatedLooseGameplayTag(Tag);
		}
	}
}


void UCharacterGameplayAbility::DebugActivateAbility()
{
}


void UCharacterGameplayAbility::DebugEndAbility()
{
}


void UCharacterGameplayAbility::PrintInformation(const FString& Message) const
{
}
#pragma endregion


