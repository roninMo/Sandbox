// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "CombatAbilitySystem.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UCombatAbilitySystem : public UAbilitySystem
{
	GENERATED_BODY()


protected:
	/**
	 * Input binding for activating abilities and their input events using @ref BindAbilityActivationToInputComponent()
	 * It checks if there's local input pressed/released event delegates and adds them if so, then it loops through the abilities and invoke's their input events or activates their ability 
	 */
	virtual void AbilityLocalInputPressed(int32 InputID) override;

	/**
	 * Input binding for activated abilities input events using @ref BindAbilityActivationToInputComponent()
	 * Activates input released events for specific abilities
	 */
	virtual void AbilityLocalInputReleased(int32 InputID) override;

	
};
