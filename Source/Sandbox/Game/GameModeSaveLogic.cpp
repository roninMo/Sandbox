// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeSaveLogic.h"

#include "GameModeLibrary.h"
#include "Instances/MultiplayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Characters/Player/BasePlayerState.h"
#include "Sandbox/Characters/Player/PlayerCharacter.h"
#include "Sandbox/Data/Enums/ESaveType.h"
#include "Sandbox/Data/Enums/GameModeTypes.h"
#include "Sandbox/Data/Interfaces/Save/LevelSaveInformationInterface.h"
#include "Sandbox/Data/Save/Save.h"
#include "Sandbox/Data/Save/World/Saved_Level.h"


DEFINE_LOG_CATEGORY(GameModeLog);


AGameModeSaveLogic::AGameModeSaveLogic(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// The game mode's classification
	GameModeType = EGameModeType::None;
	
	// Save information pertaining to each Game Mode
	CurrentSave = nullptr;
	CurrentLevelSave = nullptr;

	// Retrieve this game's levels
	AGameModeSaveLogic::RetrieveLevels();

	// Retrieve saved information when traveling between levels (On game resets)
	AGameModeSaveLogic::RetrieveGameModeInformation();
}

void AGameModeSaveLogic::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StoreGameModeInformation();
	PrintMessage("EndPlay");
	Super::EndPlay(EndPlayReason);
}

void AGameModeSaveLogic::BeginPlay()
{
	Super::BeginPlay();
	PrintMessage("BeginPlay");

	RetrieveGameModeInformation();
}

void AGameModeSaveLogic::StoreGameModeInformation()
{
	if (!GetWorld()) return;
	UMultiplayerGameInstance* GameInstance = Cast<UMultiplayerGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	GameInstance->SetGameModeType(GameModeType);
	GameInstance->SetCurrentSave(CurrentSave);
	GameInstance->SetCurrentLevelSave(CurrentLevelSave);
}

void AGameModeSaveLogic::RetrieveGameModeInformation()
{
	if (!GetWorld()) return;

	// Levels
	RetrieveLevels();

	// Level persistence information
	UMultiplayerGameInstance* GameInstance = Cast<UMultiplayerGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	GameModeType = GameInstance->GetGameModeType();
	if (!CurrentSave) CurrentSave = GameInstance->GetCurrentSave();
	if (!CurrentLevelSave)
	{
		CurrentLevelSave = GameInstance->GetCurrentLevelSave();
		CurrentLevel = CurrentSave ? CurrentSave->LevelInformation.LevelName : "";
	}
}


#pragma region Saving and Loading
bool AGameModeSaveLogic::SaveGame(const FString& BaseSaveUrl, const int32 Index)
{
	// Handle save information specific to multiplayer game state here (Quests, objectives, etc.)
	//	- Games with save information that persists across multiple games, or from singleplayer / multiplayer should have custom save logic for save / retrieving that information 
	
	// Save the player information
	SavePlayers(BaseSaveUrl, Index);

	// Save the level information ->  TODO: Check if we're on the proper level to save level information
	// TODO: Update save state to be two separate things based on whether the information is persistent
	//			- Actor->SaveToLevel() saves level information, and optionally additionally save character specific information
	//			- Actor->SaveActorData() saves character specific information
	//			- Having a function that binds to the level save component to update latent information (like inventory updates, weapon equips, etc. should be handled at all times)
	SaveLevel(BaseSaveUrl, Index, true);
	
	return true;
}


