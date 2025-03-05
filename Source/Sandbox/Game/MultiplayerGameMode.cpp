// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

#include "GameModeLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Saving/Level/LevelSaveComponent.h"
#include "Sandbox/Characters/Player/BasePlayerState.h"
#include "Sandbox/Data/Enums/GameModeTypes.h"
#include "Sandbox/Data/Save/Save.h"
#include "Sandbox/World/Props/WorldItem.h"

DEFINE_LOG_CATEGORY(GameModeLog);



#pragma region Constructors
AMultiplayerGameMode::AMultiplayerGameMode(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	LevelSaveComponentClass = ULevelSaveComponent::StaticClass();

	// The game mode's classification
	GameModeType = EGameModeType::None;

	// Save information pertaining to each Game Mode
	CurrentSave = nullptr;
	SaveIndex = 0;
	SaveGameUrl = "";

	// Levels
	LobbyLevel = "/Game/Maps/Lobby/Lobby.umap";
	SingleplayerLevel = "/Game/Maps/BhopMap/Maps/UEDPIE_1_Demo.Demo";
}


void AMultiplayerGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	
	// Spawn the Level's save component
	if (LevelSaveComponentClass)
	{
		LevelSaveComponent = NewObject<ULevelSaveComponent>(this, LevelSaveComponentClass);
	}
	PrintMessage("PreInitializeComponents");
}


void AMultiplayerGameMode::Reset()
{
	Super::Reset();
	// InitGameState();
	PrintMessage("Reset");
}
#pragma endregion




#pragma region Match State Functions
void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PrintMessage("PostLogin");
	RetrieveSavePlatformId();
}


void AMultiplayerGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	PrintMessage("Handling MatchIsWaitingToStart");
}


void AMultiplayerGameMode::HandleMatchHasStarted()
{
	PrintActorsInLevel(false);

	// TODO: Once the level / GameMode has been initialized, search for the owner, and retrieve the save information
	UE_LOGFMT(GameModeLog, Warning, "{0}() Printing current players: ", *FString(__FUNCTION__));
	for (APlayerController* PlayerController : GetPlayers())
	{
		UE_LOGFMT(GameModeLog, Log, "Player: {0}", *GetNameSafe(PlayerController));
	}
	UE_LOGFMT(GameModeLog, Warning, " ");

	
	// Init the player's save information (Save components won't save / retrieve save information until the save references are ready
	for (APlayerController* PlayerController : GetPlayers())
	{
		ABasePlayerState* PlayerState = PlayerController->GetPlayerState<ABasePlayerState>();
		if (PlayerState)
		{
			PlayerState->SetSaveUrl(SaveGameUrl);
			PlayerState->SetSaveIndex(SaveIndex);
		}
	}
	
	// BeginPlay ->	(RestartPlayers, BuildLevel, NotifyBeginPlay)  -> -> InitAbilityActorInfo
	Super::HandleMatchHasStarted();

	

	// Print the level's reference, you'll probably have to create an object for levels
	ULevel* CurrentLevel = GetLevel();
	UE_LOGFMT(GameModeLog, Warning, "Level Information");
	if (CurrentLevel)
	{
		if (GetWorld()) UE_LOGFMT(GameModeLog, Log, "Map Name: {0}", *GetWorld()->GetMapName());
		UE_LOGFMT(GameModeLog, Log, "Level->GetName(): {0}", *CurrentLevel->GetName());
		UE_LOGFMT(GameModeLog, Log, "Level->GetPathName(): {0}", *CurrentLevel->GetPathName());
		UE_LOGFMT(GameModeLog, Log, "Level->GetFullName(): {0}", *CurrentLevel->GetFullName());
	}
	

	// Load the game's current save data. Level, Characters, and custom actor save data

	


	// Load the level's current save state to be replicated to clients that joined
	if (LevelSaveComponent)
	{
		LevelSaveComponent->LoadCurrentState(true);
	}

	ULevel* Level = GetLevel();
	if (!Level)
	{
		
	}
	


	// TODO: Figure out what to do for clients that are joining games during play
	// TODO: race condition with OnInitAbilityActorInfo and both adjust the settings / saved information of the character, we need to use ClientJoinSession for initializing server information at the beginning of games
	//			- until then just use OnInitAbilityActorInfo()
	PrintMessage("Handling MatchHasStarted");
}


