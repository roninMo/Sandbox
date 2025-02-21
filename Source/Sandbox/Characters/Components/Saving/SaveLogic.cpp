// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLogic.h"

#include "SaveComponent.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Enums/ESaveType.h"


USaveLogic::USaveLogic()
{
	Id = FGuid();
	DisplayName = FName("Save_Information");
	bAutoSave = false;
	SaveType = ESaveType::MAX;
}


bool USaveLogic::SaveData_Implementation()
{
	return true;
}


bool USaveLogic::IsValidToSave_Implementation()
{
	USaveComponent* SaveComponent;
	if (!GetSaveComponent(SaveComponent)) return false;
	
	return true;
}


bool USaveLogic::LoadData_Implementation()
{
	return true;
}


bool USaveLogic::GetSaveComponent(USaveComponent*& OutSaveComponent)
{
	OutSaveComponent = Cast<USaveComponent>(GetOuter());
	if (!OutSaveComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0}() {1} failed to retrieve the save component while saving the {2}!",
			*FString(__FUNCTION__), GetNameSafe(GetOuter()), *UEnum::GetValueAsString(SaveType)
		);
		return false;
	}

	return true;
}


bool USaveLogic::GetCharacterAndSaveComponent(USaveComponent*& OutSaveComponent, ACharacterBase*& OutCharacter)
{
	OutSaveComponent = Cast<USaveComponent>(GetOuter());
	if (!OutSaveComponent)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0}() {1} failed to retrieve the save component while saving the {2}!",
			*FString(__FUNCTION__), GetNameSafe(GetOuter()), *UEnum::GetValueAsString(SaveType)
		);
		return false;
	}
	
	OutCharacter = Cast<ACharacterBase>(OutSaveComponent->GetOwner());
	if (!OutCharacter)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() tried to create save logic for {2} with an invalid reference to the character!",
			*UEnum::GetValueAsString(OutSaveComponent->GetOwner()->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(OutSaveComponent->GetOwner())
		);
		return false;
	}

	return true;
}

FString USaveLogic::FormattedSaveInformation(const FString Slot) const
{
	return FString();
}


void USaveLogic::SetSaveLogicInformation(const F_SaveLogicConfiguration& Data)
{
	if (!Data.IsValid()) return;
	
	Id = Data.Id;
	DisplayName = Data.DisplayName;
	bSaveDuringEndPlay = Data.bSaveDuringEndPlay;
	SaveType = Data.SaveType;
	bAutoSave = Data.bAutoSave;
}


FGuid USaveLogic::GetId_Implementation() const { return Id; }
FName USaveLogic::GetDisplayName_Implementation() const { return DisplayName; }
bool USaveLogic::IsValidAutoSave_Implementation() const { return bAutoSave; }
ESaveType USaveLogic::GetSaveType_Implementation() const { return SaveType; }
bool USaveLogic::NeedsLoadForClient() const { return false; }
