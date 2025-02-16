// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Components/Saving/Inventory/Save_Inventory.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"


bool USave_Inventory::SaveData_Implementation()
{
	// Inventory components are specific to character classes (npc's and players)
	ACharacterBase* Character;
	USaveComponent* SaveComponent;
	if (!GetCharacterAndSaveComponent(SaveComponent, Character))
	{
		return false;
	}

	
	// Retrieve the inventory and save it's information
	UInventoryComponent* InventoryComponent = Character->GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the inventory while trying to save it's information!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(Character)
		);
		return false;
	}

	F_InventorySaveInformation InventoryData = InventoryComponent->GetInventorySaveInformation();
	FName CharacterInventorySaveId = SaveComponent->GetSaveTypeIdReference(SaveType);
	// UGameplayStatics::SaveGameToSlot(InventorySaveData, CharacterInventorySaveId, SplitScreenIndexReference);

	return true;
}
