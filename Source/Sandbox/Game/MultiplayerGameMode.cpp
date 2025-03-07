// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Save/Save.h"


AMultiplayerGameMode::AMultiplayerGameMode(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}


void AMultiplayerGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	PrintMessage("PreInitializeComponents");
}


void AMultiplayerGameMode::Reset()
{
	Super::Reset();
	// InitGameState();
	PrintMessage("Reset");
}




void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PrintMessage("PostLogin");
	
	// TODO: Once the level / GameMode has been initialized, search for the owner, and retrieve the save information
	InitOwnerSavePlatformId();
	
	// Init the player's save information (Save components won't save / retrieve save information until the save references are ready
	if (CurrentSave)
	{
		UpdatePlayerWithSaveReferences(NewPlayer);
	}
}


void AMultiplayerGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	PrintMessage("Logout");

	// Save player information // TODO: check if the character components are still valid here
	ACharacterBase* Character = Cast<ACharacterBase>(Exiting->GetPawn());
	if (CurrentSave && Character && Cast<APlayerController>(Exiting))
	{
		FString SavePlayerUrl = ConstructPlayerSaveUrl(GetCurrentSaveUrl(), Character->Execute_GetActorLevelId(Character));
		SavePlayer(Cast<APlayerController>(Exiting), SavePlayerUrl, CurrentSave->SaveIndex);
	}
}


void AMultiplayerGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	PrintMessage("Handling MatchIsWaitingToStart");
}


void AMultiplayerGameMode::HandleMatchHasStarted()
{
	// BeginPlay ->	(RestartPlayers, BuildLevel, NotifyBeginPlay)  -> -> InitAbilityActorInfo
	Super::HandleMatchHasStarted();

	// Load the game's current save data. Level, Characters, and custom actor save data for client replication
	PrintActorsInLevel(false);

	// Custom level save logic
	ULevel* Level = GetLevel();
	if (Level)
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
	if (CurrentSave)
	{
		SaveGame(GetCurrentSaveUrl(), CurrentSave->SaveIndex);
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
	if (CurrentSave)
	{
		SaveGame(GetCurrentSaveUrl(), CurrentSave->SaveIndex);
	}

	PrintMessage("Handling MatchAborted");
}


bool AMultiplayerGameMode::Travel(FString Map)
{
	if (!GetWorld()) return false;
	return GetWorld()->ServerTravel(Map);
}


void AMultiplayerGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}


void AMultiplayerGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}
