// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Sandbox/Data/Structs/AbilityInformation.h" // Potential dependency error
#include "CharacterAbilityDataSet.generated.h"


// struct FGameplayEffectInfo;
// struct FGameplayAttributeInfo;
// struct FGameplayAbilityInfo;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;




/**
 * Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct FCharacterAbilityDataSetHandle
{
	GENERATED_BODY()

	/** Handles to the granted abilities */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> Abilities;

	/** Handles to the granted gameplay effects. */
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;

	/** Handles adding and removing attributes. After the gameplay effect has been added, it then retrieves the effect handle without creating another handle
	 * We're using Gameplay effects for handling attributes because I don't want to create another attribute set every time we add attributes, that would be too complex
	 * Adding attributes the normal way is better, and the values are actually being replicated to the client. You should use instant effects for attribute adjustments, and infinite for stats adjustments
	 * Treat this as something that's reset on spawn, and save the handles for the time that happens, because it isn't replicated to the client, and that seems to slip devs minds while they explore gameplay ability scenarios
	 * Anyways it'll be alright, you just don't need to use attribute logic for adding attributes unless you're handling combat
	 */
	TArray<FActiveGameplayEffectHandle> Attributes;
	
	/** Pointers to the granted attribute sets */
	// UPROPERTY()
	// TArray<TObjectPtr<UAttributeSet>> Attributes;

	/** Copy of the tag container that was used for OwnedTags */
	UPROPERTY()
	FGameplayTagContainer OwnedTags;

	/** DisplayName of the Ability Set represented by this handle (stored here for debug reason) */
	UPROPERTY(BlueprintReadOnly, Category="Ability Sets")
	FString AbilitySetPathName;

	/** List of delegate that may have been registered to handle input binding when the ability is given on client */
	TArray<FDelegateHandle> InputBindingDelegateHandles;

	/** Default constructor */
	FCharacterAbilityDataSetHandle() = default;

	/** Returns whether the handle is valid by checking the original Ability Set pathname */
	bool IsValid() const
	{
		return !AbilitySetPathName.IsEmpty();
	}

	/** Cleans up the handle structure for any stored handles and pathname*/
	void Invalidate()
	{
		AbilitySetPathName = TEXT("");
		Abilities.Empty();
		EffectHandles.Empty();
		Attributes.Empty();
		OwnedTags.Reset();
	}

	/** Returns a String representation of the Ability Set handle */
	FString ToString(const bool bVerbose = false) const
	{
		TArray<FString> Results;
		Results.Add(FString::Printf(
			TEXT("AbilitySetPathName: %s, Abilities Handles: %d, Effect Handles: %d, Attribute Sets: %d, Owned Tags: %d"),
			*AbilitySetPathName,
			Abilities.Num(),
			EffectHandles.Num(),
			Attributes.Num(),
			OwnedTags.Num()
		));

		// No verbose output, only print high lvl info
		if (!bVerbose)
		{
			return FString::Join(Results, LINE_TERMINATOR);
		}

		Results.Add(FString::Printf(TEXT("Abilities Handles: %d"), Abilities.Num()));
		for (const FGameplayAbilitySpecHandle& AbilityHandle : Abilities)
		{
			Results.Add(FString::Printf(TEXT("\t - Ability Handle: %s"), *AbilityHandle.ToString()));
		}

		Results.Add(FString::Printf(TEXT("Effect Handles: %d"), EffectHandles.Num()));
		for (const FActiveGameplayEffectHandle& EffectHandle : EffectHandles)
		{
			Results.Add(FString::Printf(TEXT("\t - Effect Handle: %s"), *EffectHandle.ToString()));
		}
		
		Results.Add(FString::Printf(TEXT("Attribute Sets: %d"), Attributes.Num()));
		// for (const UAttributeSet* AttributeSet : Attributes)
		// {
		// 	Results.Add(FString::Printf(TEXT("\t - Attribute Set: %s"), *GetNameSafe(AttributeSet)));
		// }
		for (const FActiveGameplayEffectHandle& EffectHandle : EffectHandles)
		{
			Results.Add(FString::Printf(TEXT("\t - Effect Handle: %s"), *EffectHandle.ToString())); // I don't know how many times I've gotten false positives here, however it's good to explore scenarios
		}

		Results.Add(FString::Printf(TEXT("Owned Tags: %d"), OwnedTags.Num()));
		Results.Add(FString::Printf(TEXT("\t - Owned Tags: %s"), *OwnedTags.ToStringSimple()));

		return FString::Join(Results, LINE_TERMINATOR);
	}

	friend bool operator==(const FCharacterAbilityDataSetHandle& LHS, const FCharacterAbilityDataSetHandle& RHS)
	{
		return LHS.AbilitySetPathName == RHS.AbilitySetPathName;
	}

	friend bool operator!=(const FCharacterAbilityDataSetHandle& LHS, const FCharacterAbilityDataSetHandle& RHS)
	{
		return !(LHS == RHS);
	}
};