bool AGameModeSaveLogic::LoadSave(const FString& BaseSaveUrl, const int32 Index)
{
	// Safety precautions
	if (!GetWorld()) return false;

	// Check if it's a valid save
	FString SaveUrl = BaseSaveUrl + AppendSaveIndex(Index);
	USave* Save = Cast<USave>(UGameplayStatics::LoadGameFromSlot(SaveUrl, 0));
	if (!Save)
	{
		UE_LOGFMT(GameModeLog, Error, "{0}() SaveUrl {1} is invalid", *FString(__FUNCTION__), *SaveUrl);
		return false;
	}
	
	// Update the current save before loading the level information
	if (!SetCurrentSave(Save))
	{
		UE_LOGFMT(GameModeLog, Error, "{0}() An error occurred while trying to set the current save to {1}, {2}", *FString(__FUNCTION__), *GetNameSafe(Save), *SaveUrl);
		return false;
	}

	// Travel to the level
	if (!GetWorld()->ServerTravel(Save->LevelInformation.LevelUrl))
	{
		UE_LOGFMT(GameModeLog, Error, "{0}() Failed to travel to the world while trying to load a game save!", *FString(__FUNCTION__));

		// TODO: Add logic for handling server travel errors
		return false;
	}

	

	// Handle save information specific to multiplayer game state here (Quests, objectives, etc.)
	//	- Games with save information that persists across multiple games, or from singleplayer / multiplayer should have custom save logic for save / retrieving that information

	// Player save logic
	FString PlayerUrl = ConstructPlayerSaveUrl(BaseSaveUrl, SavePlatformId);
	if (PlayerUrl.IsEmpty()) return false;
	LoadPlayers(PlayerUrl, Index);

	// Level save logic
	if (CurrentLevel == Save->LevelInformation.LevelName)
	{
		FString LevelUrl = ConstructLevelSaveUrl(BaseSaveUrl, Save->LevelInformation.LevelName);
		if (LevelUrl.IsEmpty()) return false;
		LoadLevel(LevelUrl, Index);
	}
	
	return true;
}


#pragma region Save Handling
bool AGameModeSaveLogic::FindCurrentSave(const FString& BaseUrl, FString& OutSaveUrl, int32& OutSaveIndex) const
{
	// Construct the SaveGameId
	int32 Index = 0;
	FString SaveUrl = BaseUrl + AppendSaveIndex(Index);
	
	// Check if there's save information for this slot
	if (!UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() wasn't save information for SaveUrl: {2}, Index: {3}", *FString(__FUNCTION__), *SaveUrl, Index);
		return false;
	}

	// Find the most recent save, and delete the saves
	int32 SearchIndex = Index;
	int32 SearchStep = 100;
	bool bFoundCurrentSaveIndex = false;
	int32 bSaveSearchErrorCount = 0;
	
	FString LastValidSaveUrl = SaveUrl;
	int32 LastValidSaveIndex = 0;
	while (!bFoundCurrentSaveIndex || bSaveSearchErrorCount < 5)
	{
		SearchIndex = SearchIndex + SearchStep;
		SaveUrl = BaseUrl + AppendSaveIndex(SearchIndex);

		// If there's still save slots
		if (UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
		{
			LastValidSaveUrl = SaveUrl;
			LastValidSaveIndex = SearchIndex;
			continue;
		}
		
		// If the search step is one, this is the most current save
		if (SearchStep == 1)
		{
			bFoundCurrentSaveIndex = true;
			SearchIndex = SearchIndex - 1;

			bSaveSearchErrorCount++;
		}
		else // Backstep and search again  // IndexToFind = 479 ->  100, 200, 300, 400, 500/, 450, 500/, 475, etc.
		{
			SearchIndex = SearchIndex - SearchStep;
			SearchStep = SearchStep / 2;
			// if (SearchStep == 0) SearchStep = 1;
		}
	}

	// An error occured 
	if (bSaveSearchErrorCount >= 5)
	{
		OutSaveUrl = LastValidSaveUrl;
		OutSaveIndex = LastValidSaveIndex;
	}
	else
	{
		OutSaveUrl = BaseUrl + AppendSaveIndex(SearchIndex);
		OutSaveIndex = SearchIndex;
	}
	return true;
}


TArray<FString> AGameModeSaveLogic::FindPreviousSaves(const FString& SaveUrl, int32 CurrentSaveIndex, int32 SavesToRetrieve) const
{
	TArray<FString> PreviousSaves;
	for (int32 Index = CurrentSaveIndex - 1; Index >= 0; Index--)
	{
		// Check if it's a valid save
		FString CurrentSaveUrl = SaveUrl + AppendSaveIndex(Index);
		if (UGameplayStatics::DoesSaveGameExist(CurrentSaveUrl, 0)) PreviousSaves.AddUnique(CurrentSaveUrl);
		if (PreviousSaves.Num() >= SavesToRetrieve) return PreviousSaves;
	}

	return PreviousSaves;
}

TArray<int32> AGameModeSaveLogic::FindPreviousSaveIndexes(const FString& SaveUrl, int32 CurrentSaveIndex, int32 SavesToRetrieve) const
{
	TArray<int32> PreviousSaves;
	for (int32 Index = CurrentSaveIndex - 1; Index >= 0; Index--)
	{
		// Check if it's a valid save
		FString CurrentSaveUrl = SaveUrl + AppendSaveIndex(Index);
		if (UGameplayStatics::DoesSaveGameExist(CurrentSaveUrl, 0)) PreviousSaves.AddUnique(Index);
		if (PreviousSaves.Num() >= SavesToRetrieve) return PreviousSaves;
	}

	return PreviousSaves;
}


bool AGameModeSaveLogic::ReadyNextSave()
{
	if (!CurrentSave) return false;

	// Create a new save, and update the game state
	USave* Save = NewObject<USave>(this, USave::StaticClass());
	if (!Save)
	{
		return false;
	}

	// Check if the next save index is empty, if not increment until we find an empty slot
	FString Url;
	int32 CurrentIndex;
	FindCurrentSave(Save->SaveUrl, Url, CurrentIndex);
	if (UGameplayStatics::DoesSaveGameExist(Save->SaveUrl + AppendSaveIndex(CurrentIndex + 1), 0))
	{
		// Something is wrong with finding the most recent function
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to find the next save index while readying the next save! Url: {1}, Index: {2}", *FString(__FUNCTION__), Url, CurrentIndex + 1);
		return false;
	}

	// Save 
	Save->SaveUrl = ConstructSaveUrl(CurrentSave->PlatformId, CurrentSave->SaveSlot);
	Save->SaveInformation(
		CurrentSave->SaveName,
		CurrentSave->Description,
		CurrentSave->NetId,
		CurrentSave->PlatformId,
		CurrentSave->SaveSlot,
		CurrentIndex + 1
	);

	// Level Save
	FString LevelSaveUrl = ConstructLevelSaveUrl(Save->SaveUrl, Save->LevelInformation.LevelName, Save->SaveIndex);

	// Player Save
	FString PlayerSaveUrl = ConstructPlayerSaveUrl(Save->SaveUrl, Save->LevelInformation.LevelName, Save->SaveIndex);
	

	// Additional save state here
	

	
	// Update the game mode and player state to notify the clients to start saving to the next instance
	if (!SetCurrentSave(Save))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() Failed to update the game's save index, an invalid save game has been provided", *FString(__FUNCTION__));
		return false;
	}

	return true;
}


