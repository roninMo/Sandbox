// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAbilityDataSet.h"

#include "AbilitySystemGlobals.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"

#define LOCTEXT_NAMESPACE "CharacterAbilityDataSet"


bool UCharacterAbilityDataSet::AddToAbilitySystem(UAbilitySystemComponent* InASC, FCharacterAbilityDataSetHandle& OutAbilitySetHandle, FText* OutErrorText, const bool bShouldRegisterComponent) const
{
	if (!IsValid(InASC))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_ASC", "ASC is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}
	
	const bool bSuccess = UGameplayAbilityUtilities::TryAddAbilitySet(InASC, this, OutAbilitySetHandle);

	if (bShouldRegisterComponent)
	{
		TryRegisterCoreComponentDelegates(InASC);
	}
	
	return bSuccess;
}

bool UCharacterAbilityDataSet::AddToAbilitySystem(const AActor* InActor, FCharacterAbilityDataSetHandle& OutAbilitySetHandle, FText* OutErrorText) const
{
	if (!IsValid(InActor))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_Actor", "Passed in actor is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
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

		UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}

	return AddToAbilitySystem(ASC, OutAbilitySetHandle, OutErrorText);
}

bool UCharacterAbilityDataSet::RemoveFromAbilitySystem(UAbilitySystemComponent* InASC, FCharacterAbilityDataSetHandle& InAbilitySetHandle, FText* OutErrorText, const bool bShouldRegisterComponent)
{
	if (!IsValid(InASC))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_ASC", "ASC is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}
	
	// Make sure to notify we may have added attributes, this will shutdown all previously bound delegates to current attributes
	// Delegate registering will be called once again once we're done removing this set
	if (bShouldRegisterComponent)
	{
		TryUnregisterCoreComponentDelegates(InASC);
	}
	
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : InAbilitySetHandle.Abilities)
	{
		if (!AbilitySpecHandle.IsValid())
		{
			continue;
		}
		
		// Only Clear abilities on authority, on end if currently active, or right away
		if (InASC->IsOwnerActorAuthoritative())
		{
			InASC->SetRemoveAbilityOnEnd(AbilitySpecHandle);
		}
	}

	// Remove Effects
	for (const FActiveGameplayEffectHandle& EffectHandle : InAbilitySetHandle.EffectHandles)
	{
		if (EffectHandle.IsValid())
		{
			InASC->RemoveActiveGameplayEffect(EffectHandle);
		}
	}
	
	// Remove Attributes
	// for (UAttributeSet* AttributeSet : InAbilitySetHandle.Attributes) InASC->RemoveSpawnedAttribute(AttributeSet);
	for (const FActiveGameplayEffectHandle& AttributeHandle : InAbilitySetHandle.Attributes)
	{
		InASC->RemoveActiveGameplayEffect(AttributeHandle);
	}

	// Remove Owned Gameplay Tags
	if (InAbilitySetHandle.OwnedTags.IsValid())
	{
		// Remove tags (on server, replicated to all other clients - on owning client, for itself)
		UGameplayAbilityUtilities::RemoveLooseGameplayTagsUnique(InASC, InAbilitySetHandle.OwnedTags);
	}

	// Clear any delegate handled bound previously for this actor
	if (UAbilitySystem* ASC = Cast<UAbilitySystem>(InASC))
	{
		// Clear any delegate handled bound previously for this actor
		for (FDelegateHandle InputBindingDelegateHandle : InAbilitySetHandle.InputBindingDelegateHandles)
		{
			ASC->OnGiveAbilityDelegate.Remove(InputBindingDelegateHandle);
			InputBindingDelegateHandle.Reset();
		}
	}

	// Make sure to re-register delegates, this set may have removed some but other sets may need delegate again
	if (bShouldRegisterComponent)
	{
		TryRegisterCoreComponentDelegates(InASC);
	}

	// Clears out the handle
	InAbilitySetHandle.Invalidate();
	return true;
}

bool UCharacterAbilityDataSet::RemoveFromAbilitySystem(const AActor* InActor, FCharacterAbilityDataSetHandle& InAbilitySetHandle, FText* OutErrorText)
{
	if (!IsValid(InActor))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_Actor", "Passed in actor is nullptr or invalid (pending kill)");
		if (OutErrorText)
		{
			*OutErrorText = ErrorMessage;
		}

		UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
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

		UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}

	return RemoveFromAbilitySystem(ASC, InAbilitySetHandle, OutErrorText);
}

bool UCharacterAbilityDataSet::HasInputBinding() const
{
	for (const FGameplayAbilityInfo& GrantedAbility : GrantedAbilities)
	{
		// Needs binding whenever one of the granted abilities is registered for ability input pressed events
		if (GrantedAbility.InputId != EInputAbilities::None)
		{
			return true;
		}
	}
	
	return false;
}

void UCharacterAbilityDataSet::TryRegisterCoreComponentDelegates(UAbilitySystemComponent* InASC)
{
	check(InASC);

	const AActor* AvatarActor = InASC->GetAvatarActor_Direct();
	if (!IsValid(AvatarActor))
	{
		return;
	}

	UAbilitySystem* AbilitySystem = Cast<UAbilitySystem>(InASC);
	if (!AbilitySystem)
	{
		return;
	}

	AbilitySystem->RegisterAbilitySystemDelegates();
}

void UCharacterAbilityDataSet::TryUnregisterCoreComponentDelegates(UAbilitySystemComponent* InASC)
{
	check(InASC);

	if (!InASC->AbilityActorInfo.IsValid())
	{
		return;
	}

	UAbilitySystem* AbilitySystem = Cast<UAbilitySystem>(InASC);
	if (!AbilitySystem)
	{
		return;
	}

	AbilitySystem->ShutdownAbilitySystemDelegates();
}

