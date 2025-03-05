// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Structs/LevelSaveInformation.h"
#include "LevelSaveComponent.generated.h"

enum class EGameModeType : uint8;
class AMultiplayerGameMode;
class USaveGame;
class USaved_Level;


/**
 * The GameMode's save component for saving actors and their information within levels. @ref USaveComponent, @ref USaveLogic, @ref ULevelSaveComponent
 * Handles both saving level information, and referencing actor's SaveComponents for information, and is intended for adding custom classes for Singleplayer/Multiplayer/Api custom functionality
 *
 * TODO: Add asynchronous save logic. Errors that happen while abruptly stopping save functionality rarely corrupt save logic, however we don't want problems with performance
 */
UCLASS( Blueprintable, ClassGroup=(Saving), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API ULevelSaveComponent : public UObject
{
	GENERATED_BODY()

protected:
	/** Hash table of the saved actors that we need to update in the save slot */ // We're just going to store the updated information here, and remove it once saved for batching
	UPROPERTY(BlueprintReadWrite, Category = "Saving and Loading") TMap<FString, F_LevelSaveInformation_Actor> PendingActorsToSave;

	/** A stored reference to the save game slot for the current level */
	UPROPERTY(BlueprintReadWrite, Category= "Saving and Loading") TObjectPtr<USaved_Level> SaveState;

	
public:
	/**
	 * Updates the save information with the current list of pending actors that need to be saved
	 *
	 * @note TODO: Add custom functionality for saving individual information outside of level logic if necessary
	 * @param bSaveActorData		Whether to save the actors combat / inventory information
	 * @returns						Whether the current state information was successfully saved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") bool SaveLevel(const FString& SaveUrl, bool bSaveActorData = false);
	virtual bool SaveLevel_Implementation(const FString& SaveUrl, bool bSaveActorData = false);

	/**
	 * Loads the level's save state information and updates the actors within the level. Optionally handles additionally loading the actor's save information
	 *
	 * @param bRetrieveActorData	Whether to load the actor's saved information
	 * @returns						Whether the save information was successfully retrieved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") bool LoadLevelSave(const FString& SaveUrl, bool bRetrieveActorData = false);
	virtual bool LoadLevelSave_Implementation(const FString& SaveUrl, bool bRetrieveActorData = false);

	/** Autosave logic for saving components efficiently to prevent performance issues */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") bool AutoSaveHandling();
	virtual bool AutoSaveHandling_Implementation();

	/**
	 * Adds save information to the PendingActorsToSave array
	 *
	 * @param SaveInformation		The actor's save information
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") void AddPendingActor(const F_LevelSaveInformation_Actor& SaveInformation);
	virtual void AddPendingActor_Implementation(const F_LevelSaveInformation_Actor& SaveInformation);

	/** Returns whether we're able to currently save the level's state information */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") bool IsValidToCurrentlySave() const;
	virtual bool IsValidToCurrentlySave_Implementation() const;

	
	
	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/** Retrieves the current GameMode. By default, the owner of this component should be the game mode. Subclass this for custom functionality */
	UFUNCTION(BlueprintCallable) virtual AMultiplayerGameMode* GetGameMode();

	/** Retrieves the Level from the GameMode */
	UFUNCTION(BlueprintCallable) virtual ULevel* GetLevel();

	/** Resets Level Component's state. Helpful during level transitions */
	UFUNCTION(BlueprintCallable) virtual void ResetLevelSaveComponentState();

	/** Retrieves the level's SaveGameSlot base on the Level and GameMode */
	UFUNCTION(BlueprintCallable) virtual USaved_Level* GetSaveGameRef(const FString SaveUrl);
	
	
};
