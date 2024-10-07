// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UMMOAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// Fuck
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, ManaRegenRate, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Poise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxPoise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, PoiseRegenRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, PoisonBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxPoisonBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, BleedBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxBleedBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, FrostbiteBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxFrostbiteBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, SleepBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxSleepBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MadnessBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxMadnessBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, CurseBuildup, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, MaxCurseBuildup, COND_None, REPNOTIFY_Always);
	
	// Level Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Vitality, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Endurance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Mind, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Faith, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Arcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Experience, COND_None, REPNOTIFY_Always);

	// Defences
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Defence_Standard, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Defence_Slash, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Defence_Pierce, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Defence_Strike, COND_None, REPNOTIFY_Always);

	// Resistances
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Resistance_Magic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Resistance_Ice, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Resistance_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Resistance_Holy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Resistance_Lightning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Immunity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Robustness, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Focus, COND_None, REPNOTIFY_Always);

	// Other
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Discovery, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Spells, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Weight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, EquipLoad, COND_None, REPNOTIFY_Always);
	
	// Damage Negations 
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Standard, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Slash, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Pierce, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Strike, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Magic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Ice, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Holy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Negation_Lightning, COND_None, REPNOTIFY_Always);
	
	// Meta
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Standard, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Slash, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Pierce, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Strike, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Magic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Ice, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Holy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Damage_Lightning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, DamageCalculation, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Bleed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Frostbite, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Poison, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Curse, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Madness, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMOAttributeSet, Sleep, COND_None, REPNOTIFY_Always);
}


// Player Stats
void UMMOAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const												{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Mana, OldMana); }
void UMMOAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const										{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxMana, OldMaxMana); }
void UMMOAttributeSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) const							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, ManaRegenRate, OldManaRegenRate); }

void UMMOAttributeSet::OnRep_Poise(const FGameplayAttributeData& OldPoise) const											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Poise, OldPoise); }
void UMMOAttributeSet::OnRep_MaxPoise(const FGameplayAttributeData& OldMaxPoise) const										{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxPoise, OldMaxPoise); }
void UMMOAttributeSet::OnRep_PoiseRegenRate(const FGameplayAttributeData& OldPoiseRegenRate) const							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, PoiseRegenRate, OldPoiseRegenRate); }

void UMMOAttributeSet::OnRep_PoisonBuildup(const FGameplayAttributeData& OldPoisonBuildup) const 							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, PoisonBuildup, OldPoisonBuildup); }
void UMMOAttributeSet::OnRep_MaxPoisonBuildup(const FGameplayAttributeData& OldMaxPoisonBuildup) const						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxPoisonBuildup, OldMaxPoisonBuildup); }

void UMMOAttributeSet::OnRep_BleedBuildup(const FGameplayAttributeData& OldBleedBuildup) const 								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, BleedBuildup, OldBleedBuildup); }
void UMMOAttributeSet::OnRep_MaxBleedBuildup(const FGameplayAttributeData& OldMaxBleedBuildup) const						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxBleedBuildup, OldMaxBleedBuildup); }

void UMMOAttributeSet::OnRep_FrostbiteBuildup(const FGameplayAttributeData& OldFrostbiteBuildup) const 						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, FrostbiteBuildup, OldFrostbiteBuildup); }
void UMMOAttributeSet::OnRep_MaxFrostbiteBuildup(const FGameplayAttributeData& OldMaxFrostbiteBuildup) const				{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxFrostbiteBuildup, OldMaxFrostbiteBuildup); }

void UMMOAttributeSet::OnRep_SleepBuildup(const FGameplayAttributeData& OldSleepBuildup) const 								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, SleepBuildup, OldSleepBuildup); }
void UMMOAttributeSet::OnRep_MaxSleepBuildup(const FGameplayAttributeData& OldMaxSleepBuildup) const						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxSleepBuildup, OldMaxSleepBuildup); }

void UMMOAttributeSet::OnRep_MadnessBuildup(const FGameplayAttributeData& OldMadnessBuildup) const 							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MadnessBuildup, OldMadnessBuildup); }
void UMMOAttributeSet::OnRep_MaxMadnessBuildup(const FGameplayAttributeData& OldMaxMadnessBuildup) const					{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxMadnessBuildup, OldMaxMadnessBuildup); }

