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
 *	The current state of inverse kinematics for feet
 */
UENUM(BlueprintType)
enum class EFootInverseKinematics : uint8
{
	FootPlacement		                UMETA(DisplayName = "Foot Placement"),
	FootLocking		                    UMETA(DisplayName = "Foot Locking"),
	WallRunning		                    UMETA(DisplayName = "Wall Running"),
	WallClimbing		                UMETA(DisplayName = "Wall Climbing"),
	Landing								UMETA(DisplayName = "Landing"),
	None								UMETA(DisplayName = "None")
};


/**
 *	The current state of inverse kinematics for arms/hands (did you come here to mess with these hands?)
 */
UENUM(BlueprintType)
enum class EArmsInverseKinematics : uint8
{
	WallRunning							UMETA(DisplayName = "Wall Running"),
	WallClimbing		                UMETA(DisplayName = "Wall Climbing (Ledge placement)"),
	WallPlacement		                UMETA(DisplayName = "Wall Placement"),
	None								UMETA(DisplayName = "None")
};
