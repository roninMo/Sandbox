// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AISenseTypes.generated.h"


/**
 *	The team of a specific player (this is to help ai characters identify friend or foe, so it goes over both single and multiplayer scenarios)
 */
UENUM(BlueprintType)
enum class ETeamId : uint8
{
	Team_Bandits			UMETA(DisplayName = "Bandits"),
	Team_Soldiers			UMETA(DisplayName = "Soldiers"),
	Team_Beasts				UMETA(DisplayName = "Beasts"),
	Team_Red				UMETA(DisplayName = "Red Team"),
	Team_Blue				UMETA(DisplayName = "Blue Team"),
	Team_Yellow				UMETA(DisplayName = "Yellow Team"),
	Team_Green				UMETA(DisplayName = "Green Team"),
	Team_None = 255			UMETA(DisplayName = "None") // @ref FGenericTeamId for the default value
};
