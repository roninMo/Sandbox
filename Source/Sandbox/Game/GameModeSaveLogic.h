// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Sandbox/Data/Structs/LevelInformation.h"
#include "Sandbox/Data/Structs/LevelSaveInformation.h"
#include "GameModeSaveLogic.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GameModeLog, Log, All);

enum class EGameModeType : uint8;
class USave;
class USaved_Level;


/**
 * Server logic for handling saving the level and the players on the server, and distributing save information for client's to save individual character information. \n\n
 * The idea is for this to work for singleplayer out of the box and allow you to easily incorporate online multiplayer subsystems for retrieving character information per game.
 *
 * - Epic Online Subsystem example logic for handling lobby events (probably from a player controller's remote procedure calls -> @ref EOSGO_API UGoMenu
 * 
 * TODO: This needs to be refactored in favor of another way of saving multiple indices of a slot.
 *			- I wanted fallbacks for saving while dividing the save, level, and player information, however with the choice of saving to specific instances it just makes for messy code (forward indexing with a previous save reference saving logic) 1, 2, 3, 4, 5, 3, 4, 5, 6 -> 1-9
 * TODO: Add asynchronous save logic. Errors that happen while abruptly stopping save functionality rarely corrupt save logic, however we don't want problems with performance
 *
 */
UCLASS()
class SANDBOX_API AGameModeSaveLogic : public AGameMode
{
	GENERATED_BODY()

protected:
	/** The Game Mode's classification */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameMode)
	EGameModeType GameModeType;

	/**
	 * The base save information for any gamemode or custom level with save information.
	 * This is used to create the save game reference for saving / retrieving in singleplayer / multiplayer games with servers that use SaveGame state
	 *
	 * @note This design pattern is like an abstract factory, but it's just like a hierarchy of save components that use this to properly save and retrieve information, and should be easy to use with asynchronous APIs 
	 */
	UPROPERTY(BlueprintReadWrite, Category="GameMode|Save State") TObjectPtr<USave> CurrentSave;

	/** The host's Platform Id. For singleplayer this is their Console/Account id, and for multiplayer it's their online subsystem account id that's retrieved when the server owner's game begins  */
	UPROPERTY(BlueprintReadWrite, Transient) FString SavePlatformId;

	/** A stored reference to the save game slot for the current level */
	UPROPERTY(BlueprintReadWrite, Category= "GameMode|Saving State") TObjectPtr<USaved_Level> CurrentLevelSave;

	/** Hash table of the saved actors that we need to update in the save slot */ // We're just going to store the updated information here, and remove it once saved for batching
	UPROPERTY(BlueprintReadWrite, Category = "GameMode|Saving State") TMap<FString, F_LevelSaveInformation_Actor> PendingSaves;

	/** The name of the current level */
	UPROPERTY(BlueprintReadWrite, Category = "GameMode|Saving State") FString CurrentLevel;

	/** A list of the levels in this game */
	UPROPERTY(BlueprintReadWrite, Category = "GameMode|Saving State") TMap<FString, F_LevelInformation> Levels;

	/** A stored reference to the data table that contains all the base level information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode|Levels") TObjectPtr<UDataTable> LevelInformationTable;
	
	
public:
	AGameModeSaveLogic(const FObjectInitializer& ObjectInitializer);
	
	/** Store current save state on the game instance for server travel */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Called when the game mode is first created, and once the game travels to another level and resets world information, only player controllers remain */
	virtual void BeginPlay() override;

	/** Stores the current game mode information on the game instance (for server travel) */
	UFUNCTION(BlueprintCallable) virtual void StoreGameModeInformation();

	/** Retrieves the current game mode information on the game instance (after server travel) */
	UFUNCTION(BlueprintCallable) virtual void RetrieveGameModeInformation();

	
