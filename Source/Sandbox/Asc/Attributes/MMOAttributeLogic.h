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

protected:
	//~ Begin UAttributeSet interface
	/**
	 *	Called just before modifying the value of an attribute. AttributeSet can make additional modifications here. Return true to continue, or false to throw out the modification.
	 *	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 ssecond +10 movement speed buff.
	 */	
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData &Data) override;
	
	/**
	 *	Called just before any modification happens to an attribute. This is lower level than PreAttributeModify/PostAttribute modify.
	 *	There is no additional context provided here since anything can trigger this. Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
	 *	This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
	 *	
	 *	NewValue is a mutable reference so you are able to clamp the newly applied value as well.
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	/**
	 *	Called just before a GameplayEffect is executed to modify the base value of an attribute. No more changes can be made.
	 *	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 second +10 movement speed buff.
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	/** Called just after any modification happens to an attribute. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	//~ End UAttribute interface


protected:
	/** Handles clamping attribute adjustments */
	virtual void ClampEvaluatedAttribute(const FGameplayAttribute& AttributeToClamp, FGameplayModifierEvaluatedData& EvaluatedAttribute, const float MinValue, const float MaxValue);


protected:
	/**** Cached tags ****/
	/** State when the character is poisoned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PoisonedTag;
	
	/** State when the character has been cursed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag CursedTag;
	
	/** State when the character has gone mad */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag MaddenedTag;
	
	/** State tags for the sleepy king */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag SleepTag;
	
	
};
