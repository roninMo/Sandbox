// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AISenseInformation.generated.h"


/**
 * The ai senses information for perceptions
 */
USTRUCT(BlueprintType)
struct F_AIHearingSenseConfig
{
    GENERATED_USTRUCT_BODY()
        F_AIHearingSenseConfig(
            const bool bHasHearingSense = false,
            const float HearingRange = 3400,
            const bool bDetectEnemies = false,
            const bool bDetectNeutrals = false,
            const bool bDetectFriendlies = false
        ) :

        bHasHearingSense(bHasHearingSense),
        HearingRange(HearingRange),
        bDetectEnemies(bDetectEnemies),
        bDetectNeutrals(bDetectNeutrals),
        bDetectFriendlies(bDetectFriendlies)
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasHearingSense = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HearingRange;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bDetectEnemies;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bDetectNeutrals;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bDetectFriendlies;
};


/**
 * The ai sight senses information
 */
USTRUCT(BlueprintType)
struct F_AISightSenseConfig
{
    GENERATED_USTRUCT_BODY()
        F_AISightSenseConfig(
            const bool bHasSightSense = false,
            const float SightRadius = 1640,
            const float LoseSightRadius = 2500,
            const float PeripheralVisionAngleDegrees = 85,
            const float AutoSuccessRangeFromLastSeenLocation = -1,
            const float PointOfViewBackwardOffset = 0,
            const float NearClippingRadius = 0
        ) :

        bHasSightSense(bHasSightSense),
        SightRadius(SightRadius),
        LoseSightRadius(LoseSightRadius),
        PeripheralVisionAngleDegrees(PeripheralVisionAngleDegrees),
        AutoSuccessRangeFromLastSeenLocation(AutoSuccessRangeFromLastSeenLocation),
        PointOfViewBackwardOffset(PointOfViewBackwardOffset),
        NearClippingRadius(NearClippingRadius)
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasSightSense = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LoseSightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PeripheralVisionAngleDegrees;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AutoSuccessRangeFromLastSeenLocation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PointOfViewBackwardOffset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float NearClippingRadius;
};


/**
 * The ai sight senses information
 */
USTRUCT(BlueprintType)
struct F_AITeamSenseConfig
{
    GENERATED_USTRUCT_BODY()
        F_AITeamSenseConfig(
            const bool bHasTeamSense = false,
            const float SightRadius = 1640,
            const float LoseSightRadius = 2500,
            const float PeripheralVisionAngleDegrees = 85,
            const float AutoSuccessRangeFromLastSeenLocation = -1,
            const float PointOfViewBackwardOffset = 0,
            const float NearClippingRadius = 0
        ) :

        bHasTeamSense(bHasTeamSense),
        SightRadius(SightRadius),
        LoseSightRadius(LoseSightRadius),
        PeripheralVisionAngleDegrees(PeripheralVisionAngleDegrees),
        AutoSuccessRangeFromLastSeenLocation(AutoSuccessRangeFromLastSeenLocation),
        PointOfViewBackwardOffset(PointOfViewBackwardOffset),
        NearClippingRadius(NearClippingRadius)
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasTeamSense = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LoseSightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PeripheralVisionAngleDegrees;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AutoSuccessRangeFromLastSeenLocation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PointOfViewBackwardOffset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float NearClippingRadius;
};


/**
 * The ai senses information for perceptions
 */
USTRUCT(BlueprintType)
struct F_AISenseConfigurations
{
    GENERATED_USTRUCT_BODY()
        F_AISenseConfigurations(
            const F_AIHearingSenseConfig& HearingSenseConfig = F_AIHearingSenseConfig(),
            const F_AISightSenseConfig& SightSenseConfig = F_AISightSenseConfig()
        ) :

        HearingSenseConfig(HearingSenseConfig),
        SightSenseConfig(SightSenseConfig)
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) F_AIHearingSenseConfig HearingSenseConfig;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) F_AISightSenseConfig SightSenseConfig;
};
