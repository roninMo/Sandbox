#pragma once


#include "CoreMinimal.h"
#include "ArmorTypes.generated.h"


/**
 *	The specific type of armor 
 */
UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	None						UMETA(DisplayName = "None"),
	Helm						UMETA(DisplayName = "Helm"),
	Chest						UMETA(DisplayName = "Chest"),
	Gauntlets					UMETA(DisplayName = "Gauntlets"),
	Leggings					UMETA(DisplayName = "Leggings"),
};
