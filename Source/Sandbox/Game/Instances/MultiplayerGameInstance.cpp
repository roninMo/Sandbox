// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"


USave* UMultiplayerGameInstance::GetCurrentSave()
{
	return CurrentSave;
}

void UMultiplayerGameInstance::SetCurrentSave(USave* Save)
{
	CurrentSave = Save;
}