void AMultiplayerGameMode::HandleMatchHasEnded()
{
	// Save level and character information
	if (LevelSaveComponent)
	{
		LevelSaveComponent->SaveCurrentState(true);
	}

	PrintMessage("Handling MatchHasEnded");

	Super::HandleMatchHasEnded();
}


void AMultiplayerGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
	PrintMessage("Handling LeavingMap");
}


void AMultiplayerGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();
	
	// Save level and character information
	if (LevelSaveComponent)
	{
		
	}

	PrintMessage("Handling MatchAborted");
}

void AMultiplayerGameMode::Travel(FString Map)
{
	if (!GetWorld()) return;
	// /Game/Maps/BhopMap/Maps/Demo.Demo
	
	GetWorld()->ServerTravel(Map);
}
#pragma endregion




#pragma region Save State Functions
bool AMultiplayerGameMode::SaveGame(const FString& SaveUrl, const int32 Index)
{
	return true;
}


bool AMultiplayerGameMode::LoadSave(const FString& SaveUrl, const int32 Index)
{
	return true;
}


bool AMultiplayerGameMode::SetCurrentSave(USave* Save)
{
	if (!Save)
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to update the game's save state, the save game provided was null", *FString(__FUNCTION__));
		return false;
	}
	
	CurrentSave = Save;
	SaveIndex = CurrentSave->SaveIndex;
	SaveGameUrl = ConstructSaveGameUrl(SavePlatformId, CurrentSave->SaveSlot, CurrentSave->SaveIndex);
	return true;
}


bool AMultiplayerGameMode::CreateNextSave()
{
	if (!CurrentSave) return false;
	int32 CurrentIndex = CurrentSave->SaveIndex;

	// Create a new save, and update the game state
	USave* Save = NewObject<USave>(this, USave::StaticClass());
	if (!Save)
	{
		return false;
	}
	
	Save->SaveInformation(
		CurrentSave->SaveName,
		CurrentSave->Description,
		CurrentSave->NetId,
		CurrentSave->PlatformId,
		CurrentSave->SaveSlot,
		CurrentIndex++
	);

	
	// Additional save state here
	

	
	// Update the game mode and player state to notify the clients to start saving to the next instance
	if (!SetCurrentSave(Save))
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to update the game's save index, an invalid save game has been provided", *FString(__FUNCTION__));
		return false;
	}

	// Player States
	for (APlayerController* PlayerController : GetPlayers())
	{
		ABasePlayerState* PlayerState = PlayerController->GetPlayerState<ABasePlayerState>();
		if (PlayerState)
		{
			PlayerState->SetSaveUrl(SaveGameUrl);
			PlayerState->SetSaveIndex(SaveIndex);
			PlayerState->SetSaveSlot(CurrentSave->SaveSlot);
		}
	}

	return true;
}


bool AMultiplayerGameMode::FindCurrentSave(const FString& AccountId, int32 SaveSlot, FString& OutSaveUrl, int32& OutSaveIndex) const
{
	// Construct the SaveGameId
	int32 Index = 0;
	FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, Index);
	
	// Check if there's save information for this slot
	if (!UGameplayStatics::DoesSaveGameExist(SaveUrl, Index))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() wasn't save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return false;
	}

	// Find the most recent save, and delete the saves // TODO: we should keep a reference for a specific character, and have everything else be handled naturally
	int32 SearchIndex = Index;
	int32 SearchStep = 100;
	bool bFoundCurrentSaveIndex = false;
	int32 bSaveSearchErrorCount = 0;
	
	FString LastValidSaveUrl = SaveUrl;
	int32 LastValidSaveIndex = 0;
	while (!bFoundCurrentSaveIndex || bSaveSearchErrorCount < 5)
	{
		SearchIndex = SearchIndex + SearchStep;
		SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, SearchIndex);

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
		OutSaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, SearchIndex);
		OutSaveIndex = SearchIndex;
	}
	return true;
}


