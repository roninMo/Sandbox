// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/CharacterGameplayAbility.h"
#include "MovementAbility.generated.h"

class UAdvancedMovementComponent;
/**
 * Movement Ability class with utilities for movement logic
 */
UCLASS()
class SANDBOX_API UMovementAbility : public UCharacterGameplayAbility
{
	GENERATED_BODY()

protected:
	/** Retrieves the advanced movement component from the character */
	UFUNCTION(BlueprintCallable, Category = "Ability|Movement") virtual UAdvancedMovementComponent* GetMovementComponent(AActor* Actor) const;

	
};
