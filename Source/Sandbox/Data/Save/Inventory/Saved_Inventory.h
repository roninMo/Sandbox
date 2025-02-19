// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "Saved_Inventory.generated.h"

/**
 * Save information for traditional inventories. There should be logic to retrieve the inventory within the default component already
 */
UCLASS()
class SANDBOX_API USaved_Inventory : public USaveGame
{
	GENERATED_BODY()

public:
	/** The character's saved inventory information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_InventorySaveInformation SaveInformation;

	
};
