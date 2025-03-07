// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"


EGameModeType UMultiplayerGameInstance::GetGameModeType()
{
	return GameModeType;
}

USave* UMultiplayerGameInstance::GetCurrentSave()
{
	return CurrentSave;
}

USaved_Level* UMultiplayerGameInstance::GetCurrentLevelSave()
{
	return CurrentLevelSave;
}


void UMultiplayerGameInstance::SetGameModeType(EGameModeType Type)
{
	GameModeType = Type;
}

void UMultiplayerGameInstance::SetCurrentSave(USave* Save)
{
	CurrentSave = Save;
}

void UMultiplayerGameInstance::SetCurrentLevelSave(USaved_Level* LevelSave)
{
	CurrentLevelSave = LevelSave;
}