//----------------------------------------------------------------------------------//
// Save State																		//
//----------------------------------------------------------------------------------//
public:
	/**
	 * Saves the current game to a save slot.
	 * 
	 * @param SaveUrl The specific slot we're saving to
	 * @param Index The index we're saving to. If left unset, it will save to the current save index
	 * @returns true if it successfully saved the game
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool SaveGame(const FString& BaseSaveUrl, const int32 Index = -1);

	/**
	 * Loads the current save state from a save slot.
	 * 
	 * @param SaveUrl The specific slot we're saving to
	 * @param Index The index we're retrieving save information from
	 * @returns true if it successfully loaded the save
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool LoadSave(const FString& BaseSaveUrl, const int32 Index);

	/**
	 * Finds the player's most recent save index for a specific save slot
	 *
	 * @param BaseUrl The base save url used for a specific character / slot for saving
	 * @param OutSaveUrl The save url of the current save
	 * @param OutSaveIndex The save index of the current save
	 * @returns true if it found a valid save
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Search") virtual bool FindCurrentSave(const FString& BaseUrl, UPARAM(ref) FString& OutSaveUrl, int32& OutSaveIndex) const;

	/**
	 * Retrieves a list of the previous saves using the current save. This works for actual saves, or player and level saves for a specific save slot. \n\n
	 * Helps with retrieving save information and searching for save fallbacks for levels and other stuff (In the event there's an error, or multiple levels in one game)
	 * 
	 * @param SaveUrl The account id of the player that we're finding saves for
	 * @param CurrentSaveIndex The current save's index we're backtracking from
	 * @param SavesToRetrieve How many previous saves we want to retrieve
	 * @returns A list of the previous saves, beginning from the current save to index 0
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Search") virtual TArray<FString> FindPreviousSaves(const FString& SaveUrl, int32 CurrentSaveIndex, int32 SavesToRetrieve = 10) const;
	
	/**
	 * Retrieves a list of the previous saves's indexes using the current save. This works for actual saves, or player and level saves for a specific save slot. \n\n
	 * Helps with retrieving save information and searching for save fallbacks for levels and other stuff (In the event there's an error, or multiple levels in one game)
	 * 
	 * @param SaveUrl The account id of the player that we're finding saves for
	 * @param CurrentSaveIndex The current save's index we're backtracking from
	 * @param SavesToRetrieve How many previous saves we want to retrieve
	 * @returns A list of the previous saves, beginning from the current save to index 0
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Search") virtual TArray<int32> FindPreviousSaveIndexes(const FString& SaveUrl, int32 CurrentSaveIndex, int32 SavesToRetrieve = 10) const;
	
	/**
	 * Creates another save state, and updates the save reference and index on both the current save, here, and the player states. \n\n
	 * This refers to the current index, and not a specified index during a save
	 * 
	 * @note Objects in the world should retrieve the SaveUrl and Index from the game mode on the server
	 * @returns True if it was able to create and update the current save reference
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool ReadyNextSave();

	
public:
	/** Removes one of the save slots, including all of the saves connected to the slot */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool DeleteSaveSlot(const FString& AccountId, int32 SaveSlot);
	
	/** Creates a new save using the Owner's platform id and a specified save slot. Used when creating a new save */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual USave* NewSaveSlot(const FString& AccountId, int32 SaveSlot);



