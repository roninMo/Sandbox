// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Characters/Components/Saving/SaveLogic.h"
#include "Save_Settings.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API USave_Settings : public USaveLogic
{
	GENERATED_BODY()
	
public:
	/**
	 * Handles saving the data specific to the owning actor. \n\n
	 * Subclass this logic for saving information specific to a npc, character, and their varying game modes etc.
	 *
	 * @returns	True if it successfully saved the information for the npc / player
	 */
	virtual bool SaveData_Implementation() override;

	/**
	 * Handles loading the data specific to the owning actor. \n\n
	 * Subclass this logic for saving information specific to a npc, character, and their varying game modes etc.
	 *
	 * @returns	True if it successfully loaded the information for the npc / player
	 */
	virtual bool LoadData_Implementation() override;

	/**

		Game Settings
		Video Settings
		Display Settings 
		Camera Settings
		Audio Settings
		Network Settings
		Keyboard and Mouse Settings
		Controller Settings

		Eventually have player specific settings for things like controller/mouse and keyboard settings

	 */
	
};
