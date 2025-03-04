// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatFunctionLibrary.h"

#include "Sandbox/Data/Enums/ArmamentTypes.h"
#include "Sandbox/Data/Enums/EquipSlot.h"


// TODO: Printf strings don't accept not const tchar pointers, and won't allow UEnum::ValueToString. Should we refactor this to just cast to string? This is hacky right now because it's just printed information
// This was the answer ->  // TEXT("MyText %s", *UEnum::GetValueAsString(CombatInfo->CurrentStance))
FString UCombatFunctionLibrary::ArmamentStanceToString(const EArmamentStance Stance)
{
	if (Stance == EArmamentStance::None) 			return "None";
	if (Stance == EArmamentStance::OneHanding) 	return "OneHanding";
	if (Stance == EArmamentStance::TwoWeapons) 	return "TwoWeapons";
	if (Stance == EArmamentStance::DualWielding) 	return "DualWielding";
	if (Stance == EArmamentStance::TwoHanding_L) 	return "TwoHanding_L";
	if (Stance == EArmamentStance::TwoHanding_R) 	return "TwoHanding_R";
	return "null";
}


FString UCombatFunctionLibrary::EquipSlotToString(const EEquipSlot Slot)
{
	if (Slot == EEquipSlot::LeftHandSlotOne)    return "LeftHandSlotOne";
	if (Slot == EEquipSlot::LeftHandSlotTwo)    return "LeftHandSlotTwo";
	if (Slot == EEquipSlot::LeftHandSlotThree)  return "LeftHandSlotThree";
	
	if (Slot == EEquipSlot::RightHandSlotOne)   return "RightHandSlotOne";
	if (Slot == EEquipSlot::RightHandSlotTwo)   return "RightHandSlotTwo";
	if (Slot == EEquipSlot::RightHandSlotThree) return "RightHandSlotThree";
	return "null";
}