//----------------------------------------------------------------------------------//
// Save Url																			//
//----------------------------------------------------------------------------------//
public:
	/** Initializes the SaveUrl reference and Save Index from the current SaveGameConfig, and updates the CurrentSave */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool SetCurrentSave(USave* Save);

	/** Initializes the Level Save and level information on the game mode */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool SetCurrentLevelSave(USaved_Level* Save);

	/** Retrieves the current save */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual USave* GetCurrentSave() const;

	/** Retrieves the current save url */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Url") virtual FString GetCurrentSaveUrl() const;
	
	/**
	 * Constructs a base save url using the Account/Platform Id and the Save slot. Subclass for custom logic based on game mode, singleplayer, multiplayer, etc.
	 * - BaseUrl: GameModeType + OwnerAccountId + SaveSlot ->  Adventure_Character1_S1
	 *
	 * @param PlatformId The Account or Platform Id of the server owner
	 * @param Slot The current slot used for saving character information
	 * @returns The base url for creating a save instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Url") virtual FString ConstructSaveUrl(FString PlatformId, int32 Slot) const;
	
	/**
	 * Constructs the save url for levels using the Account/Platform Id and the Save slot. Subclass for custom logic based on game mode, singleplayer, multiplayer, etc.
	 * - SaveLevelUrl: GameModeType + OwnerAccountId + SaveSlot + LevelName + SaveIndex ->  Adventure_Character1_Level1_45
	 *
	 * @param BaseUrl The base url of the specific save (Owner Platform Id + Save Slot)
	 * @param LevelName The name of level
	 * @param OptionalIndex If you need the base level save url, or a specified save index
	 * @returns The base url for creating a save instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Url") virtual FString ConstructLevelSaveUrl(FString BaseUrl, FString LevelName, int32 OptionalIndex = -1) const;
	
	/**
	 * Constructs the save url for players using the Account/Platform Id and the Save slot. Subclass for custom logic based on game mode, singleplayer, multiplayer, etc.
	 * - SavePlayerUrl: GameModeType + OwnerAccountId + SaveSlot + PlayerAccountId + SaveIndex ->  Adventure_Character1_Character1_45
	 *
	 * @param BaseUrl The base url of the specific save (Owner Platform Id + Save Slot)
	 * @param PlayerAccountId The Account or Platform Id of the server owner
	 * @param OptionalIndex If you need the base player save url, or a specified save index
	 * @returns The base url for creating a save instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Url") virtual FString ConstructPlayerSaveUrl(FString BaseUrl, FString PlayerAccountId, int32 OptionalIndex = -1) const;

	/** Utility to append formatted save indexes to the end of save urls */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual FString AppendSaveIndex(int32 Index) const;

	/**

		Character Saving (should be able to switch between singleplayer saves and an account save that is accessed in other games in multiplayer with ease) // SaveComponent
		Level Saving (State of world) // LevelSaveComponent
		Custom Level Saving (objects added to a specific level) - multiplayer specific // -> Create a new component to handle spawning on client / server for different objects

	BaseUrl: GameModeType + AccountId + SaveSlot ->  Adventure_Character1_S1


	USave Url: GameModeType + AccountId + SaveSlot + SaveIndex
		Adventure_Character1_S1_45

	Singleplayer
		Characters:
			Adventure_Character1_S1_Character1_45
				Adventure_Character1_S1_Character1_45_Combat
				Adventure_Character1_S1_Character1_45_Inventory
			Adventure_Character1_S1_Character2_45
				Adventure_Character1_S1_Character1_45_Combat
				Adventure_Character1_S1_Character1_45_Inventory
				
		Levels:
			Adventure_Character1_S1_Level_45
				Adventure_Character1_S1_Level_45_Actor1
				Adventure_Character1_S1_Level_45_Prop1
				Adventure_Character1_S1_Level_45_SpawnedActor3
				Adventure_Character1_S1_Level_45_Weapon4
			
			Adventure_Character1_S1_Level2_100
			Adventure_Character1_S1_Level1_150 <- 45 safety precautions with saving



	Multiplayer (Save state is only used for players, and levels have custom information that's specific to each level)
		Characters:
			- Retrieve character information / loadouts from their account or using the save component logic

		Levels:
			- MP -> MP_CustomLevel1_  ->   Will probably just defer to ULevelInformation stored on a save, and retrieved from a list using CustomLevel_1, CustomLevel_2, CustomLevel_3. etc. 


		An example of the information stored on a custom level that needs to be loaded on server and client
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
// Save Level Logic																	//
//----------------------------------------------------------------------------------//
public:
	/*

		Save Information on a level save, and on an auto save
				- Add a function to the LevelSaveII object for handling what is saved when on the actor
		
	*/


	/**
	 * Saves information of the level, and the world specific state of actors in the level. Optionally saves actor information
	 *
	 * @note TODO: Add save priority based on what kind of objects are saved
	 * @param SaveLevelUrl			The level url used to save information to the level 
	 * @param bSaveActorData		Whether to save the actors combat / inventory information
	 * @returns						Whether the current state information was successfully saved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Level") bool SaveLevel(const FString& SaveLevelUrl, int32 Index, bool bSaveActorData = false);
	virtual bool SaveLevel_Implementation(const FString& SaveLevelUrl, int32 Index, bool bSaveActorData = false);

	/**
	 * Loads the level's save state information and updates the actors within the level. Optionally handles additionally loading the actor's save information
	 *
	 * @note TODO: Add load order based on the objects that we're retrieving from a save
	 * @param SaveLevelUrl			The level url used to retrieve save information about a specific level
	 * @param bRetrieveActorData	Whether to retrieve the actor's save information additionally from the level  
	 * @returns						Whether the save information was successfully retrieved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Level") bool LoadLevel(const FString& SaveLevelUrl, int32 Index, bool bRetrieveActorData = true);
	virtual bool LoadLevel_Implementation(const FString& SaveLevelUrl, int32 Index, bool bRetrieveActorData = true);

	/** Autosave logic for saving components efficiently to prevent performance issues */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Level") bool AutoSaveHandling();
	virtual bool AutoSaveHandling_Implementation();

	/** Adds save information to the PendingSaves array. Used in auto saving actor specific information to the level */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Level") void AddPendingActor(const F_LevelSaveInformation_Actor& SaveInformation);
	virtual void AddPendingActor_Implementation(const F_LevelSaveInformation_Actor& SaveInformation);

	/** Returns whether we're able to currently save the level's state information */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Level") bool IsValidToCurrentlySaveLevel() const;
	virtual bool IsValidToCurrentlySaveLevel_Implementation() const;

	
