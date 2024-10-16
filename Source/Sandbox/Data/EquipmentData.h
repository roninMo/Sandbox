// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipmentData.generated.h"

enum class EEquipSlot : uint8;
class ACharacterBase;


/**
 * 
 */
UCLASS()
class SANDBOX_API UEquipmentData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UEquipmentData() = default;
	
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
	
	// TODO: Equipment stats adjustments

	
	/**
	 * Adds the equipment, it's abilities, and passives to the character. If there's an inventory component, it adds the armaments to the player's inventory as well, otherwise it creates dummy data for things like AI
	 *
	 * @param Character The character we're equipping
	 * @param ErrorText CombatComponent pointer to operate on
	 *
	 * @return True if the ability set was granted successfully, false otherwise
	 */
	bool AddToCharacter(ACharacterBase* Character, FText* ErrorText = nullptr) const;

	/**
	 * Removes the equipment, it's abilities, and passives to the character. If there's an inventory component, it removes the armaments from the player's inventory as well
	 * 
	 * @param Character The character we're equipping
	 * @param bRemoveFromInventory Whether to remove the armor from the player's inventory
	 * @param ErrorText Reason of error in case of failed operation
	 * 
	 * @return True if the ability set was removed successfully, false otherwise
	 */
	static bool RemoveFromCharacter(const ACharacterBase* Character, bool bRemoveFromInventory = false, FText* ErrorText = nullptr);

	
};
