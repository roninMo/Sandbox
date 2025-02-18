// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Components/Saving/CombatComponent/Save_CombatData.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Data/Save/Combat/Saved_CombatInfo.h"


bool USave_CombatData::SaveData_Implementation()
{
	// Inventory components are specific to character classes (npc's and players)
	ACharacterBase* Character;
	USaveComponent* SaveComponent;
	if (!GetCharacterAndSaveComponent(SaveComponent, Character))
	{
		return false;
	}
	
	// Retrieve the combat component and save it's relevant information
	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the combat component while trying to save it's information!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(Character)
		);
		return false;
	}
	
	USaved_CombatInfo* SaveInformation = NewObject<USaved_CombatInfo>();
	if (!SaveInformation)
	{
		return false;
	}
	
	// Save the character's camera settings
	SaveInformation->SaveFromCombatComponent(CombatComponent);
	FString CombatSaveSlot = SaveComponent->GetSaveSlotIdReference(SaveType);
	return UGameplayStatics::SaveGameToSlot(SaveInformation, CombatSaveSlot, 0);
}

bool USave_CombatData::LoadData_Implementation()
{
	// Inventory components are specific to character classes (npc's and players)
	ACharacterBase* Character;
	USaveComponent* SaveComponent;
	if (!GetCharacterAndSaveComponent(SaveComponent, Character))
	{
		return false;
	}
	
	// Retrieve the combat component and save it's relevant information
	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the combat component while trying to load it's information!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(Character)
		);
		return false;
	}

	UInventoryComponent* InventoryComponent = Character->GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the inventory component while trying to load it's information!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(Character)
		);
		return false;
	}
	
	
	// Retrieve the combat info
	FString CombatInfoSaveSlot = SaveComponent->GetSaveSlotIdReference(SaveType);
	USaved_CombatInfo* CombatInfo = Cast<USaved_CombatInfo>(UGameplayStatics::LoadGameFromSlot(CombatInfoSaveSlot, 0));
	if (!CombatInfo)
	{
		return false;
	}
	
	// Update the combat component with it's information // TODO: eventually just use the inventory to retrieve the weapons and armor
	CombatComponent->AddArmamentToEquipSlot(CombatInfo->LeftHandEquipSlot_One, EEquipSlot::LeftHandSlotOne);
	CombatComponent->AddArmamentToEquipSlot(CombatInfo->LeftHandEquipSlot_Two, EEquipSlot::LeftHandSlotTwo);
	CombatComponent->AddArmamentToEquipSlot(CombatInfo->LeftHandEquipSlot_Three, EEquipSlot::LeftHandSlotThree);
	CombatComponent->AddArmamentToEquipSlot(CombatInfo->RightHandEquipSlot_One, EEquipSlot::RightHandSlotOne);
	CombatComponent->AddArmamentToEquipSlot(CombatInfo->RightHandEquipSlot_Two, EEquipSlot::RightHandSlotTwo);
	CombatComponent->AddArmamentToEquipSlot(CombatInfo->RightHandEquipSlot_Three, EEquipSlot::RightHandSlotThree);

	// Equip the weapons
	CombatComponent->CreateArmament(CombatInfo->PrimaryEquipSlot);
	CombatComponent->CreateArmament(CombatInfo->SecondaryEquipSlot);
	CombatComponent->SetComboIndex(CombatInfo->ComboIndex);
	CombatComponent->SetArmamentStance(CombatInfo->CurrentStance);
	
	// Add the armor
	CombatComponent->EquipArmor(CombatInfo->Gauntlets);
	CombatComponent->EquipArmor(CombatInfo->Leggings);
	CombatComponent->EquipArmor(CombatInfo->Helm);
	CombatComponent->EquipArmor(CombatInfo->Chest);

	return true;
}
