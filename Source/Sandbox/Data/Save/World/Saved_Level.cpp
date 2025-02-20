// Fill out your copyright notice in the Description page of Project Settings.


#include "Saved_Level.h"

#include "Kismet/KismetGuidLibrary.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Data/Enums/ESaveType.h"
#include "Sandbox/Data/Interfaces/Save/LevelSaveInformationInterface.h"
#include "Sandbox/Game/MultiplayerGameMode.h"


void USaved_Level::GetSavedAndSpawnedActors(ULevel* Level, TArray<FString>& OutSpawnedActors, TArray<AActor*>& OutLevelActors, TArray<FString>& OutPlayers)
{
	if (!Level)
	{
		UE_LOGFMT(GameModeLog, Error, "{0}::{1}() Failed to retrieve the actors in the level, the Level wasn't valid!", *GetOuter()->GetName(), *FString(__FUNCTION__));
		return;
	}
	
	// Always loop through the list for map updates (while working on levels?)

	// Loop through the saved actors
	//		- find actors placed in level
	//		- send actors saved that were spawned in world
	TMap<FString, bool> LevelActors;
	for (AActor* Actor : Level->Actors)
	{
		FString ActorLevelId;
		if (!Actor) continue;

		// Is it a saved item
		const bool bLevelSaveInterface = Actor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass());
		if (bLevelSaveInterface)
		{
			ActorLevelId = ILevelSaveInformationInterface::Execute_GetActorLevelId(Actor);

			// Items placed in the level
			LevelActors.Add(ActorLevelId, true); // Spawned actors
			OutLevelActors.Add(Actor);
		}
	}

	// Find and send the spawned actors and player references
	for (auto &[Id, SavedInformation] : SavedActors)
	{
		// Actors spawned in the world
		if (SavedInformation.SaveType == ESaveIdType::SpawnedActor)
		{
			OutSpawnedActors.Add(Id);
		}

		// Player save information
		if (SavedInformation.SaveType == ESaveIdType::Player)
		{
			OutPlayers.Add(Id);
		}
	}
}
