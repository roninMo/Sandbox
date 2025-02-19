// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSaveInformationInterface.h"

F_LevelSaveInformation_Actor ILevelSaveInformationInterface::SaveToLevel_Implementation()
{
	F_LevelSaveInformation_Actor SavedInformation;
	
	OnSaveToLevel(SavedInformation);
	return SavedInformation;
}


bool ILevelSaveInformationInterface::LoadFromLevel_Implementation(const F_LevelSaveInformation_Actor& PreviousSave)
{
	OnLoadFromLevel(PreviousSave, true);
	return true;
}


FGuid ILevelSaveInformationInterface::GetActorLevelId_Implementation() const
{
	return FGuid();
}
