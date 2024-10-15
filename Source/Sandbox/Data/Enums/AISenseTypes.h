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
	None = 255						UMETA(DisplayName = "None"), // @ref FGenericTeamId for the default value
	Soldiers = 0					UMETA(DisplayName = "Soldiers"),
	Beasts = 1						UMETA(DisplayName = "Beasts"),
	Bandits = 2						UMETA(DisplayName = "Bandits"),
	Red = 3							UMETA(DisplayName = "Red Team"),
	Blue = 4						UMETA(DisplayName = "Blue Team"),
	Yellow = 5						UMETA(DisplayName = "Yellow Team"),
	Green = 6						UMETA(DisplayName = "Green Team"),
};
