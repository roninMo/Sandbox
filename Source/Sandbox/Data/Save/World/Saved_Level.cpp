// Fill out your copyright notice in the Description page of Project Settings.


#include "Saved_Level.h"

#include "EngineUtils.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Data/Interfaces/Save/LevelSaveInformationInterface.h"
#include "Sandbox/Game/MultiplayerGameMode.h"


void USaved_Level::GetSavedAndSpawnedActors(UWorld* World, TArray<AActor*>& OutLevelActors, TArray<AActor*>& OutSpawnedActors)
{
	if (!World)
	{
		UE_LOGFMT(GameModeLog, Error, "{0}::{1}() Failed to retrieve the actors in the level, the world wasn't valid!", *GetOuter()->GetName(), *FString(__FUNCTION__));
		return;
	}

	// Loop through the actors in the level and find the ones that have been spawned
	TMap<FGuid, bool> LevelActors;
	for (FActorIterator Actor(World); Actor; ++Actor)
	{
		FGuid ActorLevelId;
		AActor* LevelActor = *Actor;
		if (!LevelActor) continue; // We NeEd a VaLiD ReFeReNcE tO tHe AcTor

		const bool bLevelSaveInterface = Actor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass());
		if (bLevelSaveInterface)
		{
			ActorLevelId = ILevelSaveInformationInterface::Execute_GetActorLevelId(LevelActor);
		}

		// Is it a saved item
		if (!ActorLevelId.IsValid())
		{
			continue;
		}

		// Check if the actor has been saved the level's actor list
		LevelActors.Add(ActorLevelId, !SavedActors.Contains(ActorLevelId));
	}
}
