// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "SaveComponent_Npc.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API USaveComponent_Npc : public USaveComponent
{
	GENERATED_BODY()
	
public:
	/** Sets the character's Net and Platform Id based on whether it's a player, and if it's single / multiplayer games with subsystems */
	virtual void SetNetAndPlatformId() override;
	
	
};