public:
	/** Resets Level Component's state. Helpful during level transitions */
	UFUNCTION(BlueprintCallable) virtual void ResetLevelSaveComponentState();
	


	
//----------------------------------------------------------------------------------//
// Save Players Logic																//
//----------------------------------------------------------------------------------//
public:
	// TODO: We need valid save logic from the player controller while still saving actors within the level safely -> add the save component to the player state
	/**
	 * Saves all player's information using their own save logic (Save Component reference -> Default / EOS / Etc.)
	 * @returns						Whether the current state information was successfully saved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Players") bool SavePlayers(const FString& BaseSaveUrl, int32 Index);
	virtual bool SavePlayers_Implementation(const FString& BaseSaveUrl, int32 Index);

	/**
	 * Loads all player's information using their own save logic (Save Component reference -> Default / EOS / Etc.)
	 * @returns						Whether the current state information was successfully retrieved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Players") bool LoadPlayers(const FString& BaseSaveUrl, int32 Index);
	virtual bool LoadPlayers_Implementation(const FString& BaseSaveUrl, int32 Index);

	/**
	 * Saves the player's information using their own save logic (Save Component reference -> Default / EOS / Etc.)
	 * @returns						Whether the current state information was successfully saved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Players") bool SavePlayer(APlayerController* PlayerController, const FString& PlayerSaveUrl, int32 Index);
	virtual bool SavePlayer_Implementation(APlayerController* Player, const FString& PlayerSaveUrl, int32 Index);

	/**
	 * Loads the player's information using their own save logic (Save Component reference -> Default / EOS / Etc.)
	 * @returns						Whether the current state information was successfully retrieved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player State|Saving|Players") bool LoadPlayer(APlayerController* PlayerController, const FString& PlayerSaveUrl, int32 Index);
	virtual bool LoadPlayer_Implementation(APlayerController* Player, const FString& PlayerSaveUrl, int32 Index);


protected:
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Players") virtual void UpdatePlayerWithSaveReferences(APlayerController* PlayerController) const;
	

	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/** Retrieves the GameMode's classification */
	UFUNCTION(BlueprintCallable, Category = "Game") virtual EGameModeType GetGameModeType();

	/** Initializes the game's levels */
	UFUNCTION(BlueprintCallable, Category = "Game") virtual bool RetrieveLevels();

	/** Initializes the player's account/platform information and uses it for handling save references. */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual void InitOwnerSavePlatformId();

	/** Retrieves the save platform id */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual FString GetSavePlatformId() const;

	/** Retrieves the current level save information */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Utility") virtual USaved_Level* GetLevelSaveInformation() const;

	/** Prints a message with a reference to the player controller */
	UFUNCTION() virtual void PrintMessage(const FString& Message);
	
	/** Prints a list of the actors within the level */
	UFUNCTION(BlueprintCallable) virtual void PrintActorsInLevel(bool bSavedActors = false);
	
	/** Retrieves a list of the player controllers */
	UFUNCTION(BlueprintCallable, Category = "Game") virtual TArray<APlayerController*> GetPlayers() const;
	
	
};
