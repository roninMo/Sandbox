// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Information/EnemyEquipmentDataSet.h"

#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Data/Enums/ArmorTypes.h"
#include "Sandbox/Data/Enums/EquipSlot.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"


bool UEnemyEquipmentDataSet::AddToCharacter(UAbilitySystem* AbilitySystemComponent, UInventoryComponent* InventoryComponent, UCombatComponent* CombatComponent) const
{
	if (!IsValid(AbilitySystemComponent) || !InventoryComponent || !CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}() Something happened while trying to add character equipment! {1} {2} {3}",
			*FString(__FUNCTION__), *GetNameSafe(AbilitySystemComponent), *GetNameSafe(InventoryComponent), *GetNameSafe(CombatComponent));
		return false;
	}

	// Weapons
	if (!Armament_LeftHandSlotOne.IsNone())
	{
		F_Item LeftHandWeaponSlotOne;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Armament_LeftHandSlotOne, LeftHandWeaponSlotOne);
		CombatComponent->AddArmamentToEquipSlot(LeftHandWeaponSlotOne, EEquipSlot::LeftHandSlotOne);
	}
	if (!Armament_LeftHandSlotTwo.IsNone())
	{
		F_Item LeftHandWeaponSlotTwo;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Armament_LeftHandSlotTwo, LeftHandWeaponSlotTwo);
		CombatComponent->AddArmamentToEquipSlot(LeftHandWeaponSlotTwo, EEquipSlot::LeftHandSlotTwo);
	}
	if (!Armament_LeftHandSlotThree.IsNone())
	{
		F_Item LeftHandWeaponSlotThree;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Armament_LeftHandSlotThree, LeftHandWeaponSlotThree);
		CombatComponent->AddArmamentToEquipSlot(LeftHandWeaponSlotThree, EEquipSlot::LeftHandSlotThree);
	}

	
	if (!Armament_RightHandSlotOne.IsNone())
	{
		F_Item RightHandWeaponSlotOne;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Armament_RightHandSlotOne, RightHandWeaponSlotOne);
		CombatComponent->AddArmamentToEquipSlot(RightHandWeaponSlotOne, EEquipSlot::RightHandSlotOne);
	}
	if (!Armament_RightHandSlotTwo.IsNone())
	{
		F_Item RightHandWeaponSlotTwo;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Armament_RightHandSlotTwo, RightHandWeaponSlotTwo);
		CombatComponent->AddArmamentToEquipSlot(RightHandWeaponSlotTwo, EEquipSlot::RightHandSlotTwo);
	}
	if (!Armament_RightHandSlotThree.IsNone())
	{
		F_Item RightHandWeaponSlotThree;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Armament_RightHandSlotThree, RightHandWeaponSlotThree);
		CombatComponent->AddArmamentToEquipSlot(RightHandWeaponSlotThree, EEquipSlot::RightHandSlotThree);
	}

	// Equipped weapons
	if (EquippedLeftHandArmament != EEquipSlot::None)
	{
		CombatComponent->CreateArmament(EquippedLeftHandArmament);
	}
	if (EquippedRightHandArmament != EEquipSlot::None)
	{
		CombatComponent->CreateArmament(EquippedRightHandArmament);
	}
	
	// Armor
	if (!Gauntlets.IsNone())
	{
		F_Item GauntletsInformation;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Gauntlets, GauntletsInformation);
		CombatComponent->EquipArmor(GauntletsInformation);
	}
	if (!Leggings.IsNone())
	{
		F_Item LeggingsInformation;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Leggings, LeggingsInformation);
		CombatComponent->EquipArmor(LeggingsInformation);
	}
	if (!Helm.IsNone())
	{
		F_Item HelmInformation;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Helm, HelmInformation);
		CombatComponent->EquipArmor(HelmInformation);
	}
	if (!Chest.IsNone())
	{
		F_Item ChestInformation;
		InventoryComponent->Execute_GetDataBaseItem(InventoryComponent, Chest, ChestInformation);
		CombatComponent->EquipArmor(ChestInformation);
	}

	// FGameplayEffectInfo GE_ArmorValuesEffect;
	// GE_ArmorValuesEffect.Effect = ArmorValues;
	// AbilitySystemComponent->AddGameplayEffect(GE_ArmorValuesEffect);
	//
	// FGameplayEffectInfo GE_AttributeValues;
	// GE_AttributeValues.Effect = AttributeValues;
	// AbilitySystemComponent->AddGameplayEffect(GE_AttributeValues);
	

	// Other combat related things
	// AttackPatterns = CharacterInformation.AttackPatterns;
	// CombatClassification = CharacterInformation.CombatClassification;
	return true;
}


bool UEnemyEquipmentDataSet::RemoveFromCharacter(UAbilitySystem* AbilitySystemComponent, UInventoryComponent* InventoryComponent, UCombatComponent* CombatComponent)
{
	if (!IsValid(AbilitySystemComponent) || !InventoryComponent || !CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}() Something happened while trying to remove character equipment! {1} {2} {3}",
			*FString(__FUNCTION__), *GetNameSafe(AbilitySystemComponent), *GetNameSafe(InventoryComponent), *GetNameSafe(CombatComponent));
		return false;
	}


	CombatComponent->DeleteEquippedArmament(CombatComponent->GetArmament(true));
	CombatComponent->DeleteEquippedArmament(CombatComponent->GetArmament(false));

	CombatComponent->UnequipArmor(EArmorSlot::Gauntlets);
	CombatComponent->UnequipArmor(EArmorSlot::Leggings);
	CombatComponent->UnequipArmor(EArmorSlot::Helm);
	CombatComponent->UnequipArmor(EArmorSlot::Chest);

	return true;
}
