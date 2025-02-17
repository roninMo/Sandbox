// Fill out your copyright notice in the Description page of Project Settings.


#include "Saved_CameraSettings.h"

#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"

void USaved_CameraSettings::SaveFromCameraCharacter(ACharacterCameraLogic* CameraCharacter)
{
	if (!CameraCharacter) return;

	CameraStyle = CameraCharacter->GetCameraStyle();
	CameraOrientation = CameraCharacter->GetCameraOrientation();

	// Retrieve the CDO
	if (CameraStyle == CameraStyle_TargetLocking)
	{
		CameraStyle = ACharacterCameraLogic::StaticClass()->GetDefaultObject<ACharacterCameraLogic>()->GetCameraStyle();
	}

	// TODO: add other custom camera settings
}
