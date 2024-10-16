// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/AbilityInformation.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityData.generated.h"


/**
 * Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct FAbilityDataHandle
{
	GENERATED_BODY()

	/** Handles to the granted abilities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayAbilitySpecHandle> Abilities;

	/** Handles to the granted gameplay effects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FActiveGameplayEffectHandle> EffectHandles;

	/** Copy of the tag container that was used for OwnedTags */
	UPROPERTY()
	FGameplayTagContainer OwnedTags;

	/** Default constructor */
	FAbilityDataHandle() = default;

	/** Returns whether the handle is valid by checking the original Ability Set pathname */
	bool IsValid() const
	{
		return Abilities.IsEmpty() && EffectHandles.IsEmpty();
	}

	/** Cleans up the handle structure for any stored handles and pathname*/
	void Invalidate()
	{
		Abilities.Empty();
		OwnedTags.Reset();
		EffectHandles.Empty();
	}

	/** Returns a String representation of the Ability Set handle */
	FString ToString(const bool bVerbose = false) const
	{
		TArray<FString> Results;
		Results.Add(FString::Printf(
			TEXT("Abilities Handles: %d, Effect Handles: %d, Owned Tags: %d"),
			Abilities.Num(),
			EffectHandles.Num(),
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

		Results.Add(FString::Printf(TEXT("Owned Tags: %d"), OwnedTags.Num()));
		Results.Add(FString::Printf(TEXT("\t - Owned Tags: %s"), *OwnedTags.ToStringSimple()));

		return FString::Join(Results, LINE_TERMINATOR);
	}

	friend bool operator==(const FAbilityDataHandle& LHS, const FAbilityDataHandle& RHS)
	{
		return LHS.Abilities == RHS.Abilities && LHS.EffectHandles == RHS.EffectHandles;
	}

	friend bool operator!=(const FAbilityDataHandle& LHS, const FAbilityDataHandle& RHS)
	{
		return !(LHS.Abilities == RHS.Abilities && LHS.EffectHandles == RHS.EffectHandles);
	}
};


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SANDBOX_API UAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UAbilityData() = default;

	/** List of Gameplay Abilities to grant to the character */
	UPROPERTY(EditDefaultsOnly, Category="Abilities", meta=(TitleProperty="{InputId}: {Ability}"))
	TArray<FGameplayAbilityInfo> GrantedAbilities;

	/** List of Gameplay Effects to apply to the character */
	UPROPERTY(EditDefaultsOnly, Category="Effects", meta=(TitleProperty=Effect))
	TArray<FGameplayEffectInfo> GrantedEffects;
	
	/** An optional set of Gameplay Tags to grant to the ASC when the Ability Set is applied */
	UPROPERTY(EditDefaultsOnly, Category="Owned Gameplay Tags")
	FGameplayTagContainer OwnedTags;

	
	/**
	 * Grants itself the abilities and effects, and returns a handle of everything applied
	 *
	 * @param InAbilitySystemComponent AbilitySystemComponent pointer to operate on
	 * @param OutAbilityDataHandle Handle that can be used to remove the set later on
	 * @param OutErrorText Reason of error in case of failed operation
	 *
	 * @return True if the ability data was granted successfully, false otherwise
	 */
	bool AddAbilityDataToCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAbilityDataHandle& OutAbilityDataHandle, FText* OutErrorText = nullptr) const;

	/**
	 * Grants itself the abilities and effects, and returns a handle of everything applied
	 *
	 * Actor must implement IAbilitySystemInterface or have an AbilitySystemComponent component.
	 *
	 * @param InActor Actor (with an ASC) pointer to operate on
	 * @param OutAbilityDataHandle Handle that can be used to remove the set later on
	 * @param OutErrorText Reason of error in case of failed operation
	 *
	 * @return True if the ability data was granted successfully, false otherwise
	 */
	bool AddAbilityDataToCharacter(const AActor* InActor, FAbilityDataHandle& OutAbilityDataHandle, FText* OutErrorText = nullptr) const;

	/**
	 * Removes the AbilityData's Abilities and Effects represented by InAbilityDataHandle from the passed in Ability System Component. Clears out any previously granted Abilities and Effects from the set.
	 * 
	 * @param InAbilitySystemComponent AbilitySystemComponent pointer to operate on
	 * @param InAbilityDataHandle Handle of the AbilityDat to remove
	 * @param OutErrorText Reason of error in case of failed operation
	 * 
	 * @return True if the ability data was removed successfully, false otherwise
	 */
	static bool RemoveAbilityDataFromCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAbilityDataHandle& InAbilityDataHandle, FText* OutErrorText = nullptr);

	/**
	 * Removes the AbilityData's Abilities and Effects represented by InAbilityDataHandle from the passed in Ability System Component. Clears out any previously granted Abilities and Effects from the set.
	 * 
	 * Actor must implement IAbilitySystemInterface or have an AbilitySystemComponent component.
	 * 
	 * @param InActor Actor (with an ASC) pointer to operate on
	 * @param InAbilitySetHandle Handle of the Ability Set to remove
	 * @param OutErrorText Reason of error in case of failed operation
	 * 
	 * @return True if the ability data was removed successfully, false otherwise
	 */
	static bool RemoveAbilityDataFromCharacter(const AActor* InActor, FAbilityDataHandle& InAbilitySetHandle, FText* OutErrorText = nullptr);

	
};
