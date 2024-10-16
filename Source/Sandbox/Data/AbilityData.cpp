// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Data/AbilityData.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbility.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"

#define LOCTEXT_NAMESPACE "AbilityData"


bool UAbilityData::AddAbilityDataToCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAbilityDataHandle& OutAbilityDataHandle, FText* OutErrorText) const
{
	if (!IsValid(InAbilitySystemComponent))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_AbilitySystemComponent", "ASC is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}
	
	// Add Abilities
	int32 AbilitiesIndex = 0;
	for (const FGameplayAbilityInfo& AbilityMapping : GrantedAbilities)
	{
		AbilitiesIndex++;
		
		if (!AbilityMapping.Ability)
		{
			// TODO: Add custom logging for handling logic
			// UE_LOGFMT(AbilityLog, Error, "{0}::{1}'s GrantedAbilities Ability on ability set {2} is not valid at Index {3}",
			// 	*UEnum::GetValueAsString(AbilitySystemComponent->GetOwnerActor()->GetLocalRole()), *GetNameSafe(AbilitySystemComponent->GetOwnerActor()), *GetNameSafe(InAbilitySet), AbilitiesIndex  - 1
			// );
			continue;
		}
	
		// Try to grant the ability
		FGameplayAbilitySpec AbilitySpec;
		FGameplayAbilitySpecHandle AbilityHandle;
		UGameplayAbilityUtilities::TryAddAbility(InAbilitySystemComponent, AbilityMapping, AbilityHandle, AbilitySpec);
		OutAbilityDataHandle.Abilities.Add(AbilityHandle);
	}
	
	// Add Effects
	int32 EffectsIndex = 0;
	for (const FGameplayEffectInfo& Effect : GrantedEffects)
	{
		EffectsIndex++;
		
		if (!Effect.Effect)
		{
			// UE_LOGFMT(AbilityLog, Error, "{0}::{1}'s GrantedEffects EffectType on ability set {2} is not valid at Index {3}",
			// 	*UEnum::GetValueAsString(InAbilitySystemComponent->GetOwnerActor()->GetLocalRole()), *GetNameSafe(InAbilitySystemComponent->GetOwnerActor()), *GetName(), EffectsIndex  - 1
			// );
			continue;
		}
		
		UGameplayAbilityUtilities::TryAddGameplayEffect(InAbilitySystemComponent, Effect.Effect, Effect.Level, OutAbilityDataHandle.EffectHandles);
	}
	
	// Add Owned Gameplay Tags
	if (OwnedTags.IsValid())
	{		
		UGameplayAbilityUtilities::AddLooseGameplayTagsUnique(InAbilitySystemComponent, OwnedTags);
	
		// Store a copy of the tags, so that they can be removed later on from handle
		OutAbilityDataHandle.OwnedTags = OwnedTags;
	}
	
	return true;
}


bool UAbilityData::AddAbilityDataToCharacter(const AActor* InActor, FAbilityDataHandle& OutAbilitySetHandle, FText* OutErrorText) const
{
	if (!IsValid(InActor))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_Actor", "Passed in actor is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
	if (!ASC)
	{
		const FText ErrorMessage = FText::Format(LOCTEXT("Invalid_Actor_ASC", "Unable to get valid ASC from actor {0}"), FText::FromString(GetNameSafe(InActor)));
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}

	return AddAbilityDataToCharacter(ASC, OutAbilitySetHandle, OutErrorText);
}


bool UAbilityData::RemoveAbilityDataFromCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAbilityDataHandle& InAbilityDataHandle, FText* OutErrorText)
{
	if (!IsValid(InAbilitySystemComponent))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_ASC", "ASC is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}
	
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : InAbilityDataHandle.Abilities)
	{
		if (!AbilitySpecHandle.IsValid())
		{
			continue;
		}
		
		// Only Clear abilities on authority, on end if currently active, or right away
		if (InAbilitySystemComponent->IsOwnerActorAuthoritative())
		{
			InAbilitySystemComponent->SetRemoveAbilityOnEnd(AbilitySpecHandle);
		}
	}

	// Remove Effects
	for (const FActiveGameplayEffectHandle& EffectHandle : InAbilityDataHandle.EffectHandles)
	{
		if (EffectHandle.IsValid())
		{
			InAbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle);
		}
	}

	// Remove Owned Gameplay Tags
	if (InAbilityDataHandle.OwnedTags.IsValid())
	{
		// Remove tags (on server, replicated to all other clients - on owning client, for itself)
		UGameplayAbilityUtilities::RemoveLooseGameplayTagsUnique(InAbilitySystemComponent, InAbilityDataHandle.OwnedTags);
	}


	// Clear out the handle
	InAbilityDataHandle.Invalidate();
	return true;
}


bool UAbilityData::RemoveAbilityDataFromCharacter(const AActor* InActor, FAbilityDataHandle& InAbilitySetHandle, FText* OutErrorText)
{
	if (!IsValid(InActor))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_Actor", "Passed in actor is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
	if (!ASC)
	{
		const FText ErrorMessage = FText::Format(LOCTEXT("Invalid_Actor_ASC", "Unable to get valid ASC from actor {0}"), FText::FromString(GetNameSafe(InActor)));
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}

	return RemoveAbilityDataFromCharacter(ASC, InAbilitySetHandle, OutErrorText);
}
