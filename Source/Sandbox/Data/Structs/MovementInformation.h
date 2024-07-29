// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Enums/MovementTypes.h"
#include "MovementInformation.generated.h"


/** The information to smoothly perform a ledge climb. You'll probably want to refactor this to adjust things for how you handle different logic */
USTRUCT(BlueprintType)
struct FLedgeClimbInformation
{
	GENERATED_USTRUCT_BODY()
		FLedgeClimbInformation(
			const EClimbType LedgeClimbType = EClimbType::None,
			UCurveFloat* SpeedAdjustments = nullptr,
			const float InterpSpeed = 0
		) :

		SpeedAdjustments(SpeedAdjustments),
		InterpSpeed(InterpSpeed)
	{}

public:
	/** The ledge climb variation that this information stores */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EClimbType LedgeClimbType;
	
	/** The ledge climb curve for handling interping smoothly while handling any montage without lots of complications (this is kind of hacky, but it's better than the majority of other fixes for this) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UCurveFloat* SpeedAdjustments;

	/** The interp speed is used as a multiplier after everything else is set, and it's used to prioritize the overall speed of the ledge climb */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float InterpSpeed;
};