bool AGameModeSaveLogic::DeleteSaveSlot(const FString& AccountId, int32 SaveSlot)
{
	// Construct the SaveGameId
	FString SaveUrl = ConstructSaveUrl(AccountId, SaveSlot).Append(AppendSaveIndex(0));

	// Check if there's save information for this slot
	if (!UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() wasn't save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return true;
	}

	// Find the most recent save, and delete the saves
	FString BaseSaveUrl = ConstructSaveUrl(AccountId, SaveSlot);
	FString CurrentSaveUrl; // TODO: This needs to be adjusted
	int32 CurrentSaveIndex = 0;
	if (!FindCurrentSave(BaseSaveUrl, CurrentSaveUrl, CurrentSaveIndex))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() couldn't find the current save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return true;
	}

	// Delete all the previous saves
	USave* SaveToDelete = Cast<USave>(UGameplayStatics::LoadGameFromSlot(CurrentSaveUrl, 0));
	if (SaveToDelete)
	{
		for (int32 i = CurrentSaveIndex; CurrentSaveIndex > 0; i--)
		{
			// Actual Saves
			CurrentSaveUrl = ConstructSaveUrl(AccountId, SaveSlot).Append(AppendSaveIndex(i));
			UGameplayStatics::DeleteGameInSlot(CurrentSaveUrl, 0);
			
			// Level Saves
			for (FString Level : SaveToDelete->SavedLevels)
			{
				FString LevelSaveUrl = ConstructLevelSaveUrl(BaseSaveUrl, Level, CurrentSaveIndex);
				UGameplayStatics::DeleteGameInSlot(LevelSaveUrl, 0);
			}

			// Player Saves
			for (FString Player : SaveToDelete->SavedPlayers)
			{
				FString PlayerSaveUrl = ConstructPlayerSaveUrl(BaseSaveUrl, Player, CurrentSaveIndex).Append(AppendSaveIndex(i));
				UGameplayStatics::DeleteGameInSlot(PlayerSaveUrl, 0);
			}
		}
	}
	
	return true;
}


