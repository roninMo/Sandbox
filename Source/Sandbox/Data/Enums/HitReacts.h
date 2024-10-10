#pragma once


#include "CoreMinimal.h"
#include "HitReacts.generated.h"


/**
 *	The specific type of hitstun 
 */
UENUM(BlueprintType)
enum class EHitStun : uint8
{
	/** No hitstun | Poison Mist */
	None = 0			UMETA(DisplayName = "No Hitstun"),

  	/** Very short hitstun | Dagger R1 */
	VeryShort = 1			UMETA(DisplayName = "Very Short Hitstun"),
	
  	/** Short hitstun | Straight Sword R1 */
	Short = 2			UMETA(DisplayName = "Short Hitstun"),
	
  	/** Medium hitstun | Greatsword R1 */
	Medium = 3			UMETA(DisplayName = "Medium Hitstun"),
	
  	/** Long hitstun | Colossal Sword R1 */
	Long = 4			UMETA(DisplayName = "Long Hitstun"),
	
  	/** Knockdown | Ancient Lightning Spear */
	Knockdown = 5			UMETA(DisplayName = "Knockdown"),
	
  	/** Faceplant | Lion's Claw */
	FacePlant = 6			UMETA(DisplayName = "Faceplant"),
	
  	/** Burger Flip | Giant Hunt */
	FrontFlip = 6			UMETA(DisplayName = "FrontFlip"),

	MAX = 7
};

