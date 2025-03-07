// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelInformation.generated.h"


/**
 * Config for what to save on the actor
 */
USTRUCT(Blueprintable, BlueprintType)
struct F_LevelInformation
{
	GENERATED_USTRUCT_BODY()
	
	virtual ~F_LevelInformation() = default;
	F_LevelInformation() = default;

	/** Custom level save Id used for retrieving custom levels for a specific account  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
	
	/** Level Name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LevelName;
	
	/** The url used to travel to this level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LevelUrl;

	/** Level Description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Description;
	
	/** Custom Level Information containing spawn information for Props, Actors, Enemies, Weapons, GameMode Objects, Vehicles, etc */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) CustomInformation;
	
	/** Timestamp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDateTime CreationDate;

	
};

