// Fill out your copyright notice in the Description page of Project Settings.


#include "Saved_CameraSettings.h"

#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"

void USaved_CameraSettings::SaveFromCameraCharacter(ACharacter* Character)
{
	ACharacterCameraLogic* CameraCharacter = Cast<ACharacterCameraLogic>(Character);
	if (!CameraCharacter) return;

	CameraStyle = CameraCharacter->Execute_GetCameraStyle(CameraCharacter);
	CameraOrientation = CameraCharacter->Execute_GetCameraOrientation(CameraCharacter);

	// Retrieve the CDO
	if (CameraStyle == CameraStyle_TargetLocking)
	{
		// CameraStyle = ACharacterCameraLogic::StaticClass()->GetDefaultObject<ACharacterCameraLogic>()->Execute_GetCameraStyle(CameraCharacter); // TODO: Retrieve the blueprint's default CDO values
		CameraStyle = CameraStyle_ThirdPerson;
	}

	// TODO: add other custom camera settings
}
