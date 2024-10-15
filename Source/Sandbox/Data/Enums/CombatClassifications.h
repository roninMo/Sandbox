#pragma once


#include "CoreMinimal.h"
#include "CombatClassifications.generated.h"


/**
 *	The classification of this enemy
 */
UENUM(BlueprintType)
enum class ECombatClassification : uint8
{
	None				UMETA(DisplayName = "None"),
	Fighter				UMETA(DisplayName = "Fighter"),
	Rouge				UMETA(DisplayName = "Rouge"),
	Mage				UMETA(DisplayName = "Mage"),
	Captain				UMETA(DisplayName = "Captain")
};
