// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemPlayerState.generated.h"

class UAbilitySystem;
class UMMOAttributeLogic;


/**
 * 
 */
UCLASS()
class SANDBOX_API AAbilitySystemPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	/** Stored references */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAbilitySystem> AbilitySystemComponent;
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UMMOAttributeLogic> AttributeSet;

	
public:
	AAbilitySystemPlayerState(const FObjectInitializer& ObjectInitializer);
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Templated convenience version for getting the ability system. */
	template<class T> T* GetAbilitySystem(void) const { return Cast<T>(AbilitySystemComponent); }

	/** Templated convenience version for getting the attribute set. */
	template<class T> T* GetAttributeSet(void) const { return Cast<T>(AttributeSet); }
	
	
};
