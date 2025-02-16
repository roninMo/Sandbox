// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CustomLevelScriptActor.generated.h"

class USaveLogic;
class USaveComponent;
/**
 * A custom level blueprint for handling logic specific to the game. \n\n
 * Currently it's just used for adding additional saving logic for storing level specific save information during gameplay. Which could be player and enemy locations and enemy hp and combat state from a save
 */
UCLASS()
class SANDBOX_API ACustomLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	/*

		Npc Information
			- Location and Rotation
			- Combat State

		Player information
			- Location and Rotations

		Other / respawn logic for npcs / players for different game modes
			- Custom logic that game modes use to save information properly
		


	*/


public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving") bool SaveLevelInformation(USaveComponent* SaveComponent, USaveLogic* SaveLogic);
	virtual bool SaveLevelInformation_Implementation(USaveComponent* SaveComponent, USaveLogic* SaveLogic);


};
