// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AISenseInformation.generated.h"


/**
 * This ai senses information for perceptions
 */
USTRUCT(BlueprintType)
struct F_AISenseConfigurations
{
    GENERATED_USTRUCT_BODY()
        F_AISenseConfigurations(
            const bool bHasSight = false,
            const bool bHasTeam = false,
            const bool bHasPrediction = false,
            const bool bHasHearing = false,
            
            const float SightRadius = 1640,
            const float LoseSightRadius = 2500,
            const float PeripheralVisionAngleDegrees = 85,
            const float AutoSuccessRangeFromLastSeenLocation = -1,
            const float PointOfViewBackwardOffset = 0,
            const float NearClippingRadius = 0,
            
            const float HearingRange = 3400
        ) :

        bHasSight(bHasSight),
        bHasTeam(bHasTeam),
        bHasPrediction(bHasPrediction),
        bHasHearing(bHasHearing),
    
        SightRadius(SightRadius),
        LoseSightRadius(LoseSightRadius),
        PeripheralVisionAngleDegrees(PeripheralVisionAngleDegrees),
        AutoSuccessRangeFromLastSeenLocation(AutoSuccessRangeFromLastSeenLocation),
        PointOfViewBackwardOffset(PointOfViewBackwardOffset),
        NearClippingRadius(NearClippingRadius),
    
        HearingRange(HearingRange)
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasSight = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasTeam = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasPrediction = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasHearing = true;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LoseSightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PeripheralVisionAngleDegrees;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AutoSuccessRangeFromLastSeenLocation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PointOfViewBackwardOffset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float NearClippingRadius;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HearingRange;
};