USave* AGameModeSaveLogic::NewSaveSlot(const FString& AccountId, int32 SaveSlot)
{
	// Construct the SaveGameId
	FString SaveUrl = ConstructSaveUrl(AccountId, SaveSlot).Append(AppendSaveIndex(0));

	// Check if there's already save information for this slot
	if (UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() Unable to create a new save, there's already a save in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return nullptr;
	}

	// Create the initial save
	USave* Save = NewObject<USave>(this, USave::StaticClass());
	if (Save)
	{
		Save->SaveInformation(AccountId, "", -1, AccountId, SaveSlot, 0);

		FString BaseUrl = ConstructSaveUrl(AccountId, SaveSlot);
		Save->SaveUrl = BaseUrl;
	}

	return Save;
}
#pragma endregion
#pragma endregion




#pragma region Level Save Logic
bool AGameModeSaveLogic::SaveLevel_Implementation(const FString& SaveLevelUrl, int32 Index, bool bSaveActorData)
{
	// Safety precautions
	if (!GetLevel()) return false;

	// Check if we have a valid save game slot, otherwise create one
	USaved_Level* LevelSave = Cast<USaved_Level>(UGameplayStatics::LoadGameFromSlot(SaveLevelUrl + AppendSaveIndex(Index), 0));
	if (!LevelSave)
	{
		LevelSave = NewObject<USaved_Level>(this, USaved_Level::StaticClass());
		if (!LevelSave) return false;
	}
	
	
	// Retrieve actors in the level, players, and spawned actors
	TArray<FString> SpawnedActors;
	TArray<FString> Players;
	TArray<AActor*> LevelActors;
	CurrentLevelSave = LevelSave;
	CurrentLevelSave->GetSavedAndSpawnedActors(GetLevel(), SpawnedActors, LevelActors, Players);

	// Search through the level for actors with save logic, and find the actors that have save state
	for (AActor* Actor : LevelActors)
	{
		if (!Actor) continue;
		if (!Actor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass())) continue;
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Actor)) continue;

		// Retrieve save information
		FString Id = ILevelSaveInformationInterface::Execute_GetActorLevelId(Actor);
		F_LevelSaveInformation_Actor SaveInformation = ILevelSaveInformationInterface::Execute_SaveToLevel(Actor);
		if (SaveInformation.IsValid())
		{
			// Save Level Information
			TMap<FString, F_LevelSaveInformation_Actor>& LevelSaveInformation = CurrentLevelSave->SavedActors;
			if (LevelSaveInformation.Contains(Id)) LevelSaveInformation[Id] = SaveInformation;
			else LevelSaveInformation.Add(Id, SaveInformation);

			// If we save it to it's own game slot // TODO: this is handled for all character information on their own save component. Either use ILevelSaveInformationInterface to save / retrieve information, or handle it here for non players
			FString ActorSaveUrl = SaveLevelUrl + AppendSaveIndex(Index) + "_" + Id; // SaveComponent -> ActorSaveUrl + SaveComponent save
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

	// TODO: Add to saved levels list
	
	// Save the level information to the game slot
	return UGameplayStatics::SaveGameToSlot(CurrentLevelSave, SaveLevelUrl + AppendSaveIndex(Index), 0);
}


bool AGameModeSaveLogic::LoadLevel_Implementation(const FString& SaveLevelUrl, int32 Index, bool bRetrieveActorData)
{
	// Safety precautions
	if (!GetLevel()) return false;

	// Check if we have a valid save game slot
	CurrentLevelSave = Cast<USaved_Level>(UGameplayStatics::LoadGameFromSlot(SaveLevelUrl + AppendSaveIndex(Index), 0));
	if (!CurrentLevelSave)
	{
		// Try to find a previous save where the player was on this level
		// if (!FindCurrentSave(SaveLevelUrl))

		UE_LOGFMT(GameModeLog, Error, "{0}() {1} Failed to retrieve save information from {2}!", *FString(__FUNCTION__), *GetName(), *SaveLevelUrl);
		return false;
	}
	
	// Retrieve actors in the level, players, and spawned actors
	TMap<FString, F_LevelSaveInformation_Actor>& SavedActors = CurrentLevelSave->SavedActors;
	TArray<FString> SpawnedActors;
	TArray<FString> Players;
	TArray<AActor*> LevelActors;
	CurrentLevelSave->GetSavedAndSpawnedActors(GetLevel(), SpawnedActors, LevelActors, Players);

	// Search through the level for actors with save logic, and find players with save state
	// TODO: Refactor this for proper structure for handling level/spawned actors and player information dynamically, and with efficient retrieving of the references
	for (AActor* Actor : LevelActors)
	{
		if (!Actor) continue;
		if (!Actor->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass())) continue;
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Actor)) continue;

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
				
				// Init logic for when we spawn actors that were saved to the level
			}
		}
	}

	return true;
}


