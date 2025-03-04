// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

class USaveGameConfig;
enum class EGameModeType : uint8;
class ULevelSaveComponent;
class USaveLogic;
DECLARE_LOG_CATEGORY_EXTERN(GameModeLog, Log, All);


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

	/** The current save iteration for a specific save slot. This is used for game modes that have save state, and is required before the match begins for each game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameMode|Save State") USaveGameConfig* SaveGameConfig;

	/** A stored reference to the save logic component. Only spawned on the server; handle's the loading and spawning of save state for a Singleplayer/Multiplayer custom game state. */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<ULevelSaveComponent> LevelSaveComponent;
	// TODO: client replication will be required for creating level specific stuff. And we need classifications for objects saved / created. ->  Players / Actors that have save information in level / Actor (Spawned) / Props / etc.  


	// Respawn logic

	// Adventure / TDM / FoF -> subclassed infrastructure

	
public:
	AMultiplayerGameMode(const FObjectInitializer& ObjectInitializer);
	
	/** Called right before components are initialized, only called during gameplay */
	virtual void PreInitializeComponents() override;

	
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
	 * @param Iteration	The specific index we're saving to. Whether it's an autosave or a specific save is up to the player. Leave as -1 for it to save to the current Save Iteration
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool SaveGame(const int32 Iteration = -1);

	/**
	 * Loads the current save state from a save slot.
	 * 
	 * @param Iteration	The specific index we're saving to. Whether it's an autosave or a specific save is up to the player. Leave as -1 for it to save to the current Save Iteration
	 */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving") virtual bool LoadSave(const int32 Iteration);

	// TODO: Is there a more efficient way that prevents this from not being safe?
	/** Finds the player's last save iteration, and set's the current save iteration to it. This is used for retrieving current saves and help with the list of saved information */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual int32 FindSaveIteration() const;

	

	
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

	/** Prints a message with a reference to the player controller */
	UFUNCTION() virtual void PrintMessage(const FString& Message);

	/** Prints a list of the actors within the level */
	UFUNCTION(BlueprintCallable) virtual void PrintActorsInLevel(bool bSavedActors = false);

	
};
