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
 *
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
	UPROPERTY(BlueprintReadWrite, Transient) FString SaveGameUrl;

	/** The most recent save index for a specific save slot. ie. if it's the 100th save, and we load / save to a previous save, this shouldn't be affected */
	UPROPERTY(BlueprintReadWrite, Transient) int32 SaveIndex;
	

	/** A stored reference to the save logic component. Only spawned on the server; handle's the loading and spawning of save state for a Singleplayer/Multiplayer custom game state. */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<ULevelSaveComponent> LevelSaveComponent;
	// TODO: client replication will be required for creating level specific stuff. And we need classifications for objects saved / created. ->  Players / Actors that have save information in level / Actor (Spawned) / Props / etc.  


	// Respawn logic

	// Adventure / TDM / FoF -> subclassed infrastructure

	
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
	 * @param SaveGameRef The specific slot we're saving to
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool SaveGame(const FString& SaveGameRef, const int32 Index);

	/**
	 * Loads the current save state from a save slot.
	 * 
	 * @param SaveGameRef The specific slot we're saving to
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool LoadSave(const FString& SaveGameRef, const int32 Index);

	/** Initializes the SaveGameReference and Save Index from the current SaveGameConfig, and updates the CurrentSave */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Init") virtual bool UpdateCurrentSave(USave* Save);
	
	// TODO: Is there a more efficient way that prevents this from not being safe?
	/** Finds the player's last save index, and set's the current save index to it. This is used for retrieving current saves and help with the list of saved information */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Init") virtual bool FindCurrentSave() const;


protected:
	/**
	 * Creates another save state, and updates the save reference and index on both the current save, here, and the player states. \n\n
	 * This refers to the current index, and not a specified index during a save
	 * @note Objects in the world should retrieve the SaveGameRef and Index from the game mode on the server
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving|Init") virtual bool CreateNextSave();

	/** Retrieves a reference to the current save game root information */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual USave* GetSaveGameBase();
	
	/** Sets the save game root information */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual void SetSaveGameBase(USave* Save);

	

	
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