TArray<FString> AMultiplayerGameMode::FindPreviousSaves(const FString& AccountId, int32 SaveSlot, int32 CurrentSaveIndex, int32 SavesToRetrieve) const
{
	TArray<FString> PreviousSaves;
	for (int32 Index = CurrentSaveIndex - 1; Index >= 0; Index--)
	{
		// Check if it's a valid save
		FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, Index);
		if (UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
		{
			PreviousSaves.AddUnique(SaveUrl);
		}

		if (PreviousSaves.Num() >= SavesToRetrieve)
		{
			return PreviousSaves; 
		}
	}

	return PreviousSaves;
}


bool AMultiplayerGameMode::DeleteSaveSlot(const FString& AccountId, int32 SaveSlot)
{
	// Construct the SaveGameId
	int32 Index = 0;
	FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, Index);

	// Check if there's save information for this slot
	if (!UGameplayStatics::DoesSaveGameExist(SaveUrl, Index))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() wasn't save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return true;
	}

	// Find the most recent save, and delete the saves
	FString CurrentSaveUrl;
	int32 CurrentSaveIndex = 0;
	if (!FindCurrentSave(AccountId, SaveSlot, CurrentSaveUrl, CurrentSaveIndex))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() couldn't find the current save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return true;
	}

	// If that didn't break anything, let's just delete all the previous saves
	for (int32 i = CurrentSaveIndex; CurrentSaveIndex > 0; i--)
	{
		FString SaveToDelete = ConstructSaveGameUrl(AccountId, SaveSlot, i);
		UGameplayStatics::DeleteGameInSlot(SaveToDelete, 0);
	}
	
	return true;
}


USave* AMultiplayerGameMode::NewSaveSlot(const FString& AccountId, int32 SaveSlot)
{
	// Construct the SaveGameId
	FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, 0);

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
	}

	return Save;
}


FString AMultiplayerGameMode::ConstructSaveGameUrl(const FString& PlatformId, int32 Slot, int32 Index) const
{
	// (GameMode + PlatformId + SlotId + SaveIndex ++ additional SaveComponent logic for each actor)
	// SaveUrl -> Adventure_Character1_S1_54, MP_CustomLevel1, etc.
	// Adventure_Character1_S1_54 + _Character1 + _SaveComponents
	// Adventure_Character1_S1_54 + _Character2 + _SaveComponents
	// Adventure_Character1_S1_54_Level + _Prop1
	// Adventure_Character1_S1_54_Level + _Actor0
	
	return UGameModeLibrary::GameModeTypeToString(GameModeType)
		.Append("_")
		.Append(PlatformId) // TODO: Dedicated multiplayer servers need logic for retrieving the host, or separate functionality for multiplayer
		.Append("_")
		.Append("S").Append(FString::FromInt(Slot))
		.Append("_")
		.Append(FString::FromInt(Index));
}


USave* AMultiplayerGameMode::GetSaveGameBase()
{
	return CurrentSave;
}


void AMultiplayerGameMode::SetSaveGameBase(USave* Save)
{
	CurrentSave = Save;
}


FString AMultiplayerGameMode::GetSavePlatformId() const
{
	return SavePlatformId;
}


void AMultiplayerGameMode::RetrieveSavePlatformId()
{
	SavePlatformId = FGenericPlatformMisc::GetLoginId();
}
#pragma endregion




#pragma region Respawning
void AMultiplayerGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}


void AMultiplayerGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}
#pragma endregion




#pragma region Utility
EGameModeType AMultiplayerGameMode::GetGameModeType()
{
	return GameModeType;
}


TArray<APlayerController*> AMultiplayerGameMode::GetPlayers() const
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


void AMultiplayerGameMode::PrintMessage(const FString& Message)
{
	for(APlayerController* PlayerController : GetPlayers())
	{
		UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2}({3}) ->  {4}", *UEnum::GetValueAsString(PlayerController->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(PlayerController), GetName(), Message);
	}
}


void AMultiplayerGameMode::PrintActorsInLevel(bool bSavedAndLevelActors)
{
	UWorld* World = GetWorld();

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

	if (bSavedAndLevelActors)
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