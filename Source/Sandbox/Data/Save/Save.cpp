// Fill out your copyright notice in the Description page of Project Settings.


#include "Save.h"


bool USave::IsValidSaveState() const
{
	if (PlatformId == FString()) return false;
	return true;
}


void USave::SaveInformation(FString Name, FString SaveDescription, int32 NetworkId, FString AccountPlatformId, int32 Slot, int32 Index)
{
	SaveName = Name;
	Description = SaveDescription;
	NetId = NetworkId;
	PlatformId = AccountPlatformId;
	SaveSlot = Slot;
	SaveIndex = Index;
	Timestamp = FDateTime::Now();
}
