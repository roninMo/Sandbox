// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Saved_CameraSettings.generated.h"

class ACharacterCameraLogic;
enum class ECameraOrientation : uint8;


/**
 * 
 */
UCLASS()
class SANDBOX_API USaved_CameraSettings : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName CameraStyle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ECameraOrientation CameraOrientation;


public:
	/** Retrieves the camera settings from the character */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading")
	virtual void SaveFromCameraCharacter(ACharacterCameraLogic* CameraCharacter);

	
};