void UMMOAttributeSet::OnRep_CurseBuildup(const FGameplayAttributeData& OldCurseBuildup) const								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, CurseBuildup, OldCurseBuildup); }
void UMMOAttributeSet::OnRep_MaxCurseBuildup(const FGameplayAttributeData& OldMaxCurseBuildup) const						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, MaxCurseBuildup, OldMaxCurseBuildup); }


// Level Attributes
void UMMOAttributeSet::OnRep_Vitality(const FGameplayAttributeData OldVitality) const 										{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Vitality, OldVitality); }
void UMMOAttributeSet::OnRep_Endurance(const FGameplayAttributeData OldEndurance) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Endurance, OldEndurance); }
void UMMOAttributeSet::OnRep_Mind(const FGameplayAttributeData OldMind) const 												{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Mind, OldMind); }
void UMMOAttributeSet::OnRep_Intelligence(const FGameplayAttributeData OldDexterity) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Dexterity, OldDexterity); }
void UMMOAttributeSet::OnRep_Faith(const FGameplayAttributeData OldStrength) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Strength, OldStrength); }
void UMMOAttributeSet::OnRep_Dexterity(const FGameplayAttributeData OldDexterity) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Dexterity, OldDexterity); }
void UMMOAttributeSet::OnRep_Strength(const FGameplayAttributeData OldStrength) const 										{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Strength, OldStrength); }
void UMMOAttributeSet::OnRep_Arcane(const FGameplayAttributeData OldArcane) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Arcane, OldArcane); }
void UMMOAttributeSet::OnRep_Experience(const FGameplayAttributeData OldExperience) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Experience, OldExperience); }


// Defences
void UMMOAttributeSet::OnRep_Defence_Standard(const FGameplayAttributeData OldDefence_Standard) const 						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Defence_Standard, OldDefence_Standard); }
void UMMOAttributeSet::OnRep_Defence_Slash(const FGameplayAttributeData OldDefence_Slash) const 							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Defence_Slash, OldDefence_Slash); }
void UMMOAttributeSet::OnRep_Defence_Pierce(const FGameplayAttributeData OldDefence_Pierce) const 							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Defence_Pierce, OldDefence_Pierce); }
void UMMOAttributeSet::OnRep_Defence_Strike(const FGameplayAttributeData OldDefence_Strike) const 							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Defence_Strike, OldDefence_Strike); }

// Resistances
void UMMOAttributeSet::OnRep_Resistance_Magic(const FGameplayAttributeData OldResistance_Magic) const						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Resistance_Magic, OldResistance_Magic); }
void UMMOAttributeSet::OnRep_Resistance_Ice(const FGameplayAttributeData OldResistance_Ice) const							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Resistance_Ice, OldResistance_Ice); }
void UMMOAttributeSet::OnRep_Resistance_Fire(const FGameplayAttributeData OldResistance_Fire) const							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Resistance_Fire, OldResistance_Fire); }
void UMMOAttributeSet::OnRep_Resistance_Holy(const FGameplayAttributeData OldResistance_Holy) const							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Resistance_Holy, OldResistance_Holy); }
void UMMOAttributeSet::OnRep_Resistance_Lightning(const FGameplayAttributeData OldResistance_Lightning) const				{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Resistance_Lightning, OldResistance_Lightning); }
void UMMOAttributeSet::OnRep_Immunity(const FGameplayAttributeData OldImmunity) const										{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Immunity, OldImmunity); }
void UMMOAttributeSet::OnRep_Robustness(const FGameplayAttributeData OldRobustness) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Robustness, OldRobustness); }
void UMMOAttributeSet::OnRep_Focus(const FGameplayAttributeData OldFocus) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Focus, OldFocus); }

// Other
void UMMOAttributeSet::OnRep_Discovery(const FGameplayAttributeData OldDiscovery) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Discovery, OldDiscovery); }
void UMMOAttributeSet::OnRep_Spells(const FGameplayAttributeData OldSpells) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Spells, OldSpells); }
void UMMOAttributeSet::OnRep_Weight(const FGameplayAttributeData OldWeight) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Weight, OldWeight); }
void UMMOAttributeSet::OnRep_EquipLoad(const FGameplayAttributeData OldEquipLoad) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, EquipLoad, OldEquipLoad); }

