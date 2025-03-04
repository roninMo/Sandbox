// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameState.h"

#include "MultiplayerGameMode.h"

void AMultiplayerGameState::SaveGameState(const int32 Iteration)
{
	AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->SaveGame(Iteration);
	}
}
