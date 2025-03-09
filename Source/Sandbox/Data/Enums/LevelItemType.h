// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelItemType.generated.h"


/**
 *	The specific type of item spawned in a custom level. Used for classification and distribution
 */
UENUM(BlueprintType)
enum class ELevelItemType : uint8
{
	None								UMETA(DisplayName = "None"),
	Prop								UMETA(DisplayName = "Prop"),
	FabricatedProp						UMETA(DisplayName = "Fabricated Prop"),
	Battlement							UMETA(DisplayName = "Battlement"),
	Enemy								UMETA(DisplayName = "Enemy"),
	Armor								UMETA(DisplayName = "Armor"),
	Weapons								UMETA(DisplayName = "Weapons"),
	Equipment							UMETA(DisplayName = "Equipment"),
	Vehicle								UMETA(DisplayName = "Vehicle"),
	GameMode							UMETA(DisplayName = "GameMode"), // GameMode / Spawn locations / etc.
	FX									UMETA(DisplayName = "Prop"), // visual fx / post process volume / etc.
	MAX
};

