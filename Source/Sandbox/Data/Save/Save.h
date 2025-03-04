// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Save.generated.h"


/**
 * Information used to create hierarchical save references based on the information from the original save.
 */
UCLASS()
class SANDBOX_API USave : public USaveGame
{
	GENERATED_BODY()

public:
	/** Display Name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString SaveName;

	/** Description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString Description;

	/** Level Reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") ALevelScriptActor* Level; // TODO: Update this to an object that has custom level logic

	/** Image  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") UTexture2D* Thumbnail;

	/** Network Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 NetId;
	
	/** Platform Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString PlatformId;

	/** Save Slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 SaveSlot;

	/** Current Save Index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 SaveIndex;

	/** TimeStamp of the official save */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FDateTime Timestamp;


public:
	/** Returns whether the current save information is valid */
	UFUNCTION(BlueprintCallable, Category = "SaveGame") virtual bool IsValidSaveState() const;

	/** Save's the player's information for saving / retrieving save information for individual characters/components */
	UFUNCTION(BlueprintCallable, Category = "SaveGame") virtual void SaveInformation(FString& Name, FString& SaveDescription, int32 NetworkId, FString& AccountPlatformId, int32 Slot, int32 Index);
	
};
