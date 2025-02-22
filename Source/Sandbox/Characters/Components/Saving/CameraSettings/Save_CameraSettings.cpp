// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Components/Saving/CameraSettings/Save_CameraSettings.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Data/Save/Settings/Camera/Saved_CameraSettings.h"


bool USave_CameraSettings::SaveData_Implementation()
{
	USaveComponent* SaveComponent;
	if (!GetSaveComponent(SaveComponent))
	{
		return false;
	}

	ACharacterCameraLogic* Character = Cast<ACharacterCameraLogic>(SaveComponent->GetOwner());
	if (!Character)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the character's camera information while trying to save it's settings!",
			*UEnum::GetValueAsString(SaveComponent->GetOwner()->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(SaveComponent->GetOwner())
		);
		return false;
	}
	
	USaved_CameraSettings* SaveInformation = NewObject<USaved_CameraSettings>();
	if (!SaveInformation)
	{
		return false;
	}
	
	// Save the character's camera settings
	SaveInformation->SaveFromCameraCharacter(Character);
	FString CameraSaveSlot = SaveComponent->GetSaveSlot(SaveType);
	return UGameplayStatics::SaveGameToSlot(SaveInformation, CameraSaveSlot, 0);
}


bool USave_CameraSettings::LoadData_Implementation()
{
	USaveComponent* SaveComponent;
	if (!GetSaveComponent(SaveComponent))
	{
		return false;
	}

	ACharacterCameraLogic* Character = Cast<ACharacterCameraLogic>(SaveComponent->GetOwner());
	if (!Character)
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} failed to retrieve the character's camera information while trying to save it's settings!",
			*UEnum::GetValueAsString(SaveComponent->GetOwner()->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(SaveComponent->GetOwner())
		);
		return false;
	}

	// Prevent Remote Procedure Call functions from causing latency issues
	if (!Character->HasAuthority())
	{
		return true;
	}
	
	// Retrieve the camera settings
	FString CameraSettingsSaveSlot = SaveComponent->GetSaveSlotReference(SaveType);
	USaved_CameraSettings* CameraSettings = Cast<USaved_CameraSettings>(UGameplayStatics::LoadGameFromSlot(CameraSettingsSaveSlot, 0));
	if (!CameraSettings)
	{
		return false;
	}

	// Load the character's saved camera settings
	Character->Execute_SetCameraOrientation(Character, CameraSettings->CameraOrientation);
	Character->Execute_SetCameraStyle(Character, CameraSettings->CameraStyle);
	return true;
}
