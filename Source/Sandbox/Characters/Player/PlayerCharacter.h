// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"
#include "PlayerCharacter.generated.h"



/**
 * 
 */
UCLASS()
class SANDBOX_API APlayerCharacter : public ACharacterCameraLogic
{
	GENERATED_BODY()
	
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
//----------------------------------------------------------------------//
// Initialization functions and components								//
//----------------------------------------------------------------------//
protected:
	virtual void BeginPlay() override;
	
	
//----------------------------------------------------------------------------------//
// Movement																			//
//----------------------------------------------------------------------------------//
protected:

	
//-------------------------------------------------------------------------------------//
// Camera																			   //
//-------------------------------------------------------------------------------------//
protected:

	
//----------------------------------------------------------------------------------//
// Inventory																		//
//----------------------------------------------------------------------------------//
protected:
	
	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
protected:
	
	
};
