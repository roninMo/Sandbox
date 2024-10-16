// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Data/ArmorData.h"

#include "Enums/ArmorTypes.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Combat/CombatComponent.h"

#define LOCTEXT_NAMESPACE "ArmorData"


bool UArmorData::AddToCharacter(ACharacterBase* Character, FText* ErrorText) const
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
	if (!IsValid(CombatComponent))
	{
		const FText ErrorMessage = LOCTEXT("Invalid_CombatComponent", "The Combat Component is nullptr or invalid (pending kill)");
		if (ErrorText)
		{
			*ErrorText = ErrorMessage;
		}

		// UE_LOGFMT(AbilityLog, Error, "{0}", *ErrorMessage.ToString());
		return false;
	}


	// Add / Retrieve item information from the inventory
	F_Item Information_Gauntlets;
	F_Item Information_Leggings;
	F_Item Information_Helm;
	F_Item Information_Chest;
	if (Inventory)
	{
		Inventory->Execute_TryAddItem(Inventory, Gauntlets, nullptr, EItemType::Inv_Armor);
		Inventory->Execute_TryAddItem(Inventory, Leggings, nullptr, EItemType::Inv_Armor);
		Inventory->Execute_TryAddItem(Inventory, Helm, nullptr, EItemType::Inv_Armor);
		Inventory->Execute_TryAddItem(Inventory, Chest, nullptr, EItemType::Inv_Armor);
	}
	// else
	// Only do this for characters that don't use/need inventory components
	// {
		Information_Gauntlets = F_Item(FGuid::NewGuid(), -1, Gauntlets);
		Information_Leggings = F_Item(FGuid::NewGuid(), -1, Leggings);
		Information_Helm = F_Item(FGuid::NewGuid(), -1, Helm);
		Information_Chest = F_Item(FGuid::NewGuid(), -1, Chest);
	// }

	
	bool bArmorEquipped = true;
	const TArray Armors = {Information_Gauntlets, Information_Leggings, Information_Helm, Information_Chest};
	for (const F_Item& Armor : Armors)
	{
		if (Armor.IsValid())
		{
			if (!CombatComponent->EquipArmor(Armor))
			{
				bArmorEquipped = false;
			}
		}
	}
	
	return bArmorEquipped;
}


bool UArmorData::RemoveFromCharacter(const ACharacterBase* Character, bool bRemoveFromInventory, FText* ErrorText)
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
		// Remove armors from the inventory
		FGuid Gauntlets_Id = CombatComponent->GetArmorItemInformation(EArmorSlot::Gauntlets).Id;
		FGuid Leggings_Id = CombatComponent->GetArmorItemInformation(EArmorSlot::Leggings).Id;
		FGuid Helm_Id = CombatComponent->GetArmorItemInformation(EArmorSlot::Helm).Id;
		FGuid Chest_Id = CombatComponent->GetArmorItemInformation(EArmorSlot::Chest).Id;
		if (Inventory)
		{
			Inventory->Execute_TryRemoveItem(Inventory, Gauntlets_Id, EItemType::Inv_Armor, false);
			Inventory->Execute_TryRemoveItem(Inventory, Leggings_Id, EItemType::Inv_Armor, false);
			Inventory->Execute_TryRemoveItem(Inventory, Helm_Id, EItemType::Inv_Armor, false);
			Inventory->Execute_TryRemoveItem(Inventory, Chest_Id, EItemType::Inv_Armor, false);
		}
	}
	
	bool bArmorUnequipped = true;
	if (CombatComponent)
	{
		if (!CombatComponent->UnequipArmor(EArmorSlot::Gauntlets)) bArmorUnequipped = false;
		if (!CombatComponent->UnequipArmor(EArmorSlot::Leggings)) bArmorUnequipped = false;
		if (!CombatComponent->UnequipArmor(EArmorSlot::Helm)) bArmorUnequipped = false;
		if (!CombatComponent->UnequipArmor(EArmorSlot::Chest)) bArmorUnequipped = false;
	}
	
	return bArmorUnequipped;
}
