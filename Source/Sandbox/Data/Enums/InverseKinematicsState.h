// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InverseKinematicsState.generated.h"


/**
 *	The current state of inverse kinematics logic
 */
UENUM(BlueprintType)
enum class EInverseKinematicsState : uint8
{
	IK_Enabled		                    UMETA(DisplayName = "Enabled"),
	IK_TransitionIn						UMETA(DisplayName = "Transition In"),
	IK_TransitionOut					UMETA(DisplayName = "Transition Out"),
	IK_Disabled		                    UMETA(DisplayName = "Disabled"),
	IK_None								UMETA(DisplayName = "None")
};


/**
 *	The current state of inverse kinematics for arms/hands (did you come here to mess with these hands?)
 */
UENUM(BlueprintType)
enum class EArmsInverseKinematics : uint8
{
	ActionEvents						UMETA(DisplayName = "Action Events"),
	WallPlacement		                UMETA(DisplayName = "Wall Placement"),
	WallRunning							UMETA(DisplayName = "Wall Running"),
	WallClimbing		                UMETA(DisplayName = "Wall Climbing"),
	LedgePlacement		                UMETA(DisplayName = "Ledge Placement"),
	None								UMETA(DisplayName = "None")
};


/**
 *  The rotation direction for inverse kinematic joints for handling twists
*/
UENUM(BlueprintType)
enum class EinverseKinematicRotations : uint8
{
	Up									UMETA(DisplayName = "Up"),
	Down								UMETA(DisplayName = "Down"),
	Clockwise							UMETA(DisplayName = "Clockwise"),
	CounterClockwise		            UMETA(DisplayName = "Counter Clockwise"),
	Left								UMETA(DisplayName = "Left"),
	Right								UMETA(DisplayName = "Right"),
	None								UMETA(DisplayName = "None")
};


/**
 *  The rotation orientation for inverse kinematic rotations
 */
UENUM(BlueprintType)
enum class ERotationOrientation : uint8
{
	RO_ForwardLeft		                UMETA(DisplayName = "Forward Right"),
	RO_ForwardRight						UMETA(DisplayName = "Forward Left"),
	RO_BackwardLeft		                UMETA(DisplayName = "Backward Right"),
	RO_BackwardRight					UMETA(DisplayName = "Backward Left"),
	
	RO_DownForwardLeft		                UMETA(DisplayName = "Down Forward Right"),
	RO_DownForwardRight						UMETA(DisplayName = "Down Forward Left"),
	RO_DownBackwardLeft		                UMETA(DisplayName = "Down Backward Right"),
	RO_DownBackwardRight					UMETA(DisplayName = "Down Backward Left"),
	
	RO_Neutral                          UMETA(DisplayName = "Neutral")
};
