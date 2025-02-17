// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MultiplayerGameState.generated.h"

/**
 * GameStateBase is a class that manages the game's global state, and is spawned by GameModeBase.
 * 
 * Conceptually, the Game State should manage information that is meant to be known to all connected clients and is specific to the Game Mode but is not specific to any individual player. \n
 *
 * The GameState should track properties that change during gameplay and are relevant and visible to everyone. \n\n
 * While the Game mode exists only on the server, the Game State exists on the server and is replicated to all clients, keeping all connected machines up to date as the game progresses.
 */
UCLASS()
class SANDBOX_API AMultiplayerGameState : public AGameState
{
	GENERATED_BODY()
	
};
