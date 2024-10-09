#pragma once		


#include "CoreMinimal.h"
#include "AttackFramesState.generated.h"


/**
 *	The current state of attack frames for a specific weapon. This helps with finding out whether it's already attacked and handling state for multiple attacks
 */
UENUM(BlueprintType)
enum class EAttackFramesState : uint8
{
	/** None */
	None					UMETA(DisplayName = "None"),

	/** Attack frames haven't been activated for this attack yet */
	Disabled				UMETA(DisplayName = "Disabled"),
	
	/** There's currently attack frames */
	Enabled					UMETA(DisplayName = "Enabled"),

	/** Attack frames have already happened for this weapon */
	Finished				UMETA(DisplayName = "Finished")
};

