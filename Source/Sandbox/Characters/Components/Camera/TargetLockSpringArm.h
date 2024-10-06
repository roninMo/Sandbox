// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sandbox/Data/Enums/PlayerCameraTypes.h"
#include "TargetLockSpringArm.generated.h"

class ACharacterCameraLogic;


/**
 * 
 */
UCLASS()
class SANDBOX_API UTargetLockSpringArm : public USpringArmComponent
{
	GENERATED_BODY()

protected:
	/** At the beginning of target locking, or when transitioning to another target, allow the camera to smoothly interp */
	UPROPERTY(BlueprintReadWrite, Category="Target Locking") bool bTargetTransition;
	
	/** The current target lock character, derived from @ref ACharacterCameraLogic's target lock logic */
	UPROPERTY(BlueprintReadWrite, Category="Target Locking") TObjectPtr<AActor> CurrentTarget;

	/** A stored reference to the character */
	UPROPERTY(BlueprintReadWrite, Category="Target Locking") TObjectPtr<ACharacterCameraLogic> Character;

	
protected:
	/** Updates the desired arm location, calling BlendLocations to do the actual blending if a trace is done */
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	
	
};
