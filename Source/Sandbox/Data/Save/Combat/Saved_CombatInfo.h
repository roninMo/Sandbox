// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "Saved_CombatInfo.generated.h"

enum class EEquipSlot : uint8;
class UCombatComponent;
enum class EArmamentStance : uint8;


/**
 * 
 */
UCLASS()
class SANDBOX_API USaved_CombatInfo : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEquipSlot PrimaryEquipSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEquipSlot SecondaryEquipSlot;

	// TODO: perhaps only use an Id for handling weapons
	/**** Armaments ****/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_One;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_Two;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_Three;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_One;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_Two;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_Three;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EArmamentStance CurrentStance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ComboIndex;
	
	/**** Armor ****/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Gauntlets;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Leggings;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Helm;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Chest;


public:
	/** Retrieves the combat information for saving */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading")
	virtual void SaveFromCombatComponent(UCombatComponent* CombatComponent);

	
};
