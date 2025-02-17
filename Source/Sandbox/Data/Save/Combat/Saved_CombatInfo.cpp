// Fill out your copyright notice in the Description page of Project Settings.


#include "Saved_CombatInfo.h"

#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "Sandbox/Data/Enums/ArmorTypes.h"


void USaved_CombatInfo::SaveFromCombatComponent(UCombatComponent* CombatComponent)
{
	if (!CombatComponent)
	{
		return;
	}

	PrimaryEquipSlot = CombatComponent->GetArmament() ? CombatComponent->GetArmament()->GetEquipSlot() : EEquipSlot::None;
	SecondaryEquipSlot = CombatComponent->GetArmament(false) ? CombatComponent->GetArmament(false)->GetEquipSlot() : EEquipSlot::None;

	LeftHandEquipSlot_One = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotOne);
	LeftHandEquipSlot_Two = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotTwo);
	LeftHandEquipSlot_Three = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotThree);
	RightHandEquipSlot_One = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotOne);
	RightHandEquipSlot_Two = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotTwo);
	RightHandEquipSlot_Three = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotThree);

	CurrentStance = CombatComponent->GetCurrentStance();
	ComboIndex = CombatComponent->GetComboIndex();

	Gauntlets = CombatComponent->GetArmorItemInformation(EArmorSlot::Gauntlets);
	Leggings = CombatComponent->GetArmorItemInformation(EArmorSlot::Leggings);
	Helm = CombatComponent->GetArmorItemInformation(EArmorSlot::Helm);
	Chest = CombatComponent->GetArmorItemInformation(EArmorSlot::Chest);
}
