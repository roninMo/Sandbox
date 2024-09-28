// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DefaultAttributes.generated.h"


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
class SANDBOX_API UDefaultAttributes : public UAttributeSet
{
	GENERATED_BODY()

	
//---------------------------------------------------------------------------------------------------------------------------------------------------//
// In game status attributes																														 //
//---------------------------------------------------------------------------------------------------------------------------------------------------//
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Health) FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Health", ReplicatedUsing = OnRep_MaxHealth) FGameplayAttributeData MaxHealth;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Health", ReplicatedUsing = OnRep_HealthRegenRate) FGameplayAttributeData HealthRegenRate;
	
	// UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Mana) FGameplayAttributeData Mana;
	// UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Mana", ReplicatedUsing = OnRep_MaxMana) FGameplayAttributeData MaxMana;
	// UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Mana", ReplicatedUsing = OnRep_ManaRegenRate) FGameplayAttributeData ManaRegenRate;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Stamina) FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Stamina", ReplicatedUsing = OnRep_MaxStamina) FGameplayAttributeData MaxStamina;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Stamina", ReplicatedUsing = OnRep_StaminaRegenRate) FGameplayAttributeData StaminaRegenRate;
	

public:
	// ThE pUbLiC AtTrIbUtE AcCeSsOrS
	ATTRIBUTE_ACCESSORS(UDefaultAttributes, Health)
	ATTRIBUTE_ACCESSORS(UDefaultAttributes, MaxHealth)
	ATTRIBUTE_ACCESSORS(UDefaultAttributes, HealthRegenRate)
	
	// ATTRIBUTE_ACCESSORS(UDefaultAttributes, Mana)
	// ATTRIBUTE_ACCESSORS(UDefaultAttributes, MaxMana)
	// ATTRIBUTE_ACCESSORS(UDefaultAttributes, ManaRegenRate)
	
	ATTRIBUTE_ACCESSORS(UDefaultAttributes, Stamina)
	ATTRIBUTE_ACCESSORS(UDefaultAttributes, MaxStamina)
	ATTRIBUTE_ACCESSORS(UDefaultAttributes, StaminaRegenRate)
	
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// In game active status attributes
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION() void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) const;
	
	// UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	// UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	// UFUNCTION() void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) const;
	
	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const;
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const;
	UFUNCTION() void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate) const;
	
	
};