/**
 * DataAsset that can be used to define and give to an AbilitySystemComponent a set of:
 *
 * - Abilities, with optional enhanced input binding
 * - Attribute Sets
 * - Gameplay Effects
 * - Owned Tags
 *
 * @note This is stolen from the gameplay ability system companion in order to learn how to add game features for quickly creating abilities/attributes/etc for different games
 * And a lot of the utility functions are beneficial and make things less complicated
 */
UCLASS(BlueprintType)
class SANDBOX_API UCharacterAbilityDataSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UCharacterAbilityDataSet() = default;

	/** List of Gameplay Abilities to grant when the Ability System Component is initialized */
	UPROPERTY(EditDefaultsOnly, Category="Abilities", meta=(TitleProperty="{InputId}: {Ability}"))
	TArray<FGameplayAbilityInfo> GrantedAbilities;

	/** List of Attribute Sets to grant when the Ability System Component is initialized, with optional initialization data */
	UPROPERTY(EditDefaultsOnly, Category="Attributes", meta=(TitleProperty=Effect))
	// TArray<FGameplayAttributeInfo> GrantedAttributes;
	TArray<FGameplayEffectInfo> GrantedAttributes;

	/** List of GameplayEffects to apply when the Ability System Component is initialized (typically on begin play) */
	UPROPERTY(EditDefaultsOnly, Category="Effects", meta=(TitleProperty=Effect))
	TArray<FGameplayEffectInfo> GrantedEffects;
	
	/** An optional set of Gameplay Tags to grant to the ASC when the Ability Set is applied */
	UPROPERTY(EditDefaultsOnly, Category="Owned Gameplay Tags")
	FGameplayTagContainer OwnedTags;

	/**
	 * Grants itself (Ability Set) to the passed in ASC, adding defined Abilities, Attributes and Effects.
	 *
	 * @param InASC AbilitySystemComponent pointer to operate on
	 * @param OutAbilitySetHandle Handle that can be used to remove the set later on
	 * @param OutErrorText Reason of error in case of failed operation
	 * @param bShouldRegisterComponent Whether the set on successful application should try to register Ability System delegates on Avatar Actor.
	 *
	 * @return True if the ability set was granted successfully, false otherwise
	 */
	bool AddToAbilitySystem(UAbilitySystemComponent* InASC, FCharacterAbilityDataSetHandle& OutAbilitySetHandle, FText* OutErrorText = nullptr, const bool bShouldRegisterComponent = true) const;

	/**
	 * Grants itself (Ability Set) to the passed in actor, adding defined Abilities, Attributes and Effects.
	 *
	 * Actor must implement IAbilitySystemInterface or have an AbilitySystemComponent component.
	 *
	 * @param InActor Actor (with an ASC) pointer to operate on
	 * @param OutAbilitySetHandle Handle that can be used to remove the set later on
	 * @param OutErrorText Reason of error in case of failed operation
	 *
	 * @return True if the ability set was granted successfully, false otherwise
	 */
	bool AddToAbilitySystem(const AActor* InActor, FCharacterAbilityDataSetHandle& OutAbilitySetHandle, FText* OutErrorText = nullptr) const;

	/**
	 * Removes the AbilitySet represented by InAbilitySetHandle from the passed in ASC. Clears out any previously granted Abilities,
	 * Attributes and Effects from the set.
	 * 
	 * @param InASC AbilitySystemComponent pointer to operate on
	 * @param InAbilitySetHandle Handle of the Ability Set to remove
	 * @param OutErrorText Reason of error in case of failed operation
	 * @param bShouldRegisterComponent Whether the set on successful application should try to register Ability System delegates on Avatar Actor.
	 * 
	 * @return True if the ability set was removed successfully, false otherwise
	 */
	static bool RemoveFromAbilitySystem(UAbilitySystemComponent* InASC, FCharacterAbilityDataSetHandle& InAbilitySetHandle, FText* OutErrorText = nullptr, const bool bShouldRegisterComponent = true);

	/**
	 * Removes the AbilitySet represented by InAbilitySetHandle from the passed in actor. Clears out any previously granted Abilities,
	 * Attributes and Effects from the set.
	 * 
	 * Actor must implement IAbilitySystemInterface or have an AbilitySystemComponent component.
	 * 
	 * @param InActor Actor (with an ASC) pointer to operate on
	 * @param InAbilitySetHandle Handle of the Ability Set to remove
	 * @param OutErrorText Reason of error in case of failed operation
	 * 
	 * @return True if the ability set was removed successfully, false otherwise
	 */
	static bool RemoveFromAbilitySystem(const AActor* InActor, FCharacterAbilityDataSetHandle& InAbilitySetHandle, FText* OutErrorText = nullptr);

	/** Returns whether this Ability Set needs Input Binding, eg. does any of the Granted Abilities in this set have a defined Input Action to bind */
	bool HasInputBinding() const;

protected:

	/** For avatar actors with a GSCCoreComponent, make sure to notify we may have added attributes, and register delegates for those */
	static void TryRegisterCoreComponentDelegates(UAbilitySystemComponent* InASC);
	
	/** For avatar actors with a GSCCoreComponent, makes sure to cleanup delegates */
	static void TryUnregisterCoreComponentDelegates(UAbilitySystemComponent* InASC);
};