// Damage Negation
void UMMOAttributeSet::OnRep_Negation_Standard(const FGameplayAttributeData OldNegation_Standard) const						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Standard, OldNegation_Standard); }
void UMMOAttributeSet::OnRep_Negation_Slash(const FGameplayAttributeData OldNegation_Slash) const							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Slash, OldNegation_Slash); }
void UMMOAttributeSet::OnRep_Negation_Pierce(const FGameplayAttributeData OldNegation_Pierce) const 						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Pierce, OldNegation_Pierce); }
void UMMOAttributeSet::OnRep_Negation_Strike(const FGameplayAttributeData OldNegation_Strike) const 						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Strike, OldNegation_Strike); }
void UMMOAttributeSet::OnRep_Negation_Magic(const FGameplayAttributeData OldNegation_Magic) const							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Magic, OldNegation_Magic); }
void UMMOAttributeSet::OnRep_Negation_Ice(const FGameplayAttributeData OldNegation_Ice) const								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Ice, OldNegation_Ice); }
void UMMOAttributeSet::OnRep_Negation_Fire(const FGameplayAttributeData OldNegation_Fire) const								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Fire, OldNegation_Fire); }
void UMMOAttributeSet::OnRep_Negation_Holy(const FGameplayAttributeData OldNegation_Holy) const								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Holy, OldNegation_Holy); }
void UMMOAttributeSet::OnRep_Negation_Lightning(const FGameplayAttributeData OldNegation_Lightning) const					{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Negation_Lightning, OldNegation_Lightning); }


// Meta Attributes
void UMMOAttributeSet::OnRep_Damage_Standard(const FGameplayAttributeData OldDamage_Standard) const 						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Standard, OldDamage_Standard); }
void UMMOAttributeSet::OnRep_Damage_Slash(const FGameplayAttributeData OldDamage_Slash) const 								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Slash, OldDamage_Slash); }
void UMMOAttributeSet::OnRep_Damage_Pierce(const FGameplayAttributeData OldDamage_Pierce) const 							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Pierce, OldDamage_Pierce); }
void UMMOAttributeSet::OnRep_Damage_Strike(const FGameplayAttributeData OldDamage_Strike) const 							{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Strike, OldDamage_Strike); }
void UMMOAttributeSet::OnRep_Damage_Magic(const FGameplayAttributeData OldDamage_Magic) const 								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Magic, OldDamage_Magic); }
void UMMOAttributeSet::OnRep_Damage_Ice(const FGameplayAttributeData OldDamage_Ice) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Ice, OldDamage_Ice); }
void UMMOAttributeSet::OnRep_Damage_Fire(const FGameplayAttributeData OldDamage_Fire) const 								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Fire, OldDamage_Fire); }
void UMMOAttributeSet::OnRep_Damage_Holy(const FGameplayAttributeData OldDamage_Holy) const 								{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Holy, OldDamage_Holy); }
void UMMOAttributeSet::OnRep_Damage_Lightning(const FGameplayAttributeData OldDamage_Lightning) const 						{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Damage_Lightning, OldDamage_Lightning); }
void UMMOAttributeSet::OnRep_DamageCalculation(const FGameplayAttributeData OldDamageCalculation) const 					{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, DamageCalculation, OldDamageCalculation); }
void UMMOAttributeSet::OnRep_Frostbite(const FGameplayAttributeData OldFrostbite) const 									{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Frostbite, OldFrostbite); }
void UMMOAttributeSet::OnRep_Madness(const FGameplayAttributeData OldMadness) const 										{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Madness, OldMadness); }
void UMMOAttributeSet::OnRep_Sleep(const FGameplayAttributeData OldSleep) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Sleep, OldSleep); }
void UMMOAttributeSet::OnRep_Bleed(const FGameplayAttributeData OldBleed) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Bleed, OldBleed); }
void UMMOAttributeSet::OnRep_Poison(const FGameplayAttributeData OldPoison) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Poison, OldPoison); }
void UMMOAttributeSet::OnRep_Curse(const FGameplayAttributeData OldCurse) const 											{ GAMEPLAYATTRIBUTE_REPNOTIFY(UMMOAttributeSet, Curse, OldCurse); }
