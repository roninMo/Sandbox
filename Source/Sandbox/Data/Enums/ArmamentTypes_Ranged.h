#pragma once


#include "CoreMinimal.h"
#include "ArmamentTypes_Ranged.generated.h"


/**
 *	The way we're calculating ranged attacks. Whether it's with projectiles, line traces, or another way
 */
UENUM(BlueprintType)
enum class EBulletType : uint8
{
	EB_None                             UMETA(DisplayName = "None"),
	EB_Projectile						UMETA(DisplayName = "Projectile"),
	EB_Trace							UMETA(DisplayName = "Trace")
};


/**
 *	The different types of ammo for a specific weapon
 */
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	AT_None                             UMETA(DisplayName = "None"),
	AT_Normal							UMETA(DisplayName = "Trace")
};


/**
 *	The weapon's fire rate types
 */
UENUM(BlueprintType)
enum class EFireRateType : uint8
{
	FR_None					UMETA(DisplayName = "None"),
	FR_SemiAuto		        UMETA(DisplayName = "Semi Automatic"),
	FR_Bolt                 UMETA(DisplayName = "Bolt Action"),
	FR_Burst                UMETA(DisplayName = "Burst Fire"),
	FR_Auto    				UMETA(DisplayName = "Automatic")
};


/**
 *	The different attachments for weapons
 */
UENUM(BlueprintType)
enum class EAttachment : uint8
{
	AA_None					UMETA(DisplayName = "None"),
	AA_Stock				UMETA(DisplayName = "Stock"),
	AA_Muzzle				UMETA(DisplayName = "Muzzle"),
	AA_Barrel				UMETA(DisplayName = "Barrel"),
	AA_Clip					UMETA(DisplayName = "Clip"),
	AA_Sights				UMETA(DisplayName = "Sights"),
	AA_UnderBarrel			UMETA(DisplayName = "Under Barrel")
};
