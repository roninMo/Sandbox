// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeSaveLogic.h"
#include "MultiplayerGameMode.generated.h"


enum class EGameModeType : uint8;
class USave;
class USaveLogic;


/**
 * Server logic for handling saving the level and the players on the server, and distributing save information for client's to save individual character information. \n\n
 * The idea is for this to work for singleplayer out of the box and allow you to easily incorporate online multiplayer subsystems for retrieving character information per game.
 *
 * - Epic Online Subsystem example logic for handling lobby events (probably from a player controller's remote procedure calls -> @ref EOSGO_API UGoMenu
 * 
 * TODO: Add asynchronous save logic. Errors that happen while abruptly stopping save functionality rarely corrupt save logic, however we don't want problems with performance
 */
UCLASS()
class SANDBOX_API AMultiplayerGameMode : public AGameModeSaveLogic
{
	GENERATED_BODY()

public:
	// Adventure / TDM / FoF -> subclassed infrastructure

	
public:
	AMultiplayerGameMode(const FObjectInitializer& ObjectInitializer);
	
	/** Called right before components are initialized, only called during gameplay */
	virtual void PreInitializeComponents() override;

	/** Reset actor to initial state - used when restarting level without reloading. */
	virtual void Reset() override;

protected:
	/** Tries to spawn the player's pawn, at the location returned by FindPlayerStart */
	virtual void RestartPlayer(AController* NewPlayer) override;

	/** Tries to spawn the player's pawn at the specified actor's location */
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;



	
//----------------------------------------------------------------------------------//
// Match State Functions															//
//----------------------------------------------------------------------------------//
protected:
	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	/** Called when a Controller with a PlayerState leaves the game or is destroyed */
	virtual void Logout(AController* Exiting) override;
	
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
	UFUNCTION(BlueprintCallable, Category = "Server Travel") virtual bool Travel(FString Map);



	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	
	
};
