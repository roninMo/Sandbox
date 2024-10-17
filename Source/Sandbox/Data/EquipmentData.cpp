// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Data/EquipmentData.h"

#include "Enums/EquipSlot.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Combat/CombatComponent.h"

#define LOCTEXT_NAMESPACE "EquipmentData"


bool UEquipmentData::AddToCharacter(ACharacterBase* Character, FText* ErrorText) const
{
	if (!IsValid(Character))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_Character", "Character is nullptr or invalid (pending kill)");
		if (ErrorText)
		{
			*ErrorText = ErrorMessage;
		}

		return false;
	}

	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	UInventoryComponent* Inventory = Character->GetInventory<UInventoryComponent>();
	if (!IsValid(CombatComponent) || !IsValid(Inventory))
	{
		FText ErrorMessage;
		if (!IsValid(CombatComponent)) ErrorMessage = LOCTEXT("Invalid_CombatComponent", "The Combat Component is nullptr or invalid (pending kill)");
		else ErrorMessage = LOCTEXT("Invalid_Inventory", "The Inventory Component is nullptr or invalid (pending kill)");
		if (ErrorText)
		{
			*ErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}
	
	
	// Add / Retrieve item information from the inventory
	F_Item LeftHandArmament_SlotOne(FGuid::NewGuid(), Armament_LeftHandSlotOne);
	F_Item LeftHandArmament_SlotTwo(FGuid::NewGuid(), Armament_LeftHandSlotTwo);
	F_Item LeftHandArmament_SlotThree(FGuid::NewGuid(), Armament_LeftHandSlotThree);
	F_Item RightHandArmament_SlotOne(FGuid::NewGuid(), Armament_RightHandSlotOne);
	F_Item RightHandArmament_SlotTwo(FGuid::NewGuid(), Armament_RightHandSlotTwo);
	F_Item RightHandArmament_SlotThree(FGuid::NewGuid(), Armament_RightHandSlotThree);

	// Add the item information to the inventory
	Inventory->Execute_TryAddItem(Inventory, LeftHandArmament_SlotOne.Id, Armament_LeftHandSlotOne, nullptr, EItemType::Inv_Weapon);
	Inventory->Execute_TryAddItem(Inventory, LeftHandArmament_SlotTwo.Id, Armament_LeftHandSlotTwo, nullptr, EItemType::Inv_Weapon);
	Inventory->Execute_TryAddItem(Inventory, LeftHandArmament_SlotThree.Id, Armament_LeftHandSlotThree, nullptr, EItemType::Inv_Weapon);
	Inventory->Execute_TryAddItem(Inventory, RightHandArmament_SlotOne.Id, Armament_RightHandSlotOne, nullptr, EItemType::Inv_Weapon);
	Inventory->Execute_TryAddItem(Inventory, RightHandArmament_SlotTwo.Id, Armament_RightHandSlotTwo, nullptr, EItemType::Inv_Weapon);
	Inventory->Execute_TryAddItem(Inventory, RightHandArmament_SlotThree.Id, Armament_RightHandSlotThree, nullptr, EItemType::Inv_Weapon);
	
	// Retrieve the information to build the armament
	Inventory->Execute_GetItem(Inventory, LeftHandArmament_SlotOne, LeftHandArmament_SlotOne.Id, EItemType::Inv_Weapon);
	Inventory->Execute_GetItem(Inventory, LeftHandArmament_SlotTwo, LeftHandArmament_SlotTwo.Id, EItemType::Inv_Weapon);
	Inventory->Execute_GetItem(Inventory, LeftHandArmament_SlotThree, LeftHandArmament_SlotThree.Id, EItemType::Inv_Weapon);
	Inventory->Execute_GetItem(Inventory, RightHandArmament_SlotOne, RightHandArmament_SlotOne.Id, EItemType::Inv_Weapon);
	Inventory->Execute_GetItem(Inventory, RightHandArmament_SlotTwo, RightHandArmament_SlotTwo.Id, EItemType::Inv_Weapon);
	Inventory->Execute_GetItem(Inventory, RightHandArmament_SlotThree, RightHandArmament_SlotThree.Id, EItemType::Inv_Weapon);

	
	bool bEquippedArmaments = true;
	CombatComponent->AddArmamentToEquipSlot(LeftHandArmament_SlotOne, EEquipSlot::LeftHandSlotOne);
	CombatComponent->AddArmamentToEquipSlot(LeftHandArmament_SlotTwo, EEquipSlot::LeftHandSlotTwo);
	CombatComponent->AddArmamentToEquipSlot(LeftHandArmament_SlotThree, EEquipSlot::LeftHandSlotThree);
	CombatComponent->AddArmamentToEquipSlot(RightHandArmament_SlotOne, EEquipSlot::RightHandSlotOne);
	CombatComponent->AddArmamentToEquipSlot(RightHandArmament_SlotTwo, EEquipSlot::RightHandSlotTwo);
	CombatComponent->AddArmamentToEquipSlot(RightHandArmament_SlotThree, EEquipSlot::RightHandSlotThree);
	
	if (EquippedLeftHandArmament != EEquipSlot::None)
	{
		if (!CombatComponent->CreateArmament(EquippedLeftHandArmament)) bEquippedArmaments = false;
	}
	
	if (EquippedRightHandArmament != EEquipSlot::None)
	{
		if (!CombatComponent->CreateArmament(EquippedRightHandArmament)) bEquippedArmaments = false;
	}
	
	return bEquippedArmaments;
}


