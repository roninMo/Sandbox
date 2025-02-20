// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSaveInformationInterface.h"

F_LevelSaveInformation_Actor ILevelSaveInformationInterface::SaveToLevel_Implementation()
{
	return F_LevelSaveInformation_Actor();
}

bool ILevelSaveInformationInterface::SaveActorData_Implementation(const F_LevelSaveInformation_Actor& SaveConfig)
{
	return true;
}


bool ILevelSaveInformationInterface::LoadFromLevel_Implementation(const F_LevelSaveInformation_Actor& PreviousSave, bool bRetrieveActorSave)
{
	return true;
}


FString ILevelSaveInformationInterface::GetActorLevelId_Implementation() const
{
	return FString();
}
