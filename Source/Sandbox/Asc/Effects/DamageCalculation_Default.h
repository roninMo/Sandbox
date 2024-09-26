// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageCalculation_Default.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UDamageCalculation_Default : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDamageCalculation_Default();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	
};
