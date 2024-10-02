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
 */
UENUM(BlueprintType)
enum class EInputAbilities : uint8
{
	None = 0						UMETA(DisplayName = "None"),
	Confirm = 1						UMETA(DisplayName = "Confirm"),
	Cancel = 2						UMETA(DisplayName = "Cancel"),
	Sprint = 3						UMETA(DisplayName = "Sprint"),
	PrimaryAttack = 4				UMETA(DisplayName = "PrimaryAttack"),
	SecondaryAttack = 5				UMETA(DisplayName = "Secondary Attack"),
	SpecialAttack = 6 				UMETA(DisplayName = "Special Attack"),
	Aim = 7							UMETA(DisplayName = "Aim"),
	Reload = 8						UMETA(DisplayName = "Reload"),
	Crouch = 9						UMETA(DisplayName = "Crouch"),
	Jump = 10						UMETA(DisplayName = "Jump"),
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
struct FGameplayAbilityMapping
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FGameplayAbilityMapping(
		const TSoftClassPtr<UGameplayAbility>& Ability = nullptr,
		const int32 Level = 1,
		const EInputAbilities InputId = EInputAbilities::None,
		const TSubclassOf<UObject> ObjectInformation = nullptr,
		const FGuid Id = FGuid()
	) :
		Ability(Ability),
		Level(Level),
		InputId(InputId),
		ObjectInformation(ObjectInformation),
		Id(Id)
	{}

	/** Type of ability to grant */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	TSoftClassPtr<UGameplayAbility> Ability;

	/** Level to grant the ability at */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	int32 Level;

	/** The input id of the ability */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	EInputAbilities InputId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Ability)
	TSubclassOf<UObject> ObjectInformation;
	
	/** A reference to where we retrieved the ability from. This is usually the id of one of the player's inventory items, or null */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	FGuid Id;
};




USTRUCT(BlueprintType)
struct FGameplayAttributeMapping
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FGameplayAttributeMapping(
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
struct FGameplayEffectMapping
{
	GENERATED_USTRUCT_BODY()
	/** Default constructor */
	FGameplayEffectMapping() = default;
	// FGameplayEffectMapping(
	// 	const TSoftClassPtr<UGameplayEffect>& EffectType = nullptr,
	// 	const int32 Level = 1
	// ): EffectType(EffectType), Level(Level) {}

	/** Gameplay Effect to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Effect")
	TSoftClassPtr<UGameplayEffect> Effect;

	/** Level for the Gameplay Effect to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Effect")
	int32 Level = 1;
};



