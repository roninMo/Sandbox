// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GameModeLog, Log, All);

enum class EGameModeType : uint8;
class USave;
class ULevelSaveComponent;
class USaveLogic;


/**
 * https://docs.unrealengine.com/latest/INT/Engine/Blueprints/UserGuide/Types/LevelBlueprint/index.html
 * - Epic Online Subsystem example logic for handling lobby events (probably from a player controller's remote procedure calls -> @ref EOSGO_API UGoMenu
 */
UCLASS()
class SANDBOX_API AMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	/** Class of the level's SaveLogic, which handles save information and state for actors during play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Classes|Level")
	TSubclassOf<ULevelSaveComponent> LevelSaveComponentClass;

	/** The Game Mode's classification */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameMode)
	EGameModeType GameModeType; // TODO: If the game mode isn't recreated on server travel, add infrastructure here for lobby -> to single / multi / custom games here instead of the GameInstance

	/**
	 * The base save information for any gamemode or custom level with save information.
	 * This is used to create the save game reference for saving / retrieving in singleplayer / multiplayer games with servers that use SaveGame state
	 *
	 * @note This design pattern is like an abstract factory, but it's just like a hierarchy of save components that use this to properly save and retrieve information, and should be easy to use with asynchronous APIs 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameMode|Save State") TObjectPtr<USave> CurrentSave;

	/** The save game reference, used for saving and retrieving save information for a specific save */
	// UPROPERTY(BlueprintReadWrite, Transient) FString SaveGameUrl;

	/** The most recent save index for a specific save slot. ie. if it's the 100th save, and we load / save to a previous save, this shouldn't be affected */
	UPROPERTY(BlueprintReadWrite, Transient) int32 SaveIndex;

	/** The host's Platform Id. For singleplayer this is their Console/Account id, and for multiplayer it's their online subsystem account id that's retrieved when the server owner's game begins  */
	UPROPERTY(BlueprintReadWrite, Transient) FString SavePlatformId; // TODO: This is just the singleplayer's reference for retrieving the platform id, find a better way at beginplay to retrieve the save state

	/** A stored reference to the save logic component. Only spawned on the server; handle's the loading and spawning of save state for a Singleplayer/Multiplayer custom game state. */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<ULevelSaveComponent> LevelSaveComponent;
	// TODO: client replication will be required for creating level specific stuff. And we need classifications for objects saved / created. ->  Players / Actors that have save information in level / Actor (Spawned) / Props / etc.  


	// Respawn logic

	// Adventure / TDM / FoF -> subclassed infrastructure
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameMode|Save State|Levels") FString LobbyLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameMode|Save State|Levels") FString SingleplayerLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameMode|Save State|Levels") FString CustomLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameMode|Save State|Levels") TArray<FString> MultiplayerLevels;
	

	
public:
	AMultiplayerGameMode(const FObjectInitializer& ObjectInitializer);
	
	/** Called right before components are initialized, only called during gameplay */
	virtual void PreInitializeComponents() override;

	/** Reset actor to initial state - used when restarting level without reloading. */
	virtual void Reset() override;

	

	
protected:
	// Respawning
	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;



	
//----------------------------------------------------------------------------------//
// Save State Functions																//
//----------------------------------------------------------------------------------//
public:
	/**
	 * Saves the current game to a save slot.
	 * If there was a specified Save Index, it will overwrite the previous save with the current level information; otherwise, it saves to the current iteration and increments to begin the next save state
	 * 
	 * @param SaveUrl The specific slot we're saving to
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool SaveGame(const FString& SaveUrl, const int32 Index);

	/**
	 * Loads the current save state from a save slot.
	 * 
	 * @param SaveUrl The specific slot we're saving to
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool LoadSave(const FString& SaveUrl, const int32 Index);

	/** Initializes the SaveUrlerence and Save Index from the current SaveGameConfig, and updates the CurrentSave */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool SetCurrentSave(USave* Save);
	
	/**
	 * Creates another save state, and updates the save reference and index on both the current save, here, and the player states. \n\n
	 * This refers to the current index, and not a specified index during a save
	 * @note Objects in the world should retrieve the SaveUrl and Index from the game mode on the server
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Init") virtual bool CreateNextSave();
	
	/** Finds the player's current save index for a specific save slot */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Init") virtual bool FindCurrentSave(const FString& AccountId, int32 SaveSlot, UPARAM(ref) FString& OutSaveUrl, int32& OutSaveIndex) const;

	/** Retrieves a list of the previous saves using the current save */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Init") virtual TArray<FString> FindPreviousSaves(const FString& AccountId, int32 SaveSlot, int32 CurrentSaveIndex, int32 SavesToRetrieve = 10) const;
	
	/** Removes one of the save slots, including all of the saves connected to the slot */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool DeleteSaveSlot(const FString& AccountId, int32 SaveSlot);
	
	/** Creates a new save using the Owner's platform id and a specified save slot. Used when creating a new save */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual USave* NewSaveSlot(const FString& AccountId, int32 SaveSlot);

	
