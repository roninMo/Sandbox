// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameState.h"

#include "MultiplayerGameMode.h"

void AMultiplayerGameState::SaveGameState(const FString& SaveGameRef, const int32 Index)
{
	AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->SaveGame(SaveGameRef, Index);
	}
}