bool AGameModeSaveLogic::AutoSaveHandling_Implementation()
{
	// Safety precautions
	if (!CurrentSave || !GetLevel()) return false;
	
	// Everything's already been saved
	if (PendingSaves.IsEmpty()) return false;

	// Check if we have a valid save game slot, otherwise create one
	FString IndexedLevelSaveUrl = GetCurrentSaveUrl() + "_" + CurrentSave->LevelInformation.LevelName + AppendSaveIndex(CurrentSave->SaveIndex);
	
	// Update the actors save state for those that are pending save
	for (auto &[Id, Data] : PendingSaves) // TODO: Add ways of handling this based on the server's capacity to handle it at the moment
	{
		TMap<FString, F_LevelSaveInformation_Actor>& SaveData = CurrentLevelSave->SavedActors;

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
	bool bSuccessfullySaved = UGameplayStatics::SaveGameToSlot(CurrentLevelSave, IndexedLevelSaveUrl, 0);

	// Clear out the pending save list if we saved the data properly
	if (bSuccessfullySaved) PendingSaves.Empty();
	return bSuccessfullySaved;
}


void AGameModeSaveLogic::AddPendingActor_Implementation(const F_LevelSaveInformation_Actor& SaveInformation)
{
	if (!SaveInformation.IsValid()) return;

	PendingSaves.Add(SaveInformation.Id, SaveInformation);
}


bool AGameModeSaveLogic::IsValidToCurrentlySaveLevel_Implementation() const
{
	return true;
}


void AGameModeSaveLogic::ResetLevelSaveComponentState()
{
	PendingSaves.Empty();
	CurrentLevelSave = nullptr;
}
#pragma endregion




#pragma region Player Save Logic
bool AGameModeSaveLogic::SavePlayers_Implementation(const FString& BaseSaveUrl, int32 Index)
{
	if (!CurrentSave) return false;

	bool bSuccessfullySavedPlayers = true;
	for (APlayerController* Player : GetPlayers())
	{
		// Check if it's a player with save information
		// if (Player->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass()))
		ACharacterBase* Character = Cast<ACharacterBase>(Player->GetPawn());
		if (!Character) continue;

		if (!SavePlayer(Player, Character->Execute_GetActorLevelId(Character), Index))
		{
			bSuccessfullySavedPlayers = false;
		}
	}

	return bSuccessfullySavedPlayers;
}

bool AGameModeSaveLogic::LoadPlayers_Implementation(const FString& BaseSaveUrl, int32 Index)
{
	if (!CurrentSave) return false;

	bool bSuccessfullyLoadedPlayers = true;
	for (APlayerController* Player : GetPlayers())
	{
		// Check if it's a player with save information
		// if (Player->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass()))
		ACharacterBase* Character = Cast<APlayerCharacter>(Player->GetPawn());
		if (!Character) continue;

		if (!LoadPlayer(Player, Character->Execute_GetActorLevelId(Character), Index))
		{
			bSuccessfullyLoadedPlayers = false;
		}
	}

	return bSuccessfullyLoadedPlayers;
}

bool AGameModeSaveLogic::SavePlayer_Implementation(APlayerController* PlayerController, const FString& PlayerSaveUrl, int32 Index)
{
	// Sanity checks
	if (!PlayerController) return false;
	if (!CurrentSave) return false;

	ACharacterBase* Player = PlayerController->GetPawn<ACharacterBase>();
	if (!Player)
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to load {1} save state, the controller doesn't have a valid character reference!", *FString(__FUNCTION__), *GetNameSafe(Player));
		return false;
	}

	// Save the player's information
	if (Player->GetSaveComponent())
	{
		UpdatePlayerWithSaveReferences(PlayerController);
		// TODO: Add to saved players list
		Player->GetSaveComponent()->SaveData(ESaveType::All);
	}

	return true;
}

