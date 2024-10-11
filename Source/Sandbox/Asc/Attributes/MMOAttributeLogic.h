// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "MMOAttributeLogic.generated.h"


enum class EHitDirection : uint8;
enum class EHitStun : uint8;


/** Object for containing combat information during attribute calculations */
USTRUCT()
struct FAttributeCombatInformation
{
	GENERATED_BODY()
	
	UPROPERTY() float MagicDamageTaken = 0.0;
	UPROPERTY() float DamageTaken = 0.0;
	UPROPERTY() float PoiseDamageTaken = 0.0;
	UPROPERTY() bool bPoiseBroken = false;
	UPROPERTY() EHitDirection HitDirection;
	UPROPERTY() EHitStun HitStun;
};

/** Object for containing status information during attribute calculations */
USTRUCT()
struct FAttributeStatusInformation
{
	GENERATED_BODY()
	
	UPROPERTY() bool bWasCursed = false;
	UPROPERTY() bool bCharacterBled = false;
	UPROPERTY() bool bWasPoisoned = false;
	UPROPERTY() bool bWasFrostbitten = false;
	UPROPERTY() bool bWasMaddened = false;
	UPROPERTY() bool bSlept = false;
	
	UPROPERTY() float CurseBuildup = 0.0;
	UPROPERTY() float BleedBuildup = 0.0;
	UPROPERTY() float PoisonBuildup = 0.0;
	UPROPERTY() float FrostbiteBuildup = 0.0;
	UPROPERTY() float MadnessBuildup = 0.0;
	UPROPERTY() float SleepBuildup = 0.0;

	bool StatusProc() const
	{
		return this->bWasCursed || 
				this->bCharacterBled || 
				this->bWasCursed || 
				this->bWasFrostbitten || 
				this->bWasMaddened || 
				this->bSlept;
	}

	bool StatusDamage() const
	{
		return
			this->CurseBuildup >= 0.0 ||
	 		this->BleedBuildup >= 0.0 ||
	  		this->PoisonBuildup >= 0.0 ||
	   		this->FrostbiteBuildup >= 0.0 ||
	   		this->MadnessBuildup >= 0.0 ||
			this->SleepBuildup >= 0.0;
	}
};


/**
 * 
 */
UCLASS()
class SANDBOX_API UMMOAttributeLogic : public UMMOAttributeSet
{
	GENERATED_BODY()

protected:
	UMMOAttributeLogic();
	

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

	/** Damage calculations */
	virtual void DamageCalculations(const FGAttributeSetExecutionData& Props, const FGameplayAttribute& Attribute, FAttributeCombatInformation& CombatInformation, const float Value);
	
	/** Attribute calculations for statuses */
	virtual void StatusCalculations(const FGAttributeSetExecutionData& Props, const FGameplayAttribute& Attribute, FAttributeStatusInformation& Statuses, const float Value);

	/** Returns whether the player is immune to the specific debuff */
	virtual bool IsImmuneToAttribute(const FGAttributeSetExecutionData& Props, const FGameplayAttribute& Attribute) const;

	
protected:
	/** Handles clamping attribute adjustments */
	virtual void ClampEvaluatedAttribute(const FGameplayAttribute& AttributeToClamp, FGameplayModifierEvaluatedData& EvaluatedAttribute, const float MinValue, const float MaxValue);


private:
	/**** Cached tags ****/
	/** State when the character is poisoned */
	FGameplayTag PoisonedTag;
	
	/** State when the character is frostbitten */
	FGameplayTag FrostbiteTag;
	
	/** State when the character has gone mad */
	FGameplayTag MaddenedTag;
	
	/** State when the character has been cursed */
	FGameplayTag CursedTag;
	
	/** State tags for the sleepy king */
	FGameplayTag SleepTag;
	
	
};
