// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Data/AttributeData.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"

#define LOCTEXT_NAMESPACE "AttributeData"


bool UAttributeData::AddAttributesToCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAttributeDataHandle& OutAttributeDataHandle, FText* OutErrorText) const
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
	
	// Add Attributes
	int32 EffectsIndex = 0;
	for (const FGameplayEffectInfo& Attributes : GrantedAttributes)
	{
		EffectsIndex++;
		
		if (!Attributes.Effect)
		{
			// UE_LOGFMT(AbilityLog, Error, "{0}::{1}'s GrantedEffects EffectType on ability set {2} is not valid at Index {3}",
			// 	*UEnum::GetValueAsString(InAbilitySystemComponent->GetOwnerActor()->GetLocalRole()), *GetNameSafe(InAbilitySystemComponent->GetOwnerActor()), *GetName(), EffectsIndex  - 1
			// );
			continue;
		}
		
		UGameplayAbilityUtilities::TryAddGameplayEffect(InAbilitySystemComponent, Attributes.Effect, Attributes.Level, OutAttributeDataHandle.AttributeHandles);
	}
	
	if (CurrentAttributeInformation.Effect)
	{
		UGameplayAbilityUtilities::TryAddGameplayEffect(InAbilitySystemComponent, CurrentAttributeInformation.Effect, CurrentAttributeInformation.Level, OutAttributeDataHandle.AttributeHandles);
	}
	
	return true;
}


bool UAttributeData::AddAttributesToCharacter(const AActor* InActor, FAttributeDataHandle& OutAttributeDataHandle, FText* OutErrorText) const
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

	return AddAttributesToCharacter(ASC, OutAttributeDataHandle, OutErrorText);
}


bool UAttributeData::RemoveAttributesFromCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAttributeDataHandle& InAttributeDataHandle, FText* OutErrorText)
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

	// Remove Effects
	for (const FActiveGameplayEffectHandle& AttributeHandle : InAttributeDataHandle.AttributeHandles)
	{
		if (AttributeHandle.IsValid())
		{
			InAbilitySystemComponent->RemoveActiveGameplayEffect(AttributeHandle);
		}
	}
	
	// Clear out the handle
	InAttributeDataHandle.Invalidate();
	return true;
}


bool UAttributeData::RemoveAttributesFromCharacter(const AActor* InActor, FAttributeDataHandle& InAttributeDataHandle, FText* OutErrorText)
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

	return RemoveAttributesFromCharacter(ASC, InAttributeDataHandle, OutErrorText);
}
