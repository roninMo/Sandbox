// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityInformation.generated.h"

class UInputAction;
class UAttributeSet;
class UDataTable;
class UGameplayEffect;
class UGameplayAbility;
enum class ETriggerEvent : uint8;



/**
 *	An example enum for creating input mappings for abilities using enums with BindAbilityActivationToInputComponent()
 *	// Help with searching for locations of objects ->  GetPathNameSafe(UClass::TryFindTypeSlow<UEnum>("EInputAbilities"))
 *
 *	Dont use inputId of 5, there's probably a specific binding with the ability input system for generic inputs
 */
UENUM(BlueprintType)
enum class EInputAbilities : uint8
{
	None = 0						UMETA(DisplayName = "None"),
	Confirm = 1						UMETA(DisplayName = "Confirm"),
	Cancel = 2						UMETA(DisplayName = "Cancel"),
	Sprint = 3						UMETA(DisplayName = "Sprint"),
	PrimaryAttack = 14				UMETA(DisplayName = "Primary Attack"),
	SecondaryAttack = 15			UMETA(DisplayName = "Secondary Attack"),
	StrongAttack = 16				UMETA(DisplayName = "Strong Attack"),
	SpecialAttack = 17 				UMETA(DisplayName = "Special Attack"),
	Aim = 8							UMETA(DisplayName = "Aim"),
	Reload = 9						UMETA(DisplayName = "Reload"),
	Roll = 12						UMETA(DisplayName = "Roll"),
	Crouch = 10						UMETA(DisplayName = "Crouch"),
	Jump = 11						UMETA(DisplayName = "Jump"),
};

/**

	LT, RT 
	LMB, RMB 
	A, B, X, Y
	Dpad
	Joysticks
	Settings


	

*/



/**
 * An object for linking input actions to input abilities for quick creation and customization of player inputs. Uses an enum for creating the inputs that different abilities bind to
 */
USTRUCT(BlueprintType)
struct FInputActionAbilityMap
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FInputActionAbilityMap(
		const EInputAbilities InputId = EInputAbilities::None,
		const TObjectPtr<UInputAction>& InputAction = nullptr,
		const TArray<ETriggerEvent>& KeyEvents = {}
	): InputId(InputId), InputAction(InputAction), KeyEvents(KeyEvents) {}

	/** The input id of the ability */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EInputAbilities InputId;

	/** The input action for one of the player's inputs.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UInputAction> InputAction;

	/** The input key event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<ETriggerEvent> KeyEvents;
};



/**
 * An object for the old versions of the input system where you used input action bindings for ability inputs
 */
USTRUCT()
struct FInputActionBindingAbilityMap
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FInputActionBindingAbilityMap(const FInputActionBinding& InputAction = FInputActionBinding(), const int32 InputId = 0): InputAction(InputAction), InputId(InputId) {}

	/** The input action binding of the ability */
	FInputActionBinding InputAction;

	/** The input id of the ability */
	int32 InputId = 0;
};




USTRUCT(BlueprintType)
struct FGameplayAbilityInfo
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FGameplayAbilityInfo(
		const TSubclassOf<UGameplayAbility>& Ability = nullptr,
		const int32 Level = 1,
		const EInputAbilities InputId = EInputAbilities::None,
		const TSubclassOf<UObject> ObjectInformation = nullptr,
		const FGuid Id = FGuid::NewGuid()
	) :
		Ability(Ability),
		Level(Level),
		InputId(InputId),
		ObjectInformation(ObjectInformation),
		Id(Id)
	{}

	/** Type of ability to grant */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	TSubclassOf<UGameplayAbility> Ability;

	/** Level to grant the ability at */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	int32 Level;

	/** The input id of the ability */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	EInputAbilities InputId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Ability)
	TSubclassOf<UObject> ObjectInformation;
	
	/** A reference to where we retrieved the ability from. This is usually the id of one of the player's inventory items, or null */
	UPROPERTY(BlueprintReadOnly, Category=Ability)
	FGuid Id;
};



/**
 * 
 */
USTRUCT(BlueprintType)
struct FGameplayAttributeInfo
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FGameplayAttributeInfo(
		const TSoftClassPtr<UAttributeSet>& AttributeSet = nullptr,
		const TSoftObjectPtr<UDataTable>& InitializationData = nullptr
	): AttributeSet(AttributeSet), InitializationData(InitializationData) {}

	/** Attribute Set to grant */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attributes)
	TSoftClassPtr<UAttributeSet> AttributeSet;

	/** Data table referent to initialize the attributes with, if any (can be left unset) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attributes, meta = (RequiredAssetDataTags = "RowStructure=/Script/GameplayAbilities.AttributeMetaData"))
	TSoftObjectPtr<UDataTable> InitializationData;
};




USTRUCT(BlueprintType)
struct FGameplayEffectInfo
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FGameplayEffectInfo() = default;
	// FGameplayEffectInfo(
	// 	const TSoftClassPtr<UGameplayEffect>& EffectType = nullptr,
	// 	const int32 Level = 1
	// ): EffectType(EffectType), Level(Level) {}

	/** Gameplay Effect to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Effect")
	TSubclassOf<UGameplayEffect> Effect;

	/** Level for the Gameplay Effect to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Effect")
	int32 Level = 1;
};



