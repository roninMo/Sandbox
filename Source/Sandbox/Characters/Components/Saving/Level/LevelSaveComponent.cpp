// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSaveComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Enums/ESaveType.h"
#include "Sandbox/Data/Interfaces/Save/LevelSaveInformationInterface.h"
#include "Sandbox/Data/Save/World/Saved_Level.h"
#include "Sandbox/Game/MultiplayerGameMode.h"


bool ULevelSaveComponent::SaveLevel_Implementation(const FString& SaveUrl, bool bSaveActorData)
{
	// Safety precautions
	if (!GetGameMode() || !GetLevel()) return false;
	
	// Everything's already been saved
	if (PendingActorsToSave.IsEmpty()) return false;

	// Check if we have a valid save game slot, otherwise create one
	FString LevelSaveUrl = SaveUrl + "Level";
	if (!GetSaveGameRef(LevelSaveUrl)) // We're creating / retrieving a current level save, and adding / updating information about things in the level to it
	{
		SaveState = NewObject<USaved_Level>(this, USaved_Level::StaticClass());
		if (!SaveState) return false;
	}


	// Retrieve actors in the level, players, and spawned actors
	TArray<FString> SpawnedActors;
	TArray<FString> Players;
	TArray<AActor*> LevelActors;
	SaveState->GetSavedAndSpawnedActors(GetLevel(), SpawnedActors, LevelActors, Players);

	// Search through the level for actors with save logic, and find the actors that have save state
	for (AActor* Actor : LevelActors)
	{
		if (!Actor) continue;
		if (!Actor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass())) continue;

		FString Id = ILevelSaveInformationInterface::Execute_GetActorLevelId(Actor);
		F_LevelSaveInformation_Actor SaveInformation = ILevelSaveInformationInterface::Execute_SaveToLevel(Actor);
		if (SaveInformation.IsValid())
		{
			// Save Level Information
			TMap<FString, F_LevelSaveInformation_Actor>& LevelSaveInformation = SaveState->SavedActors;
			if (LevelSaveInformation.Contains(Id)) LevelSaveInformation[Id] = SaveInformation;
			else LevelSaveInformation.Add(Id, SaveInformation);

			// Save it to it's own game slot // TODO: this is handled for all character information on their own save component. Either use ILevelSaveInformationInterface to save / retrieve information, or handle it here for non players
			FString ActorSaveUrl = LevelSaveUrl + Id; // SaveComponent -> ActorSaveUrl + SaveComponent save
			if (USaveGame* ActorSaveInformation = UGameplayStatics::LoadGameFromSlot(ActorSaveUrl, 0))
			{
				// ILevelSaveInformationInterface::Execute_Save()
			}

			// Save Actor Information
			if (bSaveActorData)
			{
				ILevelSaveInformationInterface::Execute_SaveActorData(Actor, SaveInformation);
			}
		}
	}

	// Save the level information to the game slot // TODO: This should be asynchronous
	UGameplayStatics::SaveGameToSlot(SaveState, LevelSaveUrl, 0);

	// TODO: Decide on whether the level should contain it's save information for a specific actor, or if we should just add it to the actor, and allow it to create it's save state individually (the same way we handle it with character specific data)
	//				- If we're saving level specific to each character, we need an additional prefix
	
	return true;
}


