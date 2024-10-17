// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "Structs/AbilityInformation.h"
#include "AttributeData.generated.h"

class UAbilitySystemComponent;



/**
 * Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct FAttributeDataHandle
{
	GENERATED_BODY()

	/** Handles to the granted attributes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FActiveGameplayEffectHandle> AttributeHandles;

	/** Default constructor */
	FAttributeDataHandle() = default;

	/** Returns whether the handle is valid by checking the original Ability Set pathname */
	bool IsValid() const
	{
		return AttributeHandles.IsEmpty();
	}

	/** Cleans up the handle structure for any stored handles and pathname*/
	void Invalidate()
	{
		AttributeHandles.Empty();
	}

	/** Returns a String representation of the Ability Set handle */
	FString ToString(const bool bVerbose = false) const
	{
		TArray<FString> Results;
		Results.Add(FString::Printf(TEXT("Attribute Handles: %d"), AttributeHandles.Num()));

		// No verbose output, only print high lvl info
		if (!bVerbose)
		{
			return FString::Join(Results, LINE_TERMINATOR);
		}

		Results.Add(FString::Printf(TEXT("Attribute Handles: %d"), AttributeHandles.Num()));
		for (const FActiveGameplayEffectHandle& AttributeHandle : AttributeHandles)
		{
			Results.Add(FString::Printf(TEXT("\t - Attribute Handle: %s"), *AttributeHandle.ToString()));
		}

		return FString::Join(Results, LINE_TERMINATOR);
	}

	friend bool operator==(const FAttributeDataHandle& LHS, const FAttributeDataHandle& RHS)
	{
		return LHS.AttributeHandles == RHS.AttributeHandles;
	}

	friend bool operator!=(const FAttributeDataHandle& LHS, const FAttributeDataHandle& RHS)
	{
		return LHS.AttributeHandles != RHS.AttributeHandles;
	}
};


/**
 * 
 */
UCLASS()
class SANDBOX_API UAttributeData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** List of Attribute information to apply to the character */
	UPROPERTY(EditDefaultsOnly, Category="Attributes", meta=(TitleProperty=Effect))
	TArray<FGameplayEffectInfo> GrantedAttributes;

	/** The current health, poise, stamina, mana, etc of the character */
	UPROPERTY(EditDefaultsOnly, Category="Attributes", meta=(TitleProperty=Effect))
	FGameplayEffectInfo CurrentAttributeInformation;

	
	/**
	 * Grants itself attribute adjustments and returns a handle of the attributes applied
	 *
	 * @param InAbilitySystemComponent AbilitySystemComponent pointer to operate on
	 * @param OutAttributeDataHandle Handle that can be used to remove the set later on
	 * @param OutErrorText Reason of error in case of failed operation
	 *
	 * @return True if the attribute data was granted successfully, false otherwise
	 */
	bool AddAttributesToCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAttributeDataHandle& OutAttributeDataHandle, FText* OutErrorText = nullptr) const;

	/**
	 * Grants itself attribute adjustments and returns a handle of the attributes applied
	 *
	 * Actor must implement IAbilitySystemInterface or have an AbilitySystemComponent component.
	 *
	 * @param InActor Actor (with an ASC) pointer to operate on
	 * @param OutAttributeDataHandle Handle that can be used to remove the set later on
	 * @param OutErrorText Reason of error in case of failed operation
	 *
	 * @return True if the attribute data was granted successfully, false otherwise
	 */
	bool AddAttributesToCharacter(const AActor* InActor, FAttributeDataHandle& OutAttributeDataHandle, FText* OutErrorText = nullptr) const;

	/**
	 * Removes the applied attributes from the character
	 * 
	 * @param InAbilitySystemComponent AbilitySystemComponent pointer to operate on
	 * @param InAttributeDataHandle Handle of the AbilityDat to remove
	 * @param OutErrorText Reason of error in case of failed operation
	 * 
	 * @return True if the attribute data was removed successfully, false otherwise
	 */
	static bool RemoveAttributesFromCharacter(UAbilitySystemComponent* InAbilitySystemComponent, FAttributeDataHandle& InAttributeDataHandle, FText* OutErrorText = nullptr);

	/**
	 * Removes the applied attributes from the character
	 * 
	 * Actor must implement IAbilitySystemInterface or have an AbilitySystemComponent component.
	 * 
	 * @param InActor Actor (with an ASC) pointer to operate on
	 * @param InAttributeDataHandle Handle of the Ability Set to remove
	 * @param OutErrorText Reason of error in case of failed operation
	 * 
	 * @return True if the attribute data was removed successfully, false otherwise
	 */
	static bool RemoveAttributesFromCharacter(const AActor* InActor, FAttributeDataHandle& InAttributeDataHandle, FText* OutErrorText = nullptr);

	
};
