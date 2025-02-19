// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "SaveComponent_Character.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API USaveComponent_Character : public USaveComponent
{
	GENERATED_BODY()
	
public:
	/**
	 * Retrieves the save's reference for specific save states using the save state and actor's information. This should vary between players and enemies spawned in the world \n\n 
	 * Default logic just returns FName. Subclassed versions should return the actor's unique network or npc id followed by a reference to the type of information they're saving \n
	 *		Something like this:
	 *			- Enemies: SpawnedEnemy_0_Inventory
	 *			- Players: Player_0_Inventory
	 *
	 * @param Saving					The type of information we're saving
	 * @returns							The id used for saving information to a specific slot
	 */
	virtual FString GetSaveSlotIdReference(const ESaveType Saving) const override;
	

protected:
	/** Retrieves the player's save slot id specific to their steam/console platform account for saving purposes */
	virtual FString GetPlayerNetId() const override;

	
};
