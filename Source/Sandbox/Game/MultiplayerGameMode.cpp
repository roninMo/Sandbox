// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Saving/Level/LevelSaveComponent.h"
#include "Sandbox/Data/Enums/GameModeTypes.h"
#include "Sandbox/World/Props/WorldItem.h"

DEFINE_LOG_CATEGORY(GameModeLog);



#pragma region Constructors
AMultiplayerGameMode::AMultiplayerGameMode(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	LevelSaveComponentClass = ULevelSaveComponent::StaticClass();

	// The game mode's classification
	GameModeType = EGameModeType::None;
}


void AMultiplayerGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	
	// Spawn the Level's save component
	if (LevelSaveComponentClass)
	{
		LevelSaveComponent = NewObject<ULevelSaveComponent>(this, LevelSaveComponentClass);
	}
}
#pragma endregion



#pragma region Save State Functions
bool AMultiplayerGameMode::SaveGame(const int32 Iteration)
{
	if (!LevelSaveComponent) return false;

	
	return true;
}


bool AMultiplayerGameMode::LoadSave(const int32 Iteration)
{
	return true;
}


int32 AMultiplayerGameMode::FindSaveIteration() const
{
	if (!LevelSaveComponent) return 0;

	// LevelSaveComponent->GetSaveGameRef()
	// // Retrieve from current save configuration -> or search through previous saves. Saving should be specific to the game state, and everything should save / retrieve their information from each save.
	// // TODO: Check that when the player saves it's handled properly when the level saves, and add logic to retrieve the proper save then
	//
	//
	// // The game hasn't been saved yet
	// FString SaveSlot = ConstructSaveSlot(GetNetId(), GetPlatformId(), "Config", GetSaveSlotIndex(), 0);
	// if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SplitScreenIndex))
	// {
	// 	return 0;
	// }
	//
	// // If there is already a save
	// // Check if the current save iteration is valid, otherwise start from the beginning
	// int32 SaveIteration = GetSaveIteration();
	// SaveSlot = ConstructSaveSlot(GetNetId(), GetPlatformId(), "Config", GetSaveSlotIndex(), SaveIteration);
	// if (!UGameplayStatics::DoesSaveGameExist(SaveSlot, SplitScreenIndex))
	// {
	// 	SaveIteration = 0;
	// }
	//
	// bool bValidSaveIteration = true;
	// while (bValidSaveIteration)
	// {
	// 	SaveIteration++;
	// 	SaveSlot = ConstructSaveSlot(GetNetId(), GetPlatformId(), "Config", GetSaveSlotIndex(), SaveIteration);
	// 	if (!UGameplayStatics::DoesSaveGameExist(SaveSlot, SplitScreenIndex)) bValidSaveIteration = false;
	// }
	//
	// return SaveIteration;
	return 0;
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
	Super::HandleMatchHasStarted(); // BeginPlay

	// TODO: With the understanding that the level saves and creates it's objects in the list and creates an identifier based on the object and the order it's created (on client or server)
	//				while not creating internal logic that persists between when it's registered and removed, we're just going to trust that the save system doesn't sway from that until other problems occur
	//				going through the engine code to edit how it's packaged would edit the engine / world code and we're not doing that right now, since it's all hoisted on separate production deployments and stages

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


void AMultiplayerGameMode::PrintMessage(const FString& Message)
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		if (const APlayerController* PlayerController = Iterator->Get())
		{
			UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2} ->  {3}", *UEnum::GetValueAsString(PlayerController->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(PlayerController), Message);
		}
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