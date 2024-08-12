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
