// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementTypes.generated.h"


/** The custom movement modes */
UENUM(BlueprintType)
enum ECustomMovementMode
{
	MOVE_Custom_None				UMETA(DisplayName = "None"),
	MOVE_Custom_Slide				UMETA(DisplayName = "Slide"),
	MOVE_Custom_WallClimbing		UMETA(DisplayName = "Wall Climbing"),
	MOVE_Custom_Mantling			UMETA(DisplayName = "Mantling"),
	MOVE_Custom_LedgeClimbing		UMETA(DisplayName = "Ledge Climbing"),
	MOVE_Custom_WallRunning			UMETA(DisplayName = "Wall Running"),
	MOVE_Custom_MAX					UMETA(Hidden)
};


/** Whether the character (used for ai) is walking, jogging, or sprinting, etc. */
UENUM(BlueprintType)
enum class EMovementGait : uint8
{
	Gait_Walking						UMETA(DisplayName = "Walking"),
	Gait_Jogging						UMETA(DisplayName = "Jogging"),
	Gait_Sprinting						UMETA(DisplayName = "Sprinting"),
	Gait_None                           UMETA(DisplayName = "None")
};


/** The specific movement direction */
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	MD_Forward		                    UMETA(DisplayName = "Forward"),
	MD_Left								UMETA(DisplayName = "Left"),
	MD_Right		                    UMETA(DisplayName = "Right"),
	MD_Backward		                    UMETA(DisplayName = "Backward"),
	MD_ForwardLeft		                UMETA(DisplayName = "Forward Right"),
	MD_ForwardRight						UMETA(DisplayName = "Forward Left"),
	MD_BackwardLeft		                UMETA(DisplayName = "Backward Right"),
	MD_BackwardRight					UMETA(DisplayName = "Backward Left"),
	MD_Neutral                          UMETA(DisplayName = "Neutral")
};


/** The type of ledge climb the player is performing */
UENUM(BlueprintType)
enum class EClimbType : uint8
{
	Normal							UMETA(DisplayName = "Normal"),
	Fast							UMETA(DisplayName = "Fast"),
	Slow							UMETA(DisplayName = "Slow"),
	None							UMETA(DisplayName = "None")
};
