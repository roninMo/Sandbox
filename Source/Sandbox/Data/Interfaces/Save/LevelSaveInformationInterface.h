// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Structs/LevelSaveInformation.h"
#include "UObject/Interface.h"
#include "LevelSaveInformationInterface.generated.h"

class AMultiplayerGameState;
class AMultiplayerGameMode;


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULevelSaveInformationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors spawned in the world that are saved during play \n
 *
 * Level Save Component (GameMode / GameState)  Functions
 *		- Save Level
 *			- Loop through actors in level and save the state of (characters, enemies, collectables, event specific logic and quest state information
 *		- Load Level
 *			- Loop through saved actors after BeginPlay and find the ones that have save state, and spawn any of the saved items / objects that aren't specific to the level
 *
 *	Save Information
 *		- LevelSave_Actor
 *		- LevelSave_Collectable
 *		- LevelSave_Events
 *		- LevelSave_Quests
 *
 *	Actor Interface Functions
 *		- LoadFromLevel
 *		- SaveToLevel
 */
class SANDBOX_API ILevelSaveInformationInterface
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves the actor's save component
	 * 
	 */

	// SaveData(levelOnly?, config)
	// LoadSaveData

	/**
	 * Returns the current save state and config of the actor level during play
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level|Saving", DisplayName = "Save To Level") F_LevelSaveInformation_Actor SaveToLevel();
	virtual F_LevelSaveInformation_Actor SaveToLevel_Implementation();

	/**
	 * Blueprint event for when the actor's save information has been sent to the level for saving
	 *
	 * @params SavedInformation			The information that's sent to the level blueprint for saving
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Level|Saving", DisplayName = "On Save To Level")
	void OnSaveToLevel(const F_LevelSaveInformation_Actor& SavedInformation);

	
	/**
	 * Saves the actors information using the SaveComponent interface
	 * returns							Whether it successfully saved the actor information
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level|Saving", DisplayName = "Save To Level") bool SaveActorData(const F_LevelSaveInformation_Actor& SaveConfig);
	virtual bool SaveActorData_Implementation(const F_LevelSaveInformation_Actor& SaveConfig);

	/**
	 * Blueprint event for when the actor's saved it's information
	 *
	 * @params SaveConfig				The actor's save configuration
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Level|Saving", DisplayName = "On Save Actor Data")
	void OnSaveActorData(const F_LevelSaveInformation_Actor& SaveConfig);
	

	/**
	 * Retrieves the level's currently saved state for this actor, and loads the saved information
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level|Saving", DisplayName = "Load From Level") bool LoadFromLevel(const F_LevelSaveInformation_Actor& PreviousSave);
	virtual bool LoadFromLevel_Implementation(const F_LevelSaveInformation_Actor& PreviousSave);

	/**
	 * Blueprint event for when the actor's save information has been sent to the level for saving
	 *
	 * @params PreviousSave				The information that's sent to the level blueprint for saving
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Level|Saving", DisplayName = "On Load From Level")
	void OnLoadFromLevel(const F_LevelSaveInformation_Actor& PreviousSave, bool bSuccessfullyLoaded);

	
	/**
	 * Retrieves the id of the actor. This is used for retrieving the proper save information for actors placed and spawned in the world \n
	 * 
	 * This is going to generic for handling multiple use cases for different scenarios, and might be adjusted later because this is kind of hacky
	 *	- Players:									The character's subsystem account / platform id
	 *	- Items Placed in Level:					The name of the object based on the level's construction
	 *	- Items Spawned in Level during Play:		The inventory's id. Uses the class reference to construct and spawn the item once the game begins
	 *
	 * TODO: unreal's GetName naming convention varies between server and client and is based on what's placed in the world. Anything spawned will be respawned,
	 *		and we still need a convention for mapping the saved name to the actor's stored FGuid id for reference
	 *
	 * @returns							The id of the actor   
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Level|Saving|Utility", DisplayName = "Get Actor Level Id") FString GetActorLevelId() const;
	virtual FString GetActorLevelId_Implementation() const;

	
};
