// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCameraTypes.generated.h"


/** The different camera styles */
#define CameraStyle_None FName("None")
#define CameraStyle_Fixed FName("Fixed")
#define CameraStyle_Spectator FName("Spectator")
#define CameraStyle_FirstPerson FName("FirstPerson")
#define CameraStyle_ThirdPerson FName("ThirdPerson")
#define CameraStyle_TargetLocking FName("TargetLocking")
#define CameraStyle_Aiming FName("Aiming")




/**
*	The third person orientation of the camera
*	This is used to determine what pivot location on the skeletal mesh is used for the camera position
*/
UENUM(BlueprintType, Category = "Camera")
enum class ECameraOrientation : uint8
{
	None						UMETA(DisplayName = "None"),
	Center						UMETA(DisplayName = "Center"),
	LeftShoulder				UMETA(DisplayName = "Left Shoulder"),
	RightShoulder				UMETA(DisplayName = "Right Shoulder"),
	Custom						UMETA(DisplayName = "Custom")
};




/**
*	What direction is the transition from the previous target to the current target? Is it from the left side? 
*/
UENUM(BlueprintType, Category = "Camera")
enum class EPreviousTargetLockOrientation : uint8
{
	None						UMETA(DisplayName = "None"),
	Right						UMETA(DisplayName = "Right"),
	Left						UMETA(DisplayName = "Left"),
	Center						UMETA(DisplayName = "Center"),
	Back						UMETA(DisplayName = "Back"),
	Front						UMETA(DisplayName = "Front"),
};




/*
* Target lock information for quickly finding how close a target is to the player, and how far to one side they are
*/
USTRUCT(BlueprintType, Category = "Camera")
struct FTargetLockInformation
{
	GENERATED_USTRUCT_BODY()
		FTargetLockInformation(
			AActor* Target = nullptr,
			const float DistanceToTarget = 0.0,
			const float AngleFromForwardVector = 0.0
		) :
    
		Target(Target),
		DistanceToTarget(DistanceToTarget),
		AngleFromForwardVector(AngleFromForwardVector)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")                     AActor* Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")                     float DistanceToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")                     float AngleFromForwardVector;
    
};
