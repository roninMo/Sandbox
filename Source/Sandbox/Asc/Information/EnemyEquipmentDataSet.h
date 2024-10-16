// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sandbox/Data/Structs/ArmorInformation.h"
#include "EnemyEquipmentDataSet.generated.h"

enum class EEquipSlot : uint8;
class UInventoryComponent;
class UAbilitySystem;
class UCombatComponent;
class UGameplayEffect;


/** DataAsset that can be used to define an enemy equipment */
UCLASS(BlueprintType)
class SANDBOX_API UEnemyEquipmentDataSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UEnemyEquipmentDataSet() = default;

	/** The character's equipped gauntlets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Gauntlets;
	
	/** The character's equipped leggings */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Leggings;

	/** The character's equipped helm */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Helm;

	/** The character's equipped chest */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Chest;

	
	/** The character's first left hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") FName Armament_LeftHandSlotOne;
	
	/** The character's second left hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") FName Armament_LeftHandSlotTwo;
	
	/** The character's third left hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") FName Armament_LeftHandSlotThree;
	
	/** The character's first right hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") FName Armament_RightHandSlotOne;
	
	/** The character's second right hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") FName Armament_RightHandSlotTwo;
	
	/** The character's third right hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") FName Armament_RightHandSlotThree;

	
	/** The currently equipped left hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") EEquipSlot EquippedLeftHandArmament;
	
	/** The currently equipped right hand armament */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armaments") EEquipSlot EquippedRightHandArmament;

	
	// /** Additional armor values */
	// UPROPERTY(EditDefaultsOnly, Category="Attributes", meta=(TitleProperty=Effect))
	// TSubclassOf<UGameplayEffect> ArmorValues;
	//
	// /** Additional player stat values */
	// UPROPERTY(EditDefaultsOnly, Category="Attributes", meta=(TitleProperty=Effect))
	// TSubclassOf<UGameplayEffect> AttributeValues;

	
	/**
	 * Grants itself (Ability Set) to the passed in ASC, adding defined Abilities, Attributes and Effects.
	 *
	 * @param AbilitySystemComponent AbilitySystemComponent pointer to operate on
	 * @param InventoryComponent InventoryComponent pointer to operate on
	 * @param CombatComponent CombatComponent pointer to operate on
	 *
	 * @return True if the ability set was granted successfully, false otherwise
	 */
	bool AddToCharacter(UAbilitySystem* AbilitySystemComponent, UInventoryComponent* InventoryComponent, UCombatComponent* CombatComponent) const;

	/**
	 * Removes the AbilitySet represented by InAbilitySetHandle from the passed in ASC. Clears out any previously granted Abilities,
	 * Attributes and Effects from the set.
	 * 
	 * @param AbilitySystemComponent AbilitySystemComponent pointer to operate on
	 * @param InventoryComponent InventoryComponent pointer to operate on
	 * @param CombatComponent CombatComponent pointer to operate on
	 * 
	 * @return True if the ability set was removed successfully, false otherwise
	 */
	static bool RemoveFromCharacter(UAbilitySystem* AbilitySystemComponent, UInventoryComponent* InventoryComponent, UCombatComponent* CombatComponent);

};

