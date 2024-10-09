#pragma once


#include "CoreMinimal.h"
#include "HitDirection.generated.h"


/**
 *	The armament equip type, or how the character should wield the armament
 */
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	None			UMETA(DisplayName = "None"),
	Left 			UMETA(DisplayName = "Left"),
	Front 			UMETA(DisplayName = "Front"),
	Right			UMETA(DisplayName = "Right"),
	Back			UMETA(DisplayName = "Back")
};
