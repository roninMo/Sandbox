// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DefaultAttributes.h"
#include "MMOAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature);


/**
 * We're speeding this up. Refactor this or add your own for additional stats based on the player's attributes, or if you inspired and loved the dark souls franchise (you take all their good ideas)
 */
UCLASS()
class SANDBOX_API UMMOAttributeSet : public UDefaultAttributes
{
	GENERATED_BODY()
	
	
//--------------------------------------------------------------------------------------------------------------//
// In game status attributes																					//
//--------------------------------------------------------------------------------------------------------------//
public:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Mana) FGameplayAttributeData Mana;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Mana", ReplicatedUsing = OnRep_MaxMana) FGameplayAttributeData MaxMana;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Mana", ReplicatedUsing = OnRep_ManaRegenRate) FGameplayAttributeData ManaRegenRate;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Poise) FGameplayAttributeData Poise;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Poise", ReplicatedUsing = OnRep_MaxPoise) FGameplayAttributeData MaxPoise;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Poise", ReplicatedUsing = OnRep_PoiseRegenRate) FGameplayAttributeData PoiseRegenRate;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_PoisonBuildup) FGameplayAttributeData PoisonBuildup;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_MaxPoisonBuildup) FGameplayAttributeData MaxPoisonBuildup;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_BleedBuildup) FGameplayAttributeData BleedBuildup;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_MaxBleedBuildup) FGameplayAttributeData MaxBleedBuildup;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_FrostbiteBuildup) FGameplayAttributeData FrostbiteBuildup;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_MaxFrostbiteBuildup) FGameplayAttributeData MaxFrostbiteBuildup;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_SleepBuildup) FGameplayAttributeData SleepBuildup;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_MaxSleepBuildup) FGameplayAttributeData MaxSleepBuildup;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_MadnessBuildup) FGameplayAttributeData MadnessBuildup;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_MaxMadnessBuildup) FGameplayAttributeData MaxMadnessBuildup;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_CurseBuildup) FGameplayAttributeData CurseBuildup;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Statuses", ReplicatedUsing = OnRep_MaxCurseBuildup) FGameplayAttributeData MaxCurseBuildup;
	
	

	
//--------------------------------------------------------------------------------------------------------------//
// Level attributes																								//
//--------------------------------------------------------------------------------------------------------------//
public:
	// TODO: Find out if you can use curve tables to multiply based on level for an easier visualization -> (lvl 1 == +1), (lvl 10 == +4), instead of (lvl 1 == 1), (lvl 10 == 10) 
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Vitality) FGameplayAttributeData Vitality;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Endurance) FGameplayAttributeData Endurance;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Mind) FGameplayAttributeData Mind;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Intelligence) FGameplayAttributeData Intelligence;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Faith) FGameplayAttributeData Faith;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Dexterity) FGameplayAttributeData Dexterity;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Strength) FGameplayAttributeData Strength;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Arcane) FGameplayAttributeData Arcane;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Experience) FGameplayAttributeData Experience;

	
//--------------------------------------------------------------------------------------------------------------//
// Secondary attributes (these attributes are increased by the primary attributes)								//
//--------------------------------------------------------------------------------------------------------------//
	/**
	 * The crazy thing about gameplay effects is that they let you adjust attributes and state instantly, through durations, or using infinite effects
	 *
	 * If you want to add fire damage to a weapon, or a status effect, add slow durations, or a passive effect that heals the player
	 *
	 * There's another thing you can do with gameplay effects, and it's attribute based modifications. Read through the documenations because it's extensive, however here's a list of some of the things you should watch out for
	 *		- Attribute based modifications (Modifying one attribute with another)
	 *			- Attribute Coefficients (Using one attribute to affect another and you can use curve tables for attribute/level scaling)
	 *		- Applying one attribute/debuff/effect from a list of currently applied effects (Say you have multiple slows, you can have it so that you only apply one based on certain conditions)
	 *		- Exec Calculations, modifications using multiple attributes to adjust player stats (This is currently how combat calculations are generally handled)
	 *		- Instant changes to attributes
	 *		- Duration or Periodic temporal changes to attributes (bleed effects, or debuffs)
	 *		- Infinite effects (What you should probably use to apply stats to the player's health, stamina, poise, mana, etc.
	 *
	 *	I don't know a good way for combat calculations or how most MMO's have their attributes put together for gameplay, however I'd just take from this for reference, hopefully this helps!
	 */
	
