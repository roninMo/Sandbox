// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameConfig.h"


void USaveGameConfig::SaveGameConfig(FString& Name, FString& SaveDescription, int32 NetworkId, FString& AccountPlatformId, int32 Slot, int32 Iteration)
{
	SaveName = Name;
	Description = SaveDescription;
	NetId = NetworkId;
	PlatformId = AccountPlatformId;
	SaveSlot = Slot;
	SaveIteration = Iteration;
}
