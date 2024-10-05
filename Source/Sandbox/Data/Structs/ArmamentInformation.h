// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
// #include "Sandbox/Data/Enums/SkeletonMappings.h" // Potential error
#include "ArmamentInformation.generated.h"

enum class ECharacterSkeletonMapping : uint8;
enum class EInputAbilities : uint8;


/**
 * An object containing the same montage for different characters. Used for untangling retrieving montages while having multiple characters in a game
 */
USTRUCT(BlueprintType)
struct F_CharacterToMontage
{
	GENERATED_USTRUCT_BODY()
	F_CharacterToMontage() = default;

	/** Character to montage map. Used for any montage, add the different character's montages for a specific montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<ECharacterSkeletonMapping, UAnimMontage*> MontageMappings;
};


/**
 * An object containing the different attack patterns for every armament stance. It is still super tangled, good luck
 */
USTRUCT(BlueprintType)
struct F_ArmamentMeleeMontages
{
	GENERATED_USTRUCT_BODY()
	F_ArmamentMeleeMontages() = default;

	/** The one hand montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EInputAbilities, F_CharacterToMontage> OneHandMontages;

	/** The two hand montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EInputAbilities, F_CharacterToMontage> TwoHandMontages;

	/** The dual wield montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EInputAbilities, F_CharacterToMontage> DualWieldMontages;
	
};



/**
 * An object containing attack montages for an armament
 */
USTRUCT(BlueprintType)
struct F_ArmamentMontages
{
	GENERATED_USTRUCT_BODY()
	F_ArmamentMontages() = default;

	/** The armament's different melee montages for every character. Use @ref EArmamentStance to map each attack  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ArmamentMeleeMontages MeleeMontages;
	
	/** The armament's general montages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, F_CharacterToMontage> Montages;
};





/**
 * This is the data table to hold armament montages for every character
 */
USTRUCT(BlueprintType)
struct F_Table_ArmamentMontages : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ArmamentMontages ArmamentMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};

