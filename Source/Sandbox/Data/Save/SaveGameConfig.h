// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameConfig.generated.h"

/**
 * The details for a saved game reference. Used for retrieving save information for all of the characters in the game
 */
UCLASS()
class SANDBOX_API USaveGameConfig : public USaveGame
{
	GENERATED_BODY()

public:
	/** Display Name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString SaveName;

	/** Description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString Description;

	/** Level Reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") ULevel* Level;

	/** Image  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") UTexture2D* Thumbnail;

	/** Network Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 NetId;
	
	/** Platform Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString PlatformId;

	/** Save Slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 SaveSlot;

	/** Current Save Index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 SaveIteration;


public:
	/** Save's the player's information for saving / retrieving save information for individual characters/components */
	UFUNCTION(BlueprintCallable, Category = "SaveGame") virtual void SaveGameConfig(FString& Name, FString& SaveDescription, int32 NetworkId, FString& AccountPlatformId, int32 Slot, int32 Iteration);

	
};
