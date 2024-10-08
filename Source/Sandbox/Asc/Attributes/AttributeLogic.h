// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "AttributeLogic.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AttributeSetLog, Log, All);

class UAbilitySystem;
class UGameplayAbility;
struct FGameplayTagContainer;


/** Structure holding various information to deal with AttributeSet PostGameplayEffectExecute, extracting info from FGameplayEffectModCallbackData */
USTRUCT()
struct FGAttributeSetExecutionData
{
	GENERATED_BODY()

	/** The physical representation of the Source ASC (The ability system component of the instigator that started the whole chain) */
	UPROPERTY()
	TObjectPtr<AActor> SourceActor = nullptr;

	/** The physical representation of the owner (Avatar) for the target we intend to apply to  */
	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;

	/** The ability system component of the instigator that started the whole chain */
	UPROPERTY()
	TObjectPtr<UAbilitySystem> SourceAsc = nullptr;

	/** The ability system component of the target we intend to apply to */
	UPROPERTY()
	TObjectPtr<UAbilitySystem> TargetAsc = nullptr;

	/** Controller associated with the owning actor for the Source ASC (The ability system component of the instigator that started the whole chain) */
	UPROPERTY()
	TObjectPtr<AController> SourceController = nullptr;

	/** Controller associated with the owning actor for the target we intend to apply to */
	UPROPERTY()
	TObjectPtr<AController> TargetController = nullptr;

	/** The physical representation of the Source ASC (The ability system component of the instigator that started the whole chain), as a APawn */
	UPROPERTY()
	TObjectPtr<APawn> SourcePawn = nullptr;

	/** The physical representation of the owner (Avatar) for the target we intend to apply to, as a APawn */
	UPROPERTY()
	TObjectPtr<APawn> TargetPawn = nullptr;

	/** The object this effect was created from. */
	UPROPERTY()
	TObjectPtr<UObject> SourceObject = nullptr;

	/** This tells us how we got here (who / what applied us) */
	FGameplayEffectContextHandle Context;

	/** Combination of spec and actor tags for the captured Source Tags on GameplayEffectSpec creation */
	FGameplayTagContainer SourceTags;

	/** All tags that apply to the gameplay effect spec */
	FGameplayTagContainer SpecAssetTags;

	/** Holds the delta value between old and new, if it is available (for Additive Operations) */
	float DeltaValue;
};


/** The type of modifier operation occuring @note this shouldn't be used and is only for a function argument */
UENUM(BlueprintType)
enum class EModifierOpType : uint8
{
	Multiplicative = 1		UMETA(DisplayName = "Multiply"),
	Override = 3			UMETA(DisplayName="Override"),
	Division = 2			UMETA(DisplayName = "Divide"),
	Additive = 0			UMETA(DisplayName="Add")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostGameplayEffectExecute, FGAttributeSetExecutionData&, Props);


/**
 * The base attribute set and logic for the character. Subclass this to add attributes, and reference functions for retrieving information from player
 */
UCLASS()
class SANDBOX_API UAttributeLogic : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAttributeLogic();

	/** Called when an update to an attribute from an instant gameplay effect occurred. Use this to help respond to attribute updates on the server */
	UPROPERTY(BlueprintAssignable)
	FOnPostGameplayEffectExecute OnPostGameplayEffectExecute;

	
	//~ Begin UAttributeSet interface
	/**
	 *	Called just before a GameplayEffect is executed to modify the base value of an attribute. No more changes can be made.
	 *	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 second +10 movement speed buff.
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	//~ End UAttributeSet interface
	
	
protected:
	/**
	 * Fills out FGAttributeSetExecutionData structure based on provided data.
	 *
	 * @param Data The gameplay effect mod callback data available in attribute sets' PostGameplayEffectExecute
	 * @param OutExecutionData Returned structure with various information extracted from Data (Source / Target Actor, Controllers, etc.)
	 */
	virtual void GetExecutionData(const FGameplayEffectModCallbackData& Data, OUT FGAttributeSetExecutionData& OutExecutionData);
	
	
	
	
};

