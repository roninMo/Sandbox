// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sandbox/Data/AbilityData.h"
#include "Sandbox/Data/AttributeData.h"
#include "GameplayAbilitiyUtilities.generated.h"

class UArmorData;
class UEquipmentData;
class UInputAction;
class UGameplayEffect;
class UGameplayAbility;
class UAbilitySystemComponent;
class UAttributeSet;
class UAbilitySystem;
class UCharacterAbilityDataSet;
enum class ETriggerEvent : uint8;
struct FCharacterAbilityDataSetHandle;
struct FGameplayTagContainer;
struct FGameplayTag;
struct FGameplayAbilitySpec;
struct FActiveGameplayEffectHandle;
struct FCharacterAbilityDataSet;
struct FGameplayAttributeInfo;
struct FGameplayAbilitySpecHandle;
struct FGameplayAttribute;
struct FGameplayAbilityInfo;
struct FComponentRequestHandle;
struct FGameplayCueParameters;
struct FGameplayEffectContextHandle;


/**
 * TODO: Refactor adding abilities to use the ability system for storing abilities correctly
 */
UCLASS()
class SANDBOX_API UGameplayAbilityUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

//------------------------------------------------------------------------------//
//	Ability / Attribute / Effect												//
//------------------------------------------------------------------------------//
public:
	/** Tries to find an ability system component on the actor and cast to UAbilitySystem., will use AbilitySystemInterface or fall back to a component search */
	UFUNCTION(BlueprintPure, Category = "Ability")
	static UAbilitySystem* GetAbilitySystem(const AActor* Actor);

	/** Returns all defined Gameplay Attributes for the provided AttributeSet class */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attribute Set")
	static void GetAllAttributes(TSubclassOf<UAttributeSet> AttributeSetClass, TArray<FGameplayAttribute>& OutAttributes);


	// There's a mix of blueprint library logic and primary data logic that's mixed together, this needs to be fixed!
	/**
	 * Adds attribute information to the character.
	 * AttributeData sets are game/player specific data that's used to initialize the attributes
	 *
	 * @param InAbilitySystemComponent		The ability system component of the character we're adding the attributes to
	 * @param AttributeData					The character's attribute information stored in gameplay effects
	 * @param OutAttributeDataHandle		The handle of the attribute data
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Attribtes, Equipment, Ability, and Armor Data")
	static bool TryAddAttributeData(UAbilitySystemComponent* InAbilitySystemComponent, const UAttributeData* AttributeData, FAttributeDataHandle& OutAttributeDataHandle, bool bPrintErrorMessages = false);
	
	/**
	 * Adds equipment to the character's combat component, and the equipment to the inventory if they have one.
	 * AttributeData sets are game/player specific data that's used to initialize the attributes
	 *
	 * @param InAbilitySystemComponent		The ability system component of the character we're adding the attributes to
	 * @param EquipmentData					The character's equipment information
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Attribtes, Equipment, Ability, and Armor Data")
	static bool TryAddEquipmentData(UAbilitySystemComponent* InAbilitySystemComponent, const UEquipmentData* EquipmentData, bool bPrintErrorMessages = false);


	/**
	 * Adds abilities and passive effects to the character.
	 * AttributeData sets are game/player specific data that's used to initialize the attributes
	 *
	 * @param InAbilitySystemComponent		The ability system component of the character we're adding the attributes to
	 * @param AbilityData					The character's equipment information
	 * @param OutAbilityDataHandle			The handle of the ability data
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Attribtes, Equipment, Ability, and Armor Data")
	static bool TryAddAbilityData(UAbilitySystemComponent* InAbilitySystemComponent, const UAbilityData* AbilityData, FAbilityDataHandle& OutAbilityDataHandle, bool bPrintErrorMessages = false);


	/**
	 * Adds equipment to the character's combat component, and the equipment to the inventory if they have one.
	 * AttributeData sets are game/player specific data that's used to initialize the attributes
	 *
	 * @param InAbilitySystemComponent		The ability system component of the character we're adding the attributes to
	 * @param ArmorData						The character's armor information
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Attribtes, Equipment, Ability, and Armor Data")
	static bool TryAddArmorData(UAbilitySystemComponent* InAbilitySystemComponent, const UArmorData* ArmorData, bool bPrintErrorMessages = false);





	/**
	 * Adds abilities, attributes, and gameplay effects to a player.
	 * Ability Sets are game/player specific data that's used to initialize the character's attributes, abilities, passives, and any gameplay information specific to the game or the character
	 *
	 * @param AbilitySystemComponent		The ability system component of the player we're adding the ability set to
	 * @param InAbilitySet					The player ability data set information
	 * @param OutAbilitySetHandle			The handle of the ability set
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Adding and Clearing")
	static bool TryAddAbilitySet(UAbilitySystemComponent* AbilitySystemComponent, const UCharacterAbilityDataSet* InAbilitySet, FCharacterAbilityDataSetHandle& OutAbilitySetHandle);

	
	/** Tries to add an ability using the UAbilitySystem's AddAbility function */
	static void TryAddAbility(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAbilityInfo& InAbilityMapping, FGameplayAbilitySpecHandle& OutAbilityHandle, FGameplayAbilitySpec& OutAbilitySpec);
	
	/** Tries to add an AttributeSet and it's information with data tables. Be careful because this loosely creates an attribute set, and it's tough to reference specific attribute sets for effect calculations from this */
	static void TryAddAttributes(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttributeInfo& InAttributeSetMapping, UAttributeSet*& OutAttributeSet);
	
	/** Tries to add attribute information with gameplay effects. Returns the handles for the applied effects */
	static void TryAddAttributes(UAbilitySystemComponent* AbilitySystemComponent, const TSubclassOf<UGameplayEffect> InEffectType, const float InLevel, TArray<FActiveGameplayEffectHandle>& OutEffectHandles);

	/** Tries to add gameplay effects. Returns the handles for the applied effects */
	static void TryAddGameplayEffect(UAbilitySystemComponent* AbilitySystemComponent, const TSubclassOf<UGameplayEffect> InEffectType, const float InLevel, TArray<FActiveGameplayEffectHandle>& OutEffectHandles);

	
	/** Helper to return the AttributeSet UObject as a non const pointer, if the passed in ASC has it granted */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static UAttributeSet* GetAttributeSet(const UAbilitySystemComponent* InASC, const TSubclassOf<UAttributeSet> InAttributeSet);

	/** Determine if a gameplay effect is already applied, same class and same level */
	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Effect")
	static bool HasGameplayEffectApplied(const UAbilitySystemComponent* InASC, const TSubclassOf<UGameplayEffect>& InEffectType, TArray<FActiveGameplayEffectHandle>& OutEffectHandles);

	/** Determine if an ability is already granted, same class and same level */
	UFUNCTION(BlueprintCallable, Category = "Ability|Adding and Clearing")
	static bool IsAbilityGranted(const UAbilitySystemComponent* InASC, TSubclassOf<UGameplayAbility> InAbility, const int32 InLevel = 1);

	
