// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

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
	EGameModeType GameModeType;

	/** A stored reference to the save logic component */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<ULevelSaveComponent> LevelSaveComponent;
	
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
