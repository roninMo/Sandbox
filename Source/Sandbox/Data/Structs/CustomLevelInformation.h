// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomLevelInformation.generated.h"

/**
 * Config for what to save on the actor // TODO: We should probably use 
 */
USTRUCT(Blueprintable, BlueprintType)
struct F_CustomLevelInformation
{
	GENERATED_USTRUCT_BODY()
	
	virtual ~F_CustomLevelInformation() = default;
	F_CustomLevelInformation() = default;
	
	/** Whether to save the character's attributes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName CustomLevelName;

	/** A stored reference to the actual level's information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LevelId;

	
};