public:
	/**** Defenses ****/
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Defence_Standard) FGameplayAttributeData Defence_Standard;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Defence_Slash) FGameplayAttributeData Defence_Slash;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Defence_Pierce) FGameplayAttributeData Defence_Pierce;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Defence_Strike) FGameplayAttributeData Defence_Strike;
	
	
	/**** Resistances ****/
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Resistance_Magic) FGameplayAttributeData Resistance_Magic;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Resistance_Ice) FGameplayAttributeData Resistance_Ice;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Resistance_Fire) FGameplayAttributeData Resistance_Fire;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Resistance_Holy) FGameplayAttributeData Resistance_Holy;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Resistance_Lightning) FGameplayAttributeData Resistance_Lightning;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Immunity) FGameplayAttributeData Immunity; // poison
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Robustness) FGameplayAttributeData Robustness; // bleed / frostbite
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Focus) FGameplayAttributeData Focus; // sleep / madness

	/**** Other ****/
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Discovery) FGameplayAttributeData Discovery;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Spells) FGameplayAttributeData Spells;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Weight) FGameplayAttributeData Weight;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_EquipLoad) FGameplayAttributeData EquipLoad;
	
	/**** Damage Negations ****/
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Standard) FGameplayAttributeData Negation_Standard;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Slash) FGameplayAttributeData Negation_Slash;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Pierce) FGameplayAttributeData Negation_Pierce;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Strike) FGameplayAttributeData Negation_Strike;
		
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Magic) FGameplayAttributeData Negation_Magic;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Ice) FGameplayAttributeData Negation_Ice;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Fire) FGameplayAttributeData Negation_Fire;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Holy) FGameplayAttributeData Negation_Holy;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats|Secondary", ReplicatedUsing = OnRep_Negation_Lightning) FGameplayAttributeData Negation_Lightning;
	
	
	
	
//------------------------------------------------------------------//
// Meta attributes													//
//------------------------------------------------------------------//
public:
	/**** Damage ****/
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Standard) FGameplayAttributeData Damage_Poise;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Standard) FGameplayAttributeData Damage_Standard;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Slash) FGameplayAttributeData Damage_Slash;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Pierce) FGameplayAttributeData Damage_Pierce;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Strike) FGameplayAttributeData Damage_Strike;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Magic) FGameplayAttributeData Damage_Magic;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Ice) FGameplayAttributeData Damage_Ice;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Fire) FGameplayAttributeData Damage_Fire;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Holy) FGameplayAttributeData Damage_Holy;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Lightning) FGameplayAttributeData Damage_Lightning;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Damage_Lightning) FGameplayAttributeData DamageCalculation;

	/**** Statuses ****/
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Bleed) FGameplayAttributeData Bleed;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Poison) FGameplayAttributeData Poison;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Poison) FGameplayAttributeData Frostbite;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Curse) FGameplayAttributeData Curse;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Poison) FGameplayAttributeData Madness;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Curse) FGameplayAttributeData Sleep;
	
	

	
//------------------------------------------------------------------//
// Utility															//
//------------------------------------------------------------------//
public:
	// ThE pUbLiC AtTrIbUtE AcCeSsOrS
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxMana)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, ManaRegenRate)
	
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Poise)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxPoise)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, PoiseRegenRate)

	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, PoisonBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxPoisonBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, BleedBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxBleedBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, FrostbiteBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxFrostbiteBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, SleepBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxSleepBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MadnessBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxMadnessBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, CurseBuildup)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, MaxCurseBuildup)
	

	// Level Attributes
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Vitality)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Endurance)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Mind)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Intelligence)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Faith)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Dexterity)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Strength)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Arcane)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Experience)

	// Defences
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Defence_Standard)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Defence_Slash)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Defence_Pierce)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Defence_Strike)

	// Resistances
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Resistance_Magic)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Resistance_Ice)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Resistance_Fire)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Resistance_Holy)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Resistance_Lightning)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Immunity)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Robustness)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Focus)

	// Other
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Discovery)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Spells)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Weight)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, EquipLoad)
	
	// Damage Negations
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Standard)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Slash)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Pierce)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Strike)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Magic)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Ice)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Fire)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Holy)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Negation_Lightning)

	// Meta
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Poise)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Standard)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Slash)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Pierce)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Strike)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Magic)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Ice)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Fire)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Holy)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Damage_Lightning)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, DamageCalculation)
	
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Bleed)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Poison)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Frostbite)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Curse)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Madness)
	ATTRIBUTE_ACCESSORS(UMMOAttributeSet, Sleep)
	
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Player Stats
	UFUNCTION() virtual void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	UFUNCTION() virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	UFUNCTION() virtual void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) const;
	
	UFUNCTION() virtual void OnRep_Poise(const FGameplayAttributeData& OldPoise) const;
	UFUNCTION() virtual void OnRep_MaxPoise(const FGameplayAttributeData& OldMaxPoise) const;
	UFUNCTION() virtual void OnRep_PoiseRegenRate(const FGameplayAttributeData& OldPoiseRegenRate) const;

	UFUNCTION() virtual void OnRep_PoisonBuildup(const FGameplayAttributeData& OldPoisonBuildup) const;
	UFUNCTION() virtual void OnRep_MaxPoisonBuildup(const FGameplayAttributeData& OldMaxPoisonBuildup) const;
	UFUNCTION() virtual void OnRep_BleedBuildup(const FGameplayAttributeData& OldBleedBuildup) const;
	UFUNCTION() virtual void OnRep_MaxBleedBuildup(const FGameplayAttributeData& OldMaxBleedBuildup) const;
	UFUNCTION() virtual void OnRep_FrostbiteBuildup(const FGameplayAttributeData& OldFrostbiteBuildup) const;
	UFUNCTION() virtual void OnRep_MaxFrostbiteBuildup(const FGameplayAttributeData& OldMaxFrostbiteBuildup) const;
	UFUNCTION() virtual void OnRep_SleepBuildup(const FGameplayAttributeData& OldSleepBuildup) const;
	UFUNCTION() virtual void OnRep_MaxSleepBuildup(const FGameplayAttributeData& OldMaxSleepBuildup) const;
	UFUNCTION() virtual void OnRep_MadnessBuildup(const FGameplayAttributeData& OldMadnessBuildup) const;
	UFUNCTION() virtual void OnRep_MaxMadnessBuildup(const FGameplayAttributeData& OldMaxMadnessBuildup) const;
	UFUNCTION() virtual void OnRep_CurseBuildup(const FGameplayAttributeData& OldCurseBuildup) const;
	UFUNCTION() virtual void OnRep_MaxCurseBuildup(const FGameplayAttributeData& OldMaxCurseBuildup) const;
	
	// Attributes
	UFUNCTION() virtual void OnRep_Vitality(const FGameplayAttributeData OldVitality) const;
	UFUNCTION() virtual void OnRep_Endurance(const FGameplayAttributeData OldEndurance) const;
	UFUNCTION() virtual void OnRep_Mind(const FGameplayAttributeData OldMind) const;
	UFUNCTION() virtual void OnRep_Intelligence(const FGameplayAttributeData OldDexterity) const;
	UFUNCTION() virtual void OnRep_Faith(const FGameplayAttributeData OldStrength) const;
	UFUNCTION() virtual void OnRep_Dexterity(const FGameplayAttributeData OldDexterity) const;
	UFUNCTION() virtual void OnRep_Strength(const FGameplayAttributeData OldStrength) const;
	UFUNCTION() virtual void OnRep_Arcane(const FGameplayAttributeData OldArcane) const;
	UFUNCTION() virtual void OnRep_Experience(const FGameplayAttributeData OldExperience) const;

	// Defences
	UFUNCTION() virtual void OnRep_Defence_Standard(const FGameplayAttributeData OldDefence_Standard) const;
	UFUNCTION() virtual void OnRep_Defence_Slash(const FGameplayAttributeData OldDefence_Slash) const;
	UFUNCTION() virtual void OnRep_Defence_Pierce(const FGameplayAttributeData OldDefence_Pierce) const;
	UFUNCTION() virtual void OnRep_Defence_Strike(const FGameplayAttributeData OldDefence_Strike) const;

	// Resistances
	UFUNCTION() virtual void OnRep_Resistance_Magic(const FGameplayAttributeData OldResistance_Magic) const;
	UFUNCTION() virtual void OnRep_Resistance_Ice(const FGameplayAttributeData OldResistance_Ice) const;
	UFUNCTION() virtual void OnRep_Resistance_Fire(const FGameplayAttributeData OldResistance_Fire) const;
	UFUNCTION() virtual void OnRep_Resistance_Holy(const FGameplayAttributeData OldResistance_Holy) const;
	UFUNCTION() virtual void OnRep_Resistance_Lightning(const FGameplayAttributeData OldResistance_Lightning) const;
	UFUNCTION() virtual void OnRep_Immunity(const FGameplayAttributeData OldImmunity) const;
	UFUNCTION() virtual void OnRep_Robustness(const FGameplayAttributeData OldRobustness) const;
	UFUNCTION() virtual void OnRep_Focus(const FGameplayAttributeData OldFocus) const;

	// Other
	UFUNCTION() virtual void OnRep_Discovery(const FGameplayAttributeData OldDiscovery) const;
	UFUNCTION() virtual void OnRep_Spells(const FGameplayAttributeData OldSpells) const;
	UFUNCTION() virtual void OnRep_Weight(const FGameplayAttributeData OldWeight) const;
	UFUNCTION() virtual void OnRep_EquipLoad(const FGameplayAttributeData OldEquipLoad) const;
	
	// Damage Negations
	UFUNCTION() virtual void OnRep_Negation_Standard(const FGameplayAttributeData OldNegation_Standard) const;
	UFUNCTION() virtual void OnRep_Negation_Slash(const FGameplayAttributeData OldNegation_Slash) const;
	UFUNCTION() virtual void OnRep_Negation_Pierce(const FGameplayAttributeData OldNegation_Pierce) const;
	UFUNCTION() virtual void OnRep_Negation_Strike(const FGameplayAttributeData OldNegation_Strike) const;
	UFUNCTION() virtual void OnRep_Negation_Magic(const FGameplayAttributeData OldNegation_Magic) const;
	UFUNCTION() virtual void OnRep_Negation_Ice(const FGameplayAttributeData OldNegation_Ice) const;
	UFUNCTION() virtual void OnRep_Negation_Fire(const FGameplayAttributeData OldNegation_Fire) const;
	UFUNCTION() virtual void OnRep_Negation_Holy(const FGameplayAttributeData OldNegation_Holy) const;
	UFUNCTION() virtual void OnRep_Negation_Lightning(const FGameplayAttributeData OldNegation_Lightning) const;

	// Meta Attributes
	UFUNCTION() virtual void OnRep_Damage_Poise(const FGameplayAttributeData OldDamage_Poise) const;
	UFUNCTION() virtual void OnRep_Damage_Standard(const FGameplayAttributeData OldDamage_Standard) const;
	UFUNCTION() virtual void OnRep_Damage_Slash(const FGameplayAttributeData OldDamage_Slash) const;
	UFUNCTION() virtual void OnRep_Damage_Pierce(const FGameplayAttributeData OldDamage_Pierce) const;
	UFUNCTION() virtual void OnRep_Damage_Strike(const FGameplayAttributeData OldDamage_Strike) const;
	UFUNCTION() virtual void OnRep_Damage_Magic(const FGameplayAttributeData OldDamage_Magic) const;
	UFUNCTION() virtual void OnRep_Damage_Ice(const FGameplayAttributeData OldDamage_Ice) const;
	UFUNCTION() virtual void OnRep_Damage_Fire(const FGameplayAttributeData OldDamage_Fire) const;
	UFUNCTION() virtual void OnRep_Damage_Holy(const FGameplayAttributeData OldDamage_Holy) const;
	UFUNCTION() virtual void OnRep_Damage_Lightning(const FGameplayAttributeData OldDamage_Lightning) const;
	UFUNCTION() virtual void OnRep_DamageCalculation(const FGameplayAttributeData OldDamageCalculation) const;
	UFUNCTION() virtual void OnRep_Bleed(const FGameplayAttributeData OldBleed) const;
	UFUNCTION() virtual void OnRep_Poison(const FGameplayAttributeData OldPoison) const;
	UFUNCTION() virtual void OnRep_Frostbite(const FGameplayAttributeData OldFrostbite) const;
	UFUNCTION() virtual void OnRep_Curse(const FGameplayAttributeData OldCurse) const;
	UFUNCTION() virtual void OnRep_Madness(const FGameplayAttributeData OldMadness) const;
	UFUNCTION() virtual void OnRep_Sleep(const FGameplayAttributeData OldSleep) const;



	
};
