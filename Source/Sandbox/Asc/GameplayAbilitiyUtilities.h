// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilitiyUtilities.generated.h"

class UInputAction;
class UGameplayEffect;
class UGameplayAbility;
class UAbilitySystemComponent;
class UAttributeSet;
class UAbilitySystem;
class UCharacterAbilityDataSet;
enum class ETriggerEvent : uint8;
struct FGameplayTagContainer;
struct FGameplayTag;
struct FGameplayAbilitySpec;
struct FActiveGameplayEffectHandle;
struct FCharacterAbilityDataSet;
struct FGameplayAttributeMapping;
struct FGameplayAbilitySpecHandle;
struct FGameplayAttribute;
struct FGameplayAbilityMapping;
struct FComponentRequestHandle;
struct FGameplayCueParameters;
struct FGameplayEffectContextHandle;


/**
 * 
 */
UCLASS()
class SANDBOX_API UGameplayAbilitiyUtilities : public UBlueprintFunctionLibrary
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
	
	static void TryAddAbility(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAbilityMapping& InAbilityMapping, FGameplayAbilitySpecHandle& OutAbilityHandle, FGameplayAbilitySpec& OutAbilitySpec);
	static void TryAddAttributes(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttributeMapping& InAttributeSetMapping, UAttributeSet*& OutAttributeSet);
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
