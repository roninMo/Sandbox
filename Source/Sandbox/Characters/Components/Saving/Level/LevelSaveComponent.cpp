// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSaveComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Enums/ESaveType.h"
#include "Sandbox/Data/Enums/GameModeTypes.h"
#include "Sandbox/Data/Interfaces/Save/LevelSaveInformationInterface.h"
#include "Sandbox/Data/Save/World/Saved_Level.h"
#include "Sandbox/Game/MultiplayerGameMode.h"


bool ULevelSaveComponent::SaveCurrentState_Implementation(bool bSaveActorData)
{
	// Everything's already been saved
	if (PendingActorsToSave.IsEmpty())
	{
		return true;
	}

	// Retrieve the SaveGameSlot
	FName SaveSlot = FName("LevelSave");
	if (GetGameMode() && GetLevel())
	{
		SaveSlot = FName(GetSaveGameSlot(GetLevel()->GetName(), GetGameMode()->GetGameModeType()));
	}

	// Check if we have a valid save game slot, otherwise create one
	if (!GetSaveGameRef())
	{
		SaveState = NewObject<USaved_Level>(this, USaved_Level::StaticClass(), SaveSlot);
		if (!SaveState) return false;
	}

	// Update the actors that are pending save
	for (auto &[Id, Data] : PendingActorsToSave)
	{
		TMap<FString, F_LevelSaveInformation_Actor>& SaveData = SaveState->SavedActors;

		// Update the information if there's already save information, otherwise initialize the save information
		if (SaveData.Contains(Id)) SaveData[Id] = Data;
		else SaveData.Add(Id, Data);

		// If we're saving actor state, update based on the save configurations
		if (bSaveActorData && Data.Actor.Get() && Data.Actor.Get()->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass()))
		{
			ILevelSaveInformationInterface::Execute_SaveActorData(Data.Actor.Get(), Data);
		}
	}

	// Save the information to the save game slot. Subclass to add Subsystem functionality or to save the information to an api
	bool bSuccessfullySaved = UGameplayStatics::SaveGameToSlot(SaveState, SaveSlot.ToString(), 0);

	// Clear out the pending save list if we saved the data properly
	if (bSuccessfullySaved) PendingActorsToSave.Empty();
	return bSuccessfullySaved;
}


