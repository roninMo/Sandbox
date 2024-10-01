// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipSlot.generated.h"


/**
 *	The equip slots of characters
 */
UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	None								UMETA(DisplayName = "None"), 
	LeftHandSlotOne						UMETA(DisplayName = "Left Hand Slot One"),
	LeftHandSlotTwo						UMETA(DisplayName = "Left Hand Slot Two"),
	LeftHandSlotThree					UMETA(DisplayName = "Left Hand Slot Three"),
	RightHandSlotOne					UMETA(DisplayName = "Right Hand Slot One"),
	RightHandSlotTwo					UMETA(DisplayName = "Right Hand Slot Two"),
	RightHandSlotThree					UMETA(DisplayName = "Right Hand Slot Three"),
};
