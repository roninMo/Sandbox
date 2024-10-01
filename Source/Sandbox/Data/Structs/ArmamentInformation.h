// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sandbox/Data/Enums/MontageMappings.h" // Potential error
#include "ArmamentInformation.generated.h"

// enum class ECharacterToMontageMapping : uint8;
enum class EComboType : uint8;


/**
 * An object containing the same montage for different characters. Used for untangling retrieving montages while having multiple characters in a game
 */
USTRUCT(BlueprintType)
struct F_CharacterToMontage
{
	GENERATED_USTRUCT_BODY()
	F_CharacterToMontage() = default;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<TEnumAsByte<ECharacterToMontageMapping>, UAnimMontage*> MontageMappings;
};




/**
 * This is the data table to hold armament montages for every character
 */
USTRUCT(BlueprintType)
struct F_Table_ArmamentMontages : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EComboType, F_CharacterToMontage> ArmamentMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};

