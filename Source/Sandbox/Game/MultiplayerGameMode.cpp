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
}


void AMultiplayerGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	PrintMessage("Handling MatchIsWaitingToStart");
}


void AMultiplayerGameMode::HandleMatchHasStarted()
{
	PrintActorsInLevel(false);

	// BeginPlay ->	(RestartPlayers, BuildLevel, NotifyBeginPlay)
	Super::HandleMatchHasStarted();

	// Probably in another function -> once the level / GameMode has been initialized, search for the owner, and retrieve the save information
	UE_LOGFMT(GameModeLog, Warning, "{0}() Printing current players: ", *FString(__FUNCTION__));
	for (APlayerController* PlayerController : GetPlayers())
	{
		UE_LOGFMT(GameModeLog, Log, "{0}() Player: {1}", *FString(__FUNCTION__), *GetNameSafe(PlayerController));
	}
	UE_LOGFMT(GameModeLog, Warning, " ");

	
	// Init the player's save information
	for (APlayerController* PlayerController : GetPlayers())
	{
		ABasePlayerState* PlayerState = PlayerController->GetPlayerState<ABasePlayerState>();
		if (PlayerState)
		{
			PlayerState->SetSaveGameRef(SaveGameUrl);
			PlayerState->SetSaveIndex(SaveIndex);
		}
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
#pragma endregion




#pragma region Save State Functions
bool AMultiplayerGameMode::SaveGame(const FString& SaveGameRef, const int32 Index)
{
	return true;
}


bool AMultiplayerGameMode::LoadSave(const FString& SaveGameRef, const int32 Index)
{
	return true;
}


bool AMultiplayerGameMode::UpdateCurrentSave(USave* Save)
{
	if (!Save)
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to update the game's save state, the save game provided was null", *FString(__FUNCTION__));
		return false;
	}
	
	// (GameMode + PlatformId + SlotId + SaveIndex ++ additional SaveComponent logic for each actor)
	// SaveGameRef -> Adventure_Character1_S1_54, MP_CustomLevel1, etc.
	// Adventure_Character1_S1_54 + _Character1 + _SaveComponents
	// Adventure_Character1_S1_54 + _Character2 + _SaveComponents
	// Adventure_Character1_S1_54_Level + _Prop1
	// Adventure_Character1_S1_54_Level + _Actor0
	CurrentSave = Save;
	SaveIndex = CurrentSave->SaveIndex;
	SaveGameUrl = UGameModeLibrary::GameModeTypeToString(GameModeType)
		.Append("_")
		.Append("Host of Lobby") // TODO: Dedicated multiplayer servers need logic for retrieving the host, or separate functionality for multiplayer
		.Append("_")
		.Append("S").Append(FString::FromInt(CurrentSave->SaveSlot))
		.Append("_")
		.Append(FString::FromInt(CurrentSave->SaveIndex));


	return true;
}


bool AMultiplayerGameMode::FindCurrentSave() const
{
	if (!CurrentSave) return false;
	
	// (GameMode + PlatformId + SlotId + SaveIndex ++ additional SaveComponent logic for each actor)
	// The lobby should be able to create / find one based on the character's individual saves

	// Character
	// Save Slots 1-4
	// -> Search for the current save index

	
	/*

		// The game hasn't been saved yet
		FString SaveSlot = ConstructSaveSlot(GetNetId(), GetPlatformId(), "Config", GetSaveSlotIndex(), 0);
		if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SplitScreenIndex))
		{
			return 0;
		}
		
		// If there is already a save, check if the current save iteration is valid, otherwise start from the beginning
		int32 SaveIndex = GetSaveIndex();
		bool bValidSaveIndex = true;
		while (bValidSaveIndex)
		{
			SaveIndex++;
			SaveSlot = ConstructSaveSlot(GetNetId(), GetPlatformId(), "Config", GetSaveSlotIndex(), SaveIndex);
			if (!UGameplayStatics::DoesSaveGameExist(SaveSlot, SplitScreenIndex)) bValidSaveIndex = false;
		}
		
		return SaveIndex;

	*/
	
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
		CurrentSave->Description, // Description of current game objective or something
		CurrentSave->NetId,
		CurrentSave->PlatformId,
		CurrentSave->SaveSlot,
		CurrentIndex++
	);

	// Additional save state here
	

	// Update the game mode and player state to notify the clients to start saving to the next instance
	if (!UpdateCurrentSave(Save))
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
			PlayerState->SetSaveGameRef(SaveGameUrl);
			PlayerState->SetSaveIndex(SaveIndex);
			PlayerState->SetSaveSlot(CurrentSave->SaveSlot);
		}
	}

	return true;
}



USave* AMultiplayerGameMode::GetSaveGameBase()
{
	return CurrentSave;
}


void AMultiplayerGameMode::SetSaveGameBase(USave* Save)
{
	CurrentSave = Save;
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
		UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2} ->  {3}", *UEnum::GetValueAsString(PlayerController->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(PlayerController), Message);
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