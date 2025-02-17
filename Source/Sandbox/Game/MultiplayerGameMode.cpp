// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(GameModeLog);


void AMultiplayerGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}


void AMultiplayerGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}


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
	/* // Initialize the save component before begin play
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		if (const APlayerController* PlayerController = Iterator->Get())
		{
			ACharacterBase* Character = Cast<ACharacterBase>(PlayerController->GetCharacter());
			if (!Character || !Character->GetSaveComponent()) continue;

			// Initialize the save logic and retrieve the saved information on the server for replication
			Character->GetSaveComponent()->InitializeSavingLogic();
		}
	}
	*/
	
	Super::HandleMatchHasStarted();

	// TODO: race condition with OnInitAbilityActorInfo and both adjust the settings / saved information of the character, we need to use ClientJoinSession for initializing server information at the beginning of games
	//			- until then just use OnInitAbilityActorInfo()
	PrintMessage("Handling MatchHasStarted");
}


void AMultiplayerGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	PrintMessage("Handling MatchHasEnded");
}


void AMultiplayerGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
	PrintMessage("Handling LeavingMap");
}


void AMultiplayerGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();
	PrintMessage("Handling MatchAborted");
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
