// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "FPSAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature);


/**
 * 
 */
UCLASS()
class SANDBOX_API UFPSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	
//--------------------------------------------------------------------------------------------------------------//
// In game status attributes																					//
//--------------------------------------------------------------------------------------------------------------//
protected:
	/** Armor plates, in the form of float values hidden by the attribute set infrastructure (that keeps you trapped in a box) */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Armor) FGameplayAttributeData Armor;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Damage) FGameplayAttributeData Damage;
	
	/*

		You might not actually need these because it's a status effect that you could calculate before you attack.
		However, I'll keep these here because this is just the init attribute logic, so you don't waste time trying to recreate information, and you can just add your own effects for different behaviors

		Ranged Damage types
			- Standard
			- Armor Piercing
			- Hollow Point
			- Incendiary
			- Cryo
			- Explosive
			- Frangible


		I would not add ammo count to attributes, store that information on a combat component and the weapon instead


	*/

	
//------------------------------------------------------------------//
// Utility															//
//------------------------------------------------------------------//
public:
// ThE pUbLiC AtTrIbUtE AcCeSsOrS
	ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Armor)
	ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Damage)
	
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION() void OnRep_Damage(const FGameplayAttributeData& OldDamage) const;

	
};
