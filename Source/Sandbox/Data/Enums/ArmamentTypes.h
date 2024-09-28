#pragma once


#include "CoreMinimal.h"
#include "ArmamentTypes.generated.h"


/**
 *	The stance or montages that the player should use based on the weapons he has equipped and how he's wielding them
 */
UENUM(BlueprintType)
enum class EArmamentStance : uint8
{
	/** None */
	EAS_None				UMETA(DisplayName = "None"),

	/** The player has a single aramment and should it's montages */
	EAS_OneHanded		    UMETA(DisplayName = "One Handed"),

	/** The player has two separate weapon types equipped */
	EAS_TwoWeapons			UMETA(DisplayName = "Two Weapons"),

	/** The player is dual wielding and needs to use the montages for dual wielding */
	EAS_DualWielding		UMETA(DisplayName = "Dual Wielding"),

	/** The player has an offhand that affects some of it's montages and should use both armament's montage */
	EAS_TwoHanded			UMETA(DisplayName = "Two Handed")
};


/**
 *	Is this attack/armament's damage based on combos, player attributes, or both?
 */
UENUM(BlueprintType)
enum class EDamageInformationSource : uint8
{
	/** None */
	None					UMETA(DisplayName = "None"),
	
	/** The attack and damage scales from the armament, and every attack of that combo is based on this */
	Armament    		    UMETA(DisplayName = "Armament"),
	
	/** Each combo attack has it's own attack information that's not specific to an armament. */
	Combo    		    UMETA(DisplayName = "Combo"),
	
	/** The damage is gathered from both the armament and the combo attack */
	Hybrid				UMETA(DisplayName = "Hybrid"),
};


/**
 *	Is this a primary, secondary, special, or alt attack?
 */
UENUM(BlueprintType)
enum class EComboType : uint8
{
	None				UMETA(DisplayName = "None"),
	Primary				UMETA(DisplayName = "Primary Attack"),
	Strong				UMETA(DisplayName = "Strong Attack"),
	Special				UMETA(DisplayName = "Special Attack"),
	Blocking			UMETA(DisplayName = "Block"),
	Parrying			UMETA(DisplayName = "Parry"),
	Crouching			UMETA(DisplayName = "Crouching Attack"),
	Running				UMETA(DisplayName = "Running Attack")
};


/**
 *	The specific type of weapon 
 */
UENUM(BlueprintType)
enum class EArmamentClassification : uint8
{
	None								UMETA(DisplayName = "None"),
	Unarmed								UMETA(DisplayName = "Unarmed"),
	StraightSwords						UMETA(DisplayName = "Straight Swords"),
	Katanas								UMETA(DisplayName = "Katanas"),
	Knifes								UMETA(DisplayName = "Knifes"),
	Axes								UMETA(DisplayName = "Axes"),
	Hammers								UMETA(DisplayName = "Hammers"),
	Scythes								UMETA(DisplayName = "Scythes"),
	Halberds							UMETA(DisplayName = "Halberds"),
	Spears								UMETA(DisplayName = "Spears"),
	Staffs								UMETA(DisplayName = "Staffs"),
	Shurikens							UMETA(DisplayName = "Shurikens"),
	Bows								UMETA(DisplayName = "Bows"),
	Crossbows							UMETA(DisplayName = "Crossbows"),
	Guns								UMETA(DisplayName = "Guns"),
	Shields								UMETA(DisplayName = "Shields"),
	Torches								UMETA(DisplayName = "Torches"),
	Tools								UMETA(DisplayName = "Tools"),
	MAX									UMETA(DisplayName = "MAX")
};


/**
 *	The armament equip type, or how the character should wield the armament
 */
UENUM(BlueprintType)
enum class EEquipRestrictions : uint8
{
	None								UMETA(DisplayName = "None"),
	Default								UMETA(DisplayName = "Default"),
	TwoHandOnly							UMETA(DisplayName = "Two Hand Only"),
	OneHandOnly							UMETA(DisplayName = "One Hand Only"),
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
