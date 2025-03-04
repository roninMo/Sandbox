// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatFunctionLibrary.generated.h"

enum class EArmamentStance : uint8;
enum class EEquipSlot : uint8;
/**
 * 
 */
UCLASS()
class SANDBOX_API UCombatFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:


	
//-------------------------------------------------------------------------------------//
// Armament Functionality															   //
//-------------------------------------------------------------------------------------//
public:
	/** Retrieves the formatted string version of EArmamentStance */
	UFUNCTION(BlueprintCallable, Category = "Combat Functions|Utils") static FString ArmamentStanceToString(const EArmamentStance Stance);

	/** Retrieves the formatted string version of EEquipSlot */
	UFUNCTION(BlueprintCallable, Category = "Combat Functions|Utils") static FString EquipSlotToString(const EEquipSlot Slot);

	
};