//------------------------------------------------------------------------------//
//	GameplayCue -  Add GameplayCues without having to create GameplayEffects	//
//------------------------------------------------------------------------------//
public:
	/** Invoke a gameplay cue on the actor's ability system component */
	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Cue", meta=(GameplayTagFilter="GameplayCue"))
	static void ExecuteGameplayCueForActor(AActor* Actor, FGameplayTag GameplayCueTag, FGameplayEffectContextHandle Context);

	/** Invoke a gameplay cue on the actor's ability system component, with extra parameters */
	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Cue", meta=(GameplayTagFilter="GameplayCue"))
	static void ExecuteGameplayCueWithParams(AActor* Actor, FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	/** Adds a persistent gameplay cue to the actor's ability system component. Optionally will remove if ability ends */
	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Cue", meta=(GameplayTagFilter="GameplayCue"))
	static void AddGameplayCue(AActor* Actor, FGameplayTag GameplayCueTag, FGameplayEffectContextHandle Context);

	/** Adds a persistent gameplay cue to the actor's ability system component. Optionally will remove if ability ends */
	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Cue", meta=(GameplayTagFilter="GameplayCue"))
	static void AddGameplayCueWithParams(AActor* Actor, FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameter);

	/** Removes a persistent gameplay cue from the actor's ability system component */
	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Cue", meta=(GameplayTagFilter="GameplayCue"))
	static void RemoveGameplayCue(AActor* Actor, FGameplayTag GameplayCueTag);

	/** Removes any GameplayCue added on its own, i.e. not as part of a GameplayEffect. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Cue", meta=(GameplayTagFilter="GameplayCue"))
	static void RemoveAllGameplayCues(AActor* Actor);

	
	/** Adds a tag container to ASC, but only if ASC doesn't have said tags yet */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tags")
	static void AddLooseGameplayTagsUnique(UAbilitySystemComponent* InASC, const FGameplayTagContainer& InTags, const bool bReplicated = true);
	
	/** Removes a tag container to ASC, but only if ASC doesn't have said tags yet */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tags")
	static void RemoveLooseGameplayTagsUnique(UAbilitySystemComponent* InASC, const FGameplayTagContainer& InTags, const bool bReplicated = true);

	
//------------------------------------------------------------------------------//
//	Utility																		//
//------------------------------------------------------------------------------//
private:
	/** Handler for AbilitySystem OnGiveAbility delegate. Sets up input binding for clients (not authority) when GameFeatures are activated during Play. */
	static void HandleOnGiveAbility(
		FGameplayAbilitySpec& InAbilitySpec,
		TWeakObjectPtr<UInputComponent> InInputComponent,
		TWeakObjectPtr<UInputAction> InInputAction,
		const ETriggerEvent InTriggerEvent,
		FGameplayAbilitySpec InNewAbilitySpec
	);

	
};
