// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GameModeLog, Log, All);

/**
 * 
 */
UCLASS()
class SANDBOX_API AMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	// Respawn logic

	// Adventure / TDM / FoF -> subclassed infrastructure
	
	
protected:
	// Respawning
	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;

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


public:
	/** Prints a message with a reference to the player controller */
	UFUNCTION() virtual void PrintMessage(const FString& Message);
	
};
