// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Components/Saving/CombatComponent/Save_CombatData.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Data/Save/Combat/Saved_CombatInfo.h"


bool USave_CombatData::SaveData_Implementation(int32 Index)
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
	FString CombatSaveSlot = SaveComponent->GetSaveUrl(SaveType);
	return UGameplayStatics::SaveGameToSlot(SaveInformation, CombatSaveSlot, SaveComponent->GetUserIndex());
}

bool USave_CombatData::LoadData_Implementation(int32 Index)
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
	FString CombatInfoSaveSlot = SaveComponent->GetSaveUrl(SaveType);
	USaved_CombatInfo* CombatInfo = Cast<USaved_CombatInfo>(UGameplayStatics::LoadGameFromSlot(CombatInfoSaveSlot, SaveComponent->GetUserIndex()));
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


FString USave_CombatData::FormattedSaveInformation(const FString Slot) const
{
	// Retrieve Saved Combat Info
	USaved_CombatInfo* CombatInfo = Cast<USaved_CombatInfo>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
	if (!CombatInfo)
	{
		return FString();
	}

	// Retrieve the primary and secondary weapon information
	TArray<FString> Results;
	F_Item PrimaryWeapon;
	F_Item SecondaryWeapon;
	if (CombatInfo->PrimaryEquipSlot == EEquipSlot::LeftHandSlotOne) PrimaryWeapon = CombatInfo->LeftHandEquipSlot_One;
	if (CombatInfo->PrimaryEquipSlot == EEquipSlot::LeftHandSlotTwo) PrimaryWeapon = CombatInfo->LeftHandEquipSlot_Two;
	if (CombatInfo->PrimaryEquipSlot == EEquipSlot::LeftHandSlotThree) PrimaryWeapon = CombatInfo->LeftHandEquipSlot_Three;
	if (CombatInfo->PrimaryEquipSlot == EEquipSlot::RightHandSlotOne) PrimaryWeapon = CombatInfo->RightHandEquipSlot_One;
	if (CombatInfo->PrimaryEquipSlot == EEquipSlot::RightHandSlotTwo) PrimaryWeapon = CombatInfo->RightHandEquipSlot_Two;
	if (CombatInfo->PrimaryEquipSlot == EEquipSlot::RightHandSlotThree) PrimaryWeapon = CombatInfo->RightHandEquipSlot_Three;

	
	// Primary and Secondary Weapons
	Results.Add(FString::Printf(TEXT("Combat Info")));
	Results.Add(FString::Printf(TEXT("{")));
	Results.Add(FString::Printf(TEXT("\tPrimary:      %s,"), *PrimaryWeapon.Print()));
	Results.Add(FString::Printf(TEXT("\tSecondary:  %s,"), *SecondaryWeapon.Print()));

	// Combat Info
	Results.Add(TEXT("\tCombat Info: {"));
	Results.Add(FString::Printf(TEXT("\t\tCurrentStance: %s,"), *UEnum::GetValueAsString(CombatInfo->CurrentStance)));
	Results.Add(FString::Printf(TEXT("\t\tComboIndex: %d,"), CombatInfo->ComboIndex));
	Results.Add(FString::Printf(TEXT("\t\tPrimaryEquipSlot: %s,"), *UEnum::GetValueAsString(CombatInfo->PrimaryEquipSlot)));
	Results.Add(FString::Printf(TEXT("\t\tSecondaryEquipSlot: %s,"), *UEnum::GetValueAsString(CombatInfo->SecondaryEquipSlot)));
	Results.Add(TEXT("\t},"));
	
	// Weapon Slots
	Results.Add(TEXT("\t"));
	Results.Add(TEXT("\tWeapons: {"));
	Results.Add(FString::Printf(TEXT("\t\tLeftHandSlotOne:       %s,"), *CombatInfo->LeftHandEquipSlot_One.Print()));
	Results.Add(FString::Printf(TEXT("\t\tLeftHandSlotTwo:      %s,"), *CombatInfo->LeftHandEquipSlot_Two.Print()));
	Results.Add(FString::Printf(TEXT("\t\tLeftHandSlotThree:    %s,"), *CombatInfo->LeftHandEquipSlot_Three.Print()));
	Results.Add(FString::Printf(TEXT("\t\tRightHandSlotOne:    %s,"), *CombatInfo->RightHandEquipSlot_One.Print()));
	Results.Add(FString::Printf(TEXT("\t\tRightHandSlotTwo:    %s,"), *CombatInfo->RightHandEquipSlot_Two.Print()));
	Results.Add(FString::Printf(TEXT("\t\tRightHandSlotThree: %s,"), *CombatInfo->RightHandEquipSlot_Three.Print()));
	Results.Add(TEXT("\t},"));
	
	// Armor Slots
	Results.Add(TEXT("\t"));
	Results.Add(TEXT("\tArmors: {"));
	Results.Add(FString::Printf(TEXT("\t\tGauntlets:  %s,"), *CombatInfo->Gauntlets.Print()));
	Results.Add(FString::Printf(TEXT("\t\tLeggings:  %s,"), *CombatInfo->Leggings.Print()));
	Results.Add(FString::Printf(TEXT("\t\tHelm:         %s,"), *CombatInfo->Helm.Print()));
	Results.Add(FString::Printf(TEXT("\t\tChest:        %s,"), *CombatInfo->Chest.Print()));
	Results.Add(TEXT("\t}"));

	Results.Add(TEXT("}"));
	return FString::Join(Results, TEXT("\n"));
}

/*

{
	Primary: ZabuzaSword,
	Secondary: null,
	CombatInfo: '',

	Weapons: {
	},

	Armors: {
		
	}
}

*/



