#pragma once


#include "CoreMinimal.h"
#include "HudState.generated.h"


/**
 *	This is the specific ui element (settings, inventory, hud, scoreboard, etc.) The default hud is implemented so that the player has free movement while they have the hud open
 *	This also allows you to open inventory and settings while moving around (cursor movement won't be enabled though (it doesn't help that way)
 */
UENUM(BlueprintType)
enum class EHudState : uint8
{
	None                            UMETA(DisplayName = "None"),
	Hud		                        UMETA(DisplayName = "Hud"),
	Stats		                    UMETA(DisplayName = "Character Stats"),
	Inventory						UMETA(DisplayName = "Inventory"),
	Equipment						UMETA(DisplayName = "Equipment"),
	
	Pause							UMETA(DisplayName = "Pause"),
	Settings						UMETA(DisplayName = "Settings"),
	Controls						UMETA(DisplayName = "Controls"),
	System_Settings					UMETA(DisplayName = "System Settings")
};
