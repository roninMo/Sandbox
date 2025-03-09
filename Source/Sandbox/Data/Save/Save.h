// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Sandbox/Data/Structs/LevelInformation.h"
#include "Save.generated.h"


/**
 * Information used to create hierarchical save references based on the information from the original save.
 */
UCLASS()
class SANDBOX_API USave : public USaveGame
{
	GENERATED_BODY()

//--------------------------------------------------------------------------------------------------------------------------//
// Save Information																											//
//--------------------------------------------------------------------------------------------------------------------------//
public:
	/** Display Name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString SaveName;

	/** Description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString Description;

	/** The Current Level Reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") F_LevelInformation LevelInformation;

	/** Image  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") UTexture2D* Thumbnail;

	/** Network Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 NetId;
	
	/** Platform Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString PlatformId;

	/** Save Slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 SaveSlot;

	/** The base url used for saving / loading the current save, and it's information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FString SaveUrl;

	/** Current Save Index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") int32 SaveIndex;

	/** TimeStamp of the official save */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") FDateTime Timestamp;


	

//--------------------------------------------------------------------------------------------------------------------------//
// Saved references for handling Client/Sever multiplayer logic that stores save information with USaveGame states			//
//--------------------------------------------------------------------------------------------------------------------------//
public:
	/** A list of the players we've saved to this save slot */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") TArray<FString> SavedPlayers;
	 
	/** A list of the levels we've saved to this save slot */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details") TArray<FString> SavedLevels;
	


//--------------------------------------------------------------------------------------------------------------------------//
// Save Functions																											//
//--------------------------------------------------------------------------------------------------------------------------//
public:
	/** Returns whether the current save information is valid */
	UFUNCTION(BlueprintCallable, Category = "SaveGame") virtual bool IsValidSaveState() const;

	/** Save's the player's information for saving / retrieving save information for individual characters/components */
	UFUNCTION(BlueprintCallable, Category = "SaveGame") virtual void SaveInformation(FString Name, FString SaveDescription, int32 NetworkId, FString AccountPlatformId, int32 Slot, int32 Index);

	
};
