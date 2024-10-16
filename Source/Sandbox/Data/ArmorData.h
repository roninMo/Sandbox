// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArmorData.generated.h"

class UAbilitySystem;
class ACharacterBase;


/**
 * 
 */
UCLASS()
class SANDBOX_API UArmorData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UArmorData() = default;
	
	/** The character's equipped gauntlets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Gauntlets;
	
	/** The character's equipped leggings */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Leggings;

	/** The character's equipped helm */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Helm;

	/** The character's equipped chest */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Armor") FName Chest;

	// TODO: Armor stats adjustments

	
	/** The combat component by default creates and stores the handles for weapons and equipment, so we're just going to prioritize that while building utility logic */
	
	/**
	 * Adds the armor, it's stats, abilities, and passives to the character. If there's an inventory component, it adds the armor to the player's inventory as well, otherwise it creates dummy data for things like AI
	 *
	 * @param Character The character we're equipping
	 * @param ErrorText CombatComponent pointer to operate on
	 *
	 * @return True if the armor was granted successfully, false otherwise
	 */
	bool AddToCharacter(ACharacterBase* Character, FText* ErrorText = nullptr) const;

	/**
	 * Removes the armor, it's stats, abilities, and passives to the character. If there's an inventory component, it removes the armor from the player's inventory as well
	 * 
	 * @param Character The character we're equipping
	 * @param bRemoveFromInventory Whether to remove the armor from the player's inventory
	 * @param ErrorText Reason of error in case of failed operation
	 * 
	 * @return True if the armor was removed successfully, false otherwise
	 */
	static bool RemoveFromCharacter(const ACharacterBase* Character, bool bRemoveFromInventory = false, FText* ErrorText = nullptr);
	
	
};