bool AGameModeSaveLogic::LoadPlayer_Implementation(APlayerController* PlayerController, const FString& PlayerSaveUrl, int32 Index)
{
	// Sanity checks
	if (!PlayerController) return false;
	if (!CurrentSave) return false;
	
	ACharacterBase* Player = PlayerController->GetPawn<ACharacterBase>();
	if (!Player)
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to load {1} save state, the controller doesn't have a valid character reference!", *FString(__FUNCTION__), *GetNameSafe(Player));
		return false;
	}

	// TODO: We need LevelSaveII to have configurations for autosaving and player saving/loading
	// if (Player->GetClass()->ImplementsInterface(ULevelSaveInformationInterface::StaticClass())) 
	if (Player->GetSaveComponent())
	{
		UpdatePlayerWithSaveReferences(PlayerController);
		Player->GetSaveComponent()->LoadPlayerInformation();
	}

	return true;
}


void AGameModeSaveLogic::UpdatePlayerWithSaveReferences(APlayerController* PlayerController) const
{
	ABasePlayerState* PlayerState = PlayerController->GetPlayerState<ABasePlayerState>();
	if (PlayerState && CurrentSave)
	{
		PlayerState->SetSaveUrl(GetCurrentSaveUrl());
		PlayerState->SetSaveIndex(CurrentSave->SaveIndex);
		PlayerState->SetSaveSlot(CurrentSave->SaveSlot);
	}
}
#pragma endregion




#pragma region Save Url
bool AGameModeSaveLogic::SetCurrentSave(USave* Save)
{
	if (!Save)
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to update the game's save state, the save game provided was null", *FString(__FUNCTION__));
		return false;
	}

	// Update the current save
	CurrentSave = Save;
	CurrentSave->SaveUrl = ConstructSaveUrl(SavePlatformId, CurrentSave->SaveSlot);
	
	// Update the player's client information with the current save reference (for individual save components, this is singleplayer logic)
	for (APlayerController* PlayerController : GetPlayers())
	{
		UpdatePlayerWithSaveReferences(PlayerController);
	}

	return true;
}

bool AGameModeSaveLogic::SetCurrentLevelSave(USaved_Level* Save)
{
	if (!Save || !CurrentSave)
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to update the game's level save state, the save game provided was null", *FString(__FUNCTION__));
		return false;
	}

	CurrentLevelSave = Save;
	CurrentLevel = CurrentSave->LevelInformation.LevelName;
	return true;
}


USave* AGameModeSaveLogic::GetCurrentSave() const
{
	return CurrentSave;
}

FString AGameModeSaveLogic::GetCurrentSaveUrl() const
{
	if (!CurrentSave) return FString();
	return CurrentSave->SaveUrl;
}

FString AGameModeSaveLogic::ConstructSaveUrl(FString PlatformId, int32 Slot) const
{
	// BaseUrl: GameModeType + OwnerAccountId + SaveSlot ->  Adventure_Character1_S1
	return UGameModeLibrary::GameModeTypeToString(GameModeType)
			.Append("_")
			.Append(PlatformId) // TODO: Dedicated multiplayer servers need logic for retrieving the host, or separate functionality for multiplayer
			.Append("_")
			.Append("S").Append(FString::FromInt(Slot));
}

FString AGameModeSaveLogic::ConstructLevelSaveUrl(FString BaseUrl, FString LevelName, int32 OptionalIndex) const
{
	if (LevelName.IsEmpty()) return FString();
	FString SaveUrl = BaseUrl.Append("_").Append(LevelName);
	if (OptionalIndex >= 0) SaveUrl += "_" + FString::FromInt(OptionalIndex);
	return SaveUrl;
}