bool ULevelSaveComponent::LoadCurrentState_Implementation(bool bRetrieveActorData)
{
	if (!GetGameMode() || !GetLevel())
	{
		return false;
	}
	
	SaveState = GetSaveGameRef();
	if (!SaveState)
	{
		return true;
	}

	const TMap<FString, F_LevelSaveInformation_Actor>& SavedActors = SaveState->SavedActors;
	TArray<ACharacterBase*> Characters;
	TArray<FString> SpawnedActors;
	TArray<FString> Players;
	TArray<AActor*> LevelActors;

	// Load level information and the spawned actors, and the actor information
	// TODO: Refactor this for proper structure for handling level/spawned actors and player information dynamically, and with efficient retrieving of the references
	SaveState->GetSavedAndSpawnedActors(GetLevel(), SpawnedActors, LevelActors, Players);

	// Search through the level for actors with save logic, and find players with save state
	for (AActor* Actor : LevelActors)
	{
		if (!Actor) continue;
		if (!Actor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass())) continue;

		FString Id = ILevelSaveInformationInterface::Execute_GetActorLevelId(Actor);
		if (SavedActors.Contains(Id))
		{
			// Load the level actor / current player's saved information
			const F_LevelSaveInformation_Actor& SaveData = SavedActors[Id]; // don't update references until we save information
			if (SaveData.SaveType == ESaveIdType::LevelActor)
			{
				
			}
			else if (SaveData.SaveType == ESaveIdType::Player)
			{
				
			}

			// Load the actor's information
			ILevelSaveInformationInterface::Execute_LoadFromLevel(Actor, SavedActors[Id], bRetrieveActorData);
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


USaved_Level* ULevelSaveComponent::GetSaveGameRef()
{
	if (SaveState || !GetGameMode() || !GetLevel())
	{
		return SaveState;
	}

	FString LevelSaveSlot = GetSaveGameSlot(GetLevel()->GetName(), GetGameMode()->GetGameModeType());
	return Cast<USaved_Level>(UGameplayStatics::LoadGameFromSlot(LevelSaveSlot, 0));
}


bool ULevelSaveComponent::IsValidToCurrentlySave_Implementation() const
{
	return true;
}


FString ULevelSaveComponent::GetSaveGameSlot(const FString& LevelName, EGameModeType GameModeType) const
{
	if (LevelName.IsEmpty() || GameModeType == EGameModeType::None)
	{
		return FString();
	}

	FString Classification;
	switch (GameModeType) {
		case EGameModeType::Adventure: Classification = "Adventure";
		case EGameModeType::FoF: Classification = "FoF";
		case EGameModeType::Kotk: Classification = "Kotk";
		case EGameModeType::SoD: Classification = "SoD";
		case EGameModeType::TDM: Classification = "TDM";
		default: FString();
	}

	/**

	 SinglePlayer
		- Adventure
			- Specific levels, and save game state for each game

	 Multiplayer
		- Team Deathmatch, Free for All, etc.
		- Custom Games
			- Both could have a custom level, normal multiplayer would just have traditional logic


	Lobby
		- GameMode, Level, CustomSaveState/Level Information, GameMode logic for the type of game ->  ready for play

	// SaveGameBase: (GameMode + PlatformId + SlotId + SaveIndex ++ additional SaveComponent logic for each actor)
	// Retrieve the Save Iteration individually for fallbacks in case certain save components didn't save properly
		// SaveGameBase + "_" Iteration + "_" + Individual Save Component References
		// SaveGameBase + "_" Iteration + "_" + Level + Prop / Object References -> Individual Save Component

	Singleplayer
		- Adventure_Character1_S1_54
			Character:
				->  Adventure_Character1_S1_54_Combat
				->  Adventure_Character1_S1_54_Inventory
				->  Adventure_Character1_S1_54_CameraSettings
				->  Adventure_Character1_S1_54_Attributes
				->  Adventure_Character1_S1_54_Level
					->  Adventure_Character1_S1_54_Level_Actor0
						->  Adventure_Character1_S1_54_Level_Actor0_etc
				
		Actors placed in the level with save information:
				-> Adventure_Character1_S1_54_Level + _Actor0
					-> Adventure_Character1_S1_54_Level + _Actor0_Inventory
					-> Adventure_Character1_S1_54_Level + _Actor0_Level
				-> Adventure_Character1_S1_54_Level + _Actor1
					-> Adventure_Character1_S1_54_Level + _Actor1_Inventory
					-> Adventure_Character1_S1_54_Level + _Actor1_Level

		Actors spawned in the world with save logic:
			-> Adventure_Character1_S1_54_Level + _ActorSpawnRef_Level
			-> Adventure_Character1_S1_54_Level + _ActorSpawnRef_Combat
			-> Adventure_Character1_S1_54_Level + _ActorSpawnRef_Inventory

	Cooperative
		- When players join the lobby, online subsystems would retrieve character information individually from the server
			- If we're still using SaveGame state on the server, it would be retrieved in reference to the host's character reference, and what he saved for each character (using their Account/Platform Id)
				- Adventure_Character1_S1_54
				- Adventure_Character2_S1_54 -> and this breaks the logic
					- If it's a player and it isn't the host, the save logic will treat it as a player, and save and load it's information the same way, let's just add it to the save like this:
					- Adventure_Character1_S1_54 + _Character2 -> and use this as the save game reference
				- So we use the owner's Account/PlatformId for the Save Reference, and additionally for their own save information
					- Adventure_Character1_S1_54_Character1
					- Adventure_Character1_S1_54_Character2
					- Adventure_Character1_S1_54_Character3
					


	Multiplayer (for save state, we're probably just not going to use a character reference, unless it's a custom game mode. we'll figure out how to handle this later)
		- MP_CustomLevel1
		- MP_CustomLevel2
			Props, Actors, Enemies, Weapons, GameMode Objects, Vehicles, etc spawned in the world should be handled separately here before we handle character logic, on client / server, retrieved from the specified level, spawn presets, etc.
				-> MP_CustomLevel1_Props_Ramp (The level's custom components probably won't be spawned on the client automatically)
				-> MP_CustomLevel1_Props_Object1
				-> MP_CustomLevel1_Actors_ShieldPickup_1
					-> MP_CustomLevel1_Actors_ShieldPickup_1_Level (Transform)
				-> MP_CustomLevel1_Actors_WeaponCrate_0
					-> MP_CustomLevel1_Actors_WeaponCrate_0_Level
					-> MP_CustomLevel1_Actors_WeaponCrate_0_Inventory

		// The benefit of saving this way is effecient retrieving of information individually, and that doesn't necessarily mean that this is correct. Finding a way to save each level in their own folder should help make sense of it.
		//		However with online subsystem logic, the information would be saved and retrieved asynchronously from the server while the level information works this way
		//			So you'd just have to add that logic to the save component for properly saving / retrieving the information




	->  SaveGame Reference for retrieving Level / Character information
		- OnMatchBegin build the level, character / spawned information on server, and spawned level objects on both Client and Server 





	 */

	return Classification + "_" + LevelName;
}
#pragma endregion
