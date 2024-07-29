// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Data/Interfaces/InventoryInterface.h"

// Add default functionality here for any IInventoryInterface functions that are not pure virtual.
bool IInventoryInterface::TryAddItem_Implementation(const FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type)
{
	return false;
}

void IInventoryInterface::AddItemPendingClientLogic_Implementation(FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type)
{
}

F_Item IInventoryInterface::HandleAddItem_Implementation(const FGuid& Id, FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type)
{
	return F_Item();
}

void IInventoryInterface::HandleItemAdditionFail_Implementation(const FGuid& Id, FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type)
{
}

void IInventoryInterface::HandleItemAdditionSuccess_Implementation(const FGuid& Id, FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type)
{
}

bool IInventoryInterface::TryTransferItem_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, const EItemType Type)
{
	return false;
}

void IInventoryInterface::TransferItemPendingClientLogic_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, const EItemType Type)
{
}

bool IInventoryInterface::HandleTransferItem_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, const EItemType Type, bool& bFromThisInventory)
{
	return false;
}

void IInventoryInterface::HandleTransferItemFail_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, bool bFromThisInventory)
{
}

void IInventoryInterface::HandleTransferItemSuccess_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, bool bFromThisInventory)
{
}

void IInventoryInterface::HandleTransferItemForOtherInventoryClientLogic(const FGuid& Id, const FName DatabaseId, const EItemType Type, const bool bAddItem)
{
}

bool IInventoryInterface::TryRemoveItem_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem)
{
	return false;
}

void IInventoryInterface::RemoveItemPendingClientLogic_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem)
{
}

bool IInventoryInterface::HandleRemoveItem_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem, UObject*& SpawnedItem)
{
	return false;
}

void IInventoryInterface::HandleRemoveItemFail_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem, UObject* SpawnedItem)
{
}

void IInventoryInterface::HandleRemoveItemSuccess_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem, UObject* SpawnedItem)
{
}


F_Item IInventoryInterface::InternalGetInventoryItem_Implementation(const FGuid& Id, EItemType InventorySectionToSearch)
{
	return F_Item();
}

void IInventoryInterface::InternalAddInventoryItem_Implementation(const F_Item& Item)
{
}

void IInventoryInterface::InternalRemoveInventoryItem_Implementation(const FGuid& Id, EItemType InventorySectionToSearch)
{
}

bool IInventoryInterface::GetItem_Implementation(F_Item& ReturnedItem, FGuid Id, EItemType InventorySectionToSearch)
{
	return false;
}

FString IInventoryInterface::GetPlayerId_Implementation() const
{
	return FString();
}

bool IInventoryInterface::GetDataBaseItem_Implementation(FName Id, F_Item& Item)
{
	return false;
}

F_Item* IInventoryInterface::CreateInventoryObject() const
{
	return new F_Item();
}

TScriptInterface<IInventoryItemInterface> IInventoryInterface::SpawnWorldItem_Implementation(const F_Item& Item, const FTransform& Location)
{
	return nullptr;
}
