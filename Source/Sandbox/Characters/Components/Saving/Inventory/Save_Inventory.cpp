// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Components/Saving/Inventory/Save_Inventory.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Data/Save/Inventory/Saved_Inventory.h"


bool USave_Inventory::SaveData_Implementation()
{
	// Inventory components are specific to character classes (npc's and players)
	ACharacterBase* Character;
	USaveComponent* SaveComponent;
	if (!GetCharacterAndSaveComponent(SaveComponent, Character))
	{
		return false;
	}
	
	// Check if the character has an inventory
	UInventoryComponent* InventoryComponent = Character->GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the inventory while trying to save it's information!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(Character)
		);
		return false;
	}

	
	// Save the character's inventory
	USaved_Inventory* SaveInformation = NewObject<USaved_Inventory>();
	if (!SaveInformation)
	{
		return false;
	}
	
	SaveInformation->SaveInformation = InventoryComponent->GetInventorySaveInformation();
	FString InventorySaveSlot = SaveComponent->GetSaveSlotIdReference(SaveType);
	return UGameplayStatics::SaveGameToSlot(SaveInformation, InventorySaveSlot, 0);
}

bool USave_Inventory::LoadData_Implementation()
{
	ACharacterBase* Character;
	USaveComponent* SaveComponent;
	if (!GetCharacterAndSaveComponent(SaveComponent, Character))
	{
		return false;
	}
	
	// Check if the character has an inventory
	UInventoryComponent* InventoryComponent = Character->GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the inventory while trying to save it's information!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(Character)
		);
		return false;
	}
	
	
	// Retrieve the saved inventory
	FString InventorySaveSlot = SaveComponent->GetSaveSlotIdReference(SaveType);
	USaved_Inventory* InventoryData = Cast<USaved_Inventory>(UGameplayStatics::LoadGameFromSlot(InventorySaveSlot, 0));
	if (!InventoryData)
	{
		return false;
	}

	InventoryComponent->LoadInventoryInformation(InventoryData->SaveInformation);
	return true;
}


FString USave_Inventory::FormattedSaveInformation(const FString Slot) const
{
	// Retrieve the saved inventory
	USaved_Inventory* InventoryData = Cast<USaved_Inventory>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
	if (!InventoryData)
	{
		return FString();
	}

	if (InventoryData->SaveInformation.InventoryItems.IsEmpty())
	{
		return FString();
	}
	
	return InventoryData->SaveInformation.Print();
}

/*
{
	NetId: 0,
	PlatformId: dfgjdsfgkjldsfg,

	Items: [
		Id: 0000, ItemName: Name, SortOrder: 0,
		Id: 0000, ItemName: Name, SortOrder: 0,
		Id: 0000, ItemName: Name, SortOrder: 0,
		Id: 0000, ItemName: Name, SortOrder: 0,
	]
	
}
*/