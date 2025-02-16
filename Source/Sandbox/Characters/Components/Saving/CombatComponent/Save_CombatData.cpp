// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Components/Saving/CombatComponent/Save_CombatData.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Combat/CombatComponent.h"


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

	// CombatSaveData = CombatComponent->GetSaveInformation;
	FName CharacterCombatSaveId = SaveComponent->GetSaveTypeIdReference(SaveType);
	// UGameplayStatics::SaveGameToSlot(InventorySaveData, CharacterCombatSaveId, SplitScreenIndexReference);

	return true;
}