FString AGameModeSaveLogic::ConstructPlayerSaveUrl(FString BaseUrl, FString PlayerAccountId, int32 OptionalIndex) const
{
	if (PlayerAccountId.IsEmpty()) return FString();
	FString SaveUrl = BaseUrl.Append("_").Append(PlayerAccountId);
	if (OptionalIndex >= 0) SaveUrl += "_" + FString::FromInt(OptionalIndex);
	return SaveUrl;
}

FString AGameModeSaveLogic::AppendSaveIndex(int32 Index) const
{
	return "_" + FString::FromInt(Index);
}
#pragma endregion




#pragma region Utility
EGameModeType AGameModeSaveLogic::GetGameModeType()
{
	return GameModeType;
}

bool AGameModeSaveLogic::RetrieveLevels()
{
	if (!LevelInformationTable) return false;
	
	// Retrieve this game's levels
	TArray<F_LevelInformation*> DatabaseLevels;
	LevelInformationTable->GetAllRows(TEXT("Retrieve Levels"), DatabaseLevels);
	const FString RowContext(TEXT("Item Information Context"));
	// if (const F_LevelInformation* Data = ItemInformationTable->FindRow<FInventory_ItemDatabase>(Id, RowContext))

	for (F_LevelInformation* LevelInformation : DatabaseLevels)
	{
		if (!LevelInformation) continue;
		// if (!LevelInformation->IsValid()) continue;

		Levels.Add(LevelInformation->LevelName, *LevelInformation);
	}

	return true;
}


void AGameModeSaveLogic::InitOwnerSavePlatformId()
{
	SavePlatformId = FGenericPlatformMisc::GetLoginId();
}


FString AGameModeSaveLogic::GetSavePlatformId() const
{
	return SavePlatformId;
}

USaved_Level* AGameModeSaveLogic::GetLevelSaveInformation() const
{
	return CurrentLevelSave;
}


TArray<APlayerController*> AGameModeSaveLogic::GetPlayers() const
{
	TArray<APlayerController*> Players;
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			Players.Add(PlayerController);
		}
	}

	return Players;
}


void AGameModeSaveLogic::PrintMessage(const FString& Message)
{
	for(APlayerController* PlayerController : GetPlayers())
	{
		UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2}({3}), id: {4} ->  {5}", *UEnum::GetValueAsString(PlayerController->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(PlayerController), GetName(), GetUniqueID(), Message);
	}
}


void AGameModeSaveLogic::PrintActorsInLevel(bool bSavedActors)
{
	// Update actors in level with saved state information
	TArray<AActor*> LevelActors;
	TArray<ILevelSaveInformationInterface*> SavedActors;
	TArray<AActor*> Actors = GetLevel()->Actors;
	UE_LOGFMT(LogTemp, Log, " ");
	UE_LOGFMT(LogTemp, Log, "//--------------------------------------------------------------------------------------------//");
	UE_LOGFMT(LogTemp, Log, "// Printing the actors of the level");
	UE_LOGFMT(LogTemp, Log, "//--------------------------------------------------------------------------------------------//");
	for (AActor* Actor : Actors)
	{
		ILevelSaveInformationInterface* SavedActor = Cast<ILevelSaveInformationInterface>(Actor);
		if (SavedActor) SavedActors.Add(SavedActor);
		else LevelActors.Add(Actor);
	}

	UE_LOGFMT(LogTemp, Log, " ");
	UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
	UE_LOGFMT(LogTemp, Log, "// Saved actors list");
	UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
	for (ILevelSaveInformationInterface* SavedActor : SavedActors)
	{
		AActor* Actor = Cast<AActor>(SavedActor);
		if (!Actor) continue;

		UE_LOGFMT(LogTemp, Log, "// Id: {0}, Saved: {1}, uniqueId: {2}", *SavedActor->Execute_GetActorLevelId(Actor), *GetNameSafe(Actor), Actor->GetUniqueID());
	}
	UE_LOGFMT(LogTemp, Log, "//----------------------------------------//");

	if (bSavedActors)
	{
		UE_LOGFMT(LogTemp, Log, " ");
		UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
		UE_LOGFMT(LogTemp, Log, "// Level actors list");
		UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
		for (AActor* LevelActor : LevelActors)
		{
			UE_LOGFMT(LogTemp, Log, "// Level: {0}", *GetNameSafe(LevelActor));
		}
		UE_LOGFMT(LogTemp, Log, "//----------------------------------------//");
	}
	
}
#pragma endregion
