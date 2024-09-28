#pragma once


#include "CoreMinimal.h"
#include "ArmamentTypes.generated.h"

/**
 *	The armament equip type, or how the character should wield the armament
 */
UENUM(BlueprintType)
enum class EArmamentEquipType : uint8
{
	WT_Default								UMETA(DisplayName = "Default"),
	WT_TwoHandOnly							UMETA(DisplayName = "Two Hand Only"),
	WT_OneHandOnly							UMETA(DisplayName = "One Hand Only"),
	WT_None									UMETA(DisplayName = "None")
};


/**
 *	The specific type of weapon 
 */
// UENUM(BlueprintType)
// enum class EArmamentType : uint8
// {
// 	AT_Unarmed								UMETA(DisplayName = "Unarmed"),
// 	
// 	AT_Daggers								UMETA(DisplayName = "Daggers"),
// 	AT_StraightSwords						UMETA(DisplayName = "Straight Swords"),
// 	AT_Katanas								UMETA(DisplayName = "Katanas"),
// 	AT_Axes									UMETA(DisplayName = "Axes"),
// 	AT_Hammers								UMETA(DisplayName = "Hammers"),
// 	AT_Scythes								UMETA(DisplayName = "Scythes"),
// 	AT_Halberds								UMETA(DisplayName = "Halberds"),
// 	AT_Spears								UMETA(DisplayName = "Spears"),
// 	AT_Staffs								UMETA(DisplayName = "Staffs"),
// 	AT_Shurikens							UMETA(DisplayName = "Shurikens"),
// 	AT_Bows									UMETA(DisplayName = "Bows"),
// 	AT_Crossbows							UMETA(DisplayName = "Crossbows"),
// 	AT_Guns									UMETA(DisplayName = "Guns"),
// 	AT_Shields								UMETA(DisplayName = "Shields"),
// 	AT_Torches								UMETA(DisplayName = "Torches"),
// 	AT_Tools								UMETA(DisplayName = "Tools"),
// 	AT_None									UMETA(DisplayName = "None"),
// };
