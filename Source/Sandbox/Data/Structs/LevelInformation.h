// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LevelInformation.generated.h"


/**
 * Config for what to save on the actor
 */
USTRUCT(Blueprintable, BlueprintType)
struct F_LevelInformation
{
	GENERATED_USTRUCT_BODY()
	
	virtual ~F_LevelInformation() = default;
	F_LevelInformation()
	{
		Id = FGuid::NewGuid();
		CreationDate = FDateTime::UtcNow();
	}

	
//--------------------------------------------------------------//
// Level Reference Information									//
//--------------------------------------------------------------//
	/** Custom level save Id used for retrieving custom levels for a specific account  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGuid Id;
	
	/** Level Name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LevelName;
	
	/** The url used to travel to this level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LevelUrl;

	/** Level Description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Description;
	
	/** Timestamp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDateTime CreationDate;

	
};



/**
 * A data table for storing references to all the levels in the game
 */
USTRUCT(BlueprintType)
struct F_LevelDatabase : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_LevelInformation Information;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};



