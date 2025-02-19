// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Sandbox/Data/Structs/LevelSaveInformation.h"
#include "Saved_Level.generated.h"


/**
 * Save game logic for persistent level save information
 * Subclass this for custom save logic @ref ULevelSaveComponent
 */
UCLASS()
class SANDBOX_API USaved_Level : public USaveGame
{
	GENERATED_BODY()

public:
	// Day / Night / Weather

	// Time state
	
	// Other level specific saved information

	/* A stored reference to the current save state of the objects in the level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FString, F_LevelSaveInformation_Actor> SavedActors;

public:
	/** Retrieves the saved actors that have been placed in the level, and the ones that have been spawned in the world */
	UFUNCTION(BlueprintCallable, Category = "Level|Saving")
	virtual void GetSavedAndSpawnedActors(ULevel* Level, TArray<AActor*>& OutLevelActors, TArray<F_LevelSaveInformation_Actor>& OutSpawnedActors);
	
	
};
