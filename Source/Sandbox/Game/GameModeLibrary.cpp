// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeLibrary.h"

#include "Sandbox/Data/Enums/GameModeTypes.h"


FString UGameModeLibrary::GameModeTypeToString(const EGameModeType GameModeType)
{
	if (GameModeType == EGameModeType::Adventure) return "Adventure";
	if (GameModeType == EGameModeType::FoF) return "FoF";
	if (GameModeType == EGameModeType::Kotk) return "Kotk";
	if (GameModeType == EGameModeType::SoD) return "SoD";
	if (GameModeType == EGameModeType::TDM) return "TDM";
	return "None";
}