bool UEquipmentData::RemoveFromCharacter(const ACharacterBase* Character, bool bRemoveFromInventory, FText* ErrorText)
{
	if (!IsValid(Character))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_Character", "Character is nullptr or invalid (pending kill)");
		if (ErrorText)
		{
			*ErrorText = ErrorMessage;
		}

		return false;
	}

	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	UInventoryComponent* Inventory = Character->GetInventory<UInventoryComponent>();
	if (!IsValid(CombatComponent) || (bRemoveFromInventory && !IsValid(Inventory)))
	{
		FText ErrorMessage;
		if (!IsValid(CombatComponent)) ErrorMessage = LOCTEXT("Invalid_CombatComponent", "The Combat Component is nullptr or invalid (pending kill)");
		else ErrorMessage = LOCTEXT("Invalid_InventoryComponent", "The Inventory Component is nullptr or invalid (pending kill)");

		if (ErrorText)
		{
			*ErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}

	if (bRemoveFromInventory)
	{
		// Remove armaments from the inventory
		FGuid LeftHandArmament_SlotOne_Id = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotOne).Id;
		FGuid LeftHandArmament_SlotTwo_Id = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotTwo).Id;
		FGuid LeftHandArmament_SlotThree_Id = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotThree).Id;
		FGuid RightHandArmament_SlotOne_Id = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotOne).Id;
		FGuid RightHandArmament_SlotTwo_Id = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotTwo).Id;
		FGuid RightHandArmament_SlotThree_Id = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotThree).Id;
		if (Inventory)
		{
			Inventory->Execute_TryRemoveItem(Inventory, LeftHandArmament_SlotOne_Id, EItemType::Inv_Weapon, false);
			Inventory->Execute_TryRemoveItem(Inventory, LeftHandArmament_SlotTwo_Id, EItemType::Inv_Weapon, false);
			Inventory->Execute_TryRemoveItem(Inventory, LeftHandArmament_SlotThree_Id, EItemType::Inv_Weapon, false);
			Inventory->Execute_TryRemoveItem(Inventory, RightHandArmament_SlotOne_Id, EItemType::Inv_Weapon, false);
			Inventory->Execute_TryRemoveItem(Inventory, RightHandArmament_SlotTwo_Id, EItemType::Inv_Weapon, false);
			Inventory->Execute_TryRemoveItem(Inventory, RightHandArmament_SlotThree_Id, EItemType::Inv_Weapon, false);
		}
	}
	
	if (CombatComponent)
	{
		CombatComponent->RemoveArmamentFromEquipSlot(EEquipSlot::LeftHandSlotOne);
		CombatComponent->RemoveArmamentFromEquipSlot(EEquipSlot::LeftHandSlotTwo);
		CombatComponent->RemoveArmamentFromEquipSlot(EEquipSlot::LeftHandSlotThree);
		CombatComponent->RemoveArmamentFromEquipSlot(EEquipSlot::RightHandSlotOne);
		CombatComponent->RemoveArmamentFromEquipSlot(EEquipSlot::RightHandSlotTwo);
		CombatComponent->RemoveArmamentFromEquipSlot(EEquipSlot::RightHandSlotThree);
	}
	
	return true;
}
