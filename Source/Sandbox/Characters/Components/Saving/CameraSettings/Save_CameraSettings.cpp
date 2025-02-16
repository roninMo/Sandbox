// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Components/Saving/CameraSettings/Save_CameraSettings.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"


bool USave_CameraSettings::SaveData_Implementation()
{
	USaveComponent* SaveComponent;
	if (GetSaveComponent(SaveComponent))
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

	// CameraSettings = Character->GetCameraSettings;
	FName CharacterCameraSettingsId = SaveComponent->GetSaveTypeIdReference(SaveType);
	// UGameplayStatics::SaveGameToSlot(CameraSettings, CharacterCameraSettingsId, SplitScreenIndexReference);

	return true;
}
