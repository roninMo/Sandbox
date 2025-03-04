// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

protected:


public:
	ABasePlayerState(const FObjectInitializer& ObjectInitializer);

	/**
	 * Sends a notification to the game mode to save the current game state.
	 * If there's valid save logic for the game mode, it saves the current state and updates the save iteration for save information to be handled dynamically 
	 *
	 * @param Iteration	The specific index we're saving to. Whether it's an autosave or a specific save is up to the player. Leave as -1 for it to save to the current Save Iteration
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Player State|Saving") virtual void Server_SaveGame(const int32 Iteration = -1);

	
protected:
	
	
};
