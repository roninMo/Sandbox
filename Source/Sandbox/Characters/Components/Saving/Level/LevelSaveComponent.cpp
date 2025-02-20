// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSaveComponent.h"

#include "Kismet/GameplayStatics.h"
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

	return LevelName + "_" + Classification;
}
#pragma endregion
