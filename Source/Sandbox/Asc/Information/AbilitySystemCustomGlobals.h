// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemCustomGlobals.generated.h"

/**
 * 
 */
UCLASS()
class UAbilitySystemCustomGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
	// Create our own FGameplayAbilityActorInfo to pass the information for the abilities

	// Subclass the AbilitySystemGlobals class
	
	// Subclass the FGameplayEffectContext 


	/** Should allocate a project specific AbilityActorInfo struct. Caller is responsible for deallocation */
	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const override;

	/** Should allocate a project specific GameplayEffectContext struct. Caller is responsible for deallocation */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;


	/** Returns the gameplay cue manager singleton object, creating if necessary */
	virtual UGameplayCueManager* GetGameplayCueManager() override;


	
	/** Global callback that can handle game-specific code that needs to run before applying a gameplay effect spec */
	virtual void GlobalPreGameplayEffectSpecApply(FGameplayEffectSpec& Spec, UAbilitySystemComponent* AbilitySystemComponent) override;

	/** Override to handle global state when gameplay effects are being applied */
	virtual void PushCurrentAppliedGE(const FGameplayEffectSpec* Spec, UAbilitySystemComponent* AbilitySystemComponent) override;
	virtual void SetCurrentAppliedGE(const FGameplayEffectSpec* Spec) override;
	virtual void PopCurrentAppliedGE() override;
	
#if WITH_EDITOR
	// Allows projects to override PostEditChangeProeprty on GEs without having to subclass Gameplayeffect. Intended for validation/auto populating based on changed data.
	virtual void GameplayEffectPostEditChangeProperty(class UGameplayEffect* GE, FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	
};