bool ULevelSaveComponent::LoadLevelSave_Implementation(const FString& SaveUrl, bool bRetrieveActorData)
{
	// Safety precautions
	if (!GetGameMode() || !GetLevel()) return false;
	
	// Everything's already been saved
	if (PendingActorsToSave.IsEmpty()) return false;

	// Check if we have a valid save game slot
	FString LevelSaveUrl = SaveUrl + "_Level";
	if (!GetSaveGameRef(LevelSaveUrl))
	{
		return true;
	}

	
	// Retrieve actors in the level, players, and spawned actors
	TMap<FString, F_LevelSaveInformation_Actor>& SavedActors = SaveState->SavedActors;
	TArray<FString> SpawnedActors;
	TArray<FString> Players;
	TArray<AActor*> LevelActors;
	SaveState->GetSavedAndSpawnedActors(GetLevel(), SpawnedActors, LevelActors, Players);

	// Search through the level for actors with save logic, and find players with save state
	// TODO: Refactor this for proper structure for handling level/spawned actors and player information dynamically, and with efficient retrieving of the references
	for (AActor* Actor : LevelActors)
	{
		if (!Actor) continue;
		if (!Actor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass())) continue;

		FString Id = ILevelSaveInformationInterface::Execute_GetActorLevelId(Actor);
		if (SavedActors.Contains(Id))
		{
			// Load the level actor / current player's saved information
			SavedActors[Id].Actor = Actor;
			const F_LevelSaveInformation_Actor& SaveData = SavedActors[Id];

			// Handle actor specific logic here
			if (SaveData.SaveType == ESaveIdType::LevelActor)
			{
				
			}
			else if (SaveData.SaveType == ESaveIdType::Player)
			{
				
			}
			else if (SaveData.SaveType == ESaveIdType::SpawnedActor)
			{
				
			}

			// Load the actor's information
			if (bRetrieveActorData)
			{
				ILevelSaveInformationInterface::Execute_LoadFromLevel(Actor, SavedActors[Id], bRetrieveActorData);
			}
		}

		// TODO: Logic for handling level actors that don't have save information?
		
	}
	
	// Spawn actors that were previously spawned in the world
	// TODO: Add logic for multiple variations of actors, or after handling singleplayer Level and Spawned actors / Characters, handle custom information like Forge specific stuff from an object (Weapons / Characters / World design / etc. )
	for (FString Id : SpawnedActors)
	{
		if (!SavedActors.Contains(Id)) continue;
		if (!SavedActors[Id].IsValidForSpawning()) continue;

		// Spawn and place the actor in the level
		const F_LevelSaveInformation_Actor& SaveData = SavedActors[Id];
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.OverrideLevel = GetLevel();
		FTransform SpawnTransform = FTransform(SaveData.Rotation, SaveData.Location, FVector(0));

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SaveData.Class, SpawnTransform, SpawnInfo);
		if (!SpawnedActor)
		{
			UE_LOGFMT(GameModeLog, Error, "{0}() {1} Failed to spawn actor {2}!", *FString(__FUNCTION__), *GetName(), *SaveData.Class->GetName());
		}
		else
		{
			// Load the actor's save information
			if (SpawnedActor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass()))
			{
				ILevelSaveInformationInterface::Execute_LoadFromLevel(SpawnedActor, SaveData, bRetrieveActorData);
				
				// TODO: Init logic for when we spawn actors that were saved to the level
			}
		}
	}

	return true;
}


bool ULevelSaveComponent::AutoSaveHandling_Implementation()
{
	// Safety precautions
	if (!GetGameMode() || !GetLevel()) return false;
	
	// Everything's already been saved
	if (PendingActorsToSave.IsEmpty()) return false;

	// Check if we have a valid save game slot, otherwise create one
	FString LevelSaveUrl = GetGameMode()->GetCurrentSaveGameUrl() + "_Level";
	if (!GetSaveGameRef(LevelSaveUrl)) // We're creating / retrieving a current level save, and adding / updating information about things in the level to it
	{
		SaveState = NewObject<USaved_Level>(this, USaved_Level::StaticClass());
		if (!SaveState) return false;
	}
	

	// Update the actors save state for those that are pending save
	for (auto &[Id, Data] : PendingActorsToSave) // TODO: Add ways of handling this based on the server's capacity to handle it at the moment
	{
		TMap<FString, F_LevelSaveInformation_Actor>& SaveData = SaveState->SavedActors;

		// Update the level save information if there's already save information, otherwise initialize the save information
		if (SaveData.Contains(Id)) SaveData[Id] = Data;
		else SaveData.Add(Id, Data);

		// Save character specific state update based on the save configurations. This is where you'd handle saving multiplayer state for specific characters, or just saving that information to the level based on your game
		if (Data.Actor.Get()->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass()))
		{
			ILevelSaveInformationInterface::Execute_SaveActorData(Data.Actor.Get(), Data);
		}
	}

	// Save the information to the save game slot. Subclass to add Subsystem functionality or to save the information to an api
	bool bSuccessfullySaved = UGameplayStatics::SaveGameToSlot(SaveState, LevelSaveUrl, 0);

	// Clear out the pending save list if we saved the data properly
	if (bSuccessfullySaved) PendingActorsToSave.Empty();
	return bSuccessfullySaved;
}


void ULevelSaveComponent::AddPendingActor_Implementation(const F_LevelSaveInformation_Actor& SaveInformation)
{
	if (!SaveInformation.IsValid()) return;

	PendingActorsToSave.Add(SaveInformation.Id, SaveInformation);
}




#pragma region Utility
AMultiplayerGameMode* ULevelSaveComponent::GetGameMode()
{
	return Cast<AMultiplayerGameMode>(GetOuter());
}


ULevel* ULevelSaveComponent::GetLevel()
{
	AMultiplayerGameMode* GameMode = GetGameMode();
	if (!GameMode) return nullptr;

	return GameMode->GetLevel();
}


void ULevelSaveComponent::ResetLevelSaveComponentState()
{
	PendingActorsToSave.Empty();
	SaveState = nullptr;
}


USaved_Level* ULevelSaveComponent::GetSaveGameRef(const FString SaveUrl)
{
	if (!GetGameMode() || !GetLevel())
	{
		return nullptr;
	}
	
	return Cast<USaved_Level>(UGameplayStatics::LoadGameFromSlot(SaveUrl, 0));
}


bool ULevelSaveComponent::IsValidToCurrentlySave_Implementation() const
{
	return true;
}
#pragma endregion
