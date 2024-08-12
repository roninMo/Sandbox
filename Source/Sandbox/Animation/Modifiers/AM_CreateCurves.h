// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Sandbox/Data/Structs/AnimModiferInformation.h"
#include "AM_CreateCurves.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UAM_CreateCurves : public UAnimationModifier
{
	GENERATED_BODY()

protected:
	/** The curve values to create */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<F_AnimationCurveInfo> Curves;


protected:
	UAM_CreateCurves(const FObjectInitializer& ObjectInitializer);
	
	/** Executed when the Animation is initialized (native event for debugging / testing purposes) */
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;

	
};
