#pragma once


#include "CoreMinimal.h"
#include "QuestTypes.generated.h"


/**
 *	The current status of the objective
 */
UENUM(BlueprintType)
enum class EObjectiveStatus : uint8
{
	/** There are other objectives that haven't been completed yet */
	Locked				UMETA(DisplayName = "Locked"),

	/** This is one of the current objectives in progress for the quest */
	InProgress			UMETA(DisplayName = "In Progress"),

	/** The objective has been disabled */
	Disabled			UMETA(DisplayName = "Disabled"),

	/** The objective has been completed */
	Completed			UMETA(DisplayName = "Completed"),

	/** The objective has been failed */
	Failed				UMETA(DisplayName = "Failed"),
	
	MAX					UMETA(DisplayName = "MAX")
};




/**
 *	The current status of the quest
 */
UENUM(BlueprintType)
enum class EQuestState : uint8
{
	/** The quest hasn't been unlocked yet */
	Locked				UMETA(DisplayName = "Locked"),

	/** The unlocking requirements are met, but it is not in progress yet */
	Unlocked			UMETA(DisplayName = "Unlocked"),	

	/** The quest is in progress */
	InProgress			UMETA(DisplayName = "In Progress"),

	/** The player has already completed the quest */
	Completed			UMETA(DisplayName = "Completed"),

	MAX					UMETA(DisplayName = "MAX")
};