protected:
	/** Retrieves a reference to the current save game base information */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual USave* GetSaveGameBase();
	
	/** Sets the save game base information */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual void SetSaveGameBase(USave* Save);


public:
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual FString GetSavePlatformId() const;
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual void RetrieveSavePlatformId();


public:
	/** Retrieve the current SaveGameUrl reference for saving information to the level */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual FString GetCurrentSaveGameUrl(bool bLevel = false) const;
	
	/** Construct the SaveGameUrl from the save information */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual FString ConstructSaveGameUrl(const FString& PlatformId, int32 Slot, int32 Index, bool bLevel = false) const;


	// GetBaseSaveUrl


	// Load Character
	// Save Character
	// Load Level
	// Save Level


	
	
/**

		Character Saving (should be able to switch between singleplayer saves and an account save that is accessed in other games in multiplayer with ease) // SaveComponent
		Level Saving (State of world) // LevelSaveComponent
		Custom Level Saving (objects added to a specific level) - multiplayer specific // -> Create a new component to handle spawning on client / server for different objects

	- LevelSaveUrl
		- SP -> Adventure_Character1_S1_54_Level_
		- MP -> MP_CustomLevel1_
	- CharacterSaveUrl
		- SP -> Adventure_Character1_S1_54_Character1_
		- MP -> retrieved from an api during join (there isn't a way of handling this right now with the editor using the account / platform id) 



	- Adventure_Character1_S1_54
				Character:
					->  Adventure_Character1_S1_54_Combat
					->  Adventure_Character1_S1_54_Inventory
					->  Adventure_Character1_S1_54_CameraSettings
					->  Adventure_Character1_S1_54_Attributes
					->  Adventure_Character1_S1_54_Level

	- MP_CustomLevel1
			- MP_CustomLevel2
				Props, Actors, Enemies, Weapons, GameMode Objects, Vehicles, etc spawned in the world should be handled separately here before we handle character logic, on client / server, retrieved from the specified level, spawn presets, etc.
					-> MP_CustomLevel1_Props_Ramp (The level's custom components probably won't be spawned on the client automatically)
					-> MP_CustomLevel1_Props_Object1
					-> MP_CustomLevel1_Actors_ShieldPickup_1
						-> MP_CustomLevel1_Actors_ShieldPickup_1_Level (Transform)
					-> MP_CustomLevel1_Actors_WeaponCrate_0
						-> MP_CustomLevel1_Actors_WeaponCrate_0_Level
						-> MP_CustomLevel1_Actors_WeaponCrate_0_Inventory


	
 



*/

	
	
//----------------------------------------------------------------------------------//
// Match State Functions															//
//----------------------------------------------------------------------------------//
protected:
	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	/** Called when the state transitions to WaitingToStart */
	virtual void HandleMatchIsWaitingToStart() override;
	
	/** Called when the state transitions to InProgress */
	virtual void HandleMatchHasStarted() override;

	/** Called when the map transitions to WaitingPostMatch */
	virtual void HandleMatchHasEnded() override;

	/** Called when the match transitions to LeavingMap */
	virtual void HandleLeavingMap() override;

	/** Called when the match transitions to Aborted */
	virtual void HandleMatchAborted() override;

	/** Handles server travel */
	UFUNCTION(BlueprintCallable, Category = "Server Travel") virtual void Travel(FString Map);


	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/** Retrieves the GameMode's classification */
	UFUNCTION(BlueprintCallable, Category = "Game") virtual EGameModeType GetGameModeType();

	/** Retrieves a list of the player controllers */
	UFUNCTION(BlueprintCallable, Category = "Game") virtual TArray<APlayerController*> GetPlayers() const;

	/** Prints a message with a reference to the player controller */
	UFUNCTION() virtual void PrintMessage(const FString& Message);

	/** Prints a list of the actors within the level */
	UFUNCTION(BlueprintCallable) virtual void PrintActorsInLevel(bool bSavedActors = false);

	
};
