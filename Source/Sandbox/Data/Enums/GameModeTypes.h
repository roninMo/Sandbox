// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeTypes.generated.h"


/**
 *	The different genres of game modes. Used for classification, saving, and other gameplay logic
 */
UENUM(BlueprintType)
enum class EGameModeType : uint8
{
	None					UMETA(DisplayName = "None"),
	Adventure				UMETA(DisplayName = "We're going on an adventure today"),
	FoF						UMETA(DisplayName = "Free for All"),
	Kotk					UMETA(DisplayName = "King of the Hill"),
	SoD						UMETA(DisplayName = "Search and Destroy"),
	TDM						UMETA(DisplayName = "Team Deathmatch")
};
