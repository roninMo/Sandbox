// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "MMOAttributeLogic.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UMMOAttributeLogic : public UMMOAttributeSet
{
	GENERATED_BODY()


	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

};
