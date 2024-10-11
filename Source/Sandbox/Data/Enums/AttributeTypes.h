#pragma once


#include "CoreMinimal.h"
#include "AttributeTypes.generated.h"



/**
 * A list of the combat attributes as an enum type
 */
UENUM(BlueprintType)
enum class ECombatAttribute : uint8
{
	None,
	Health,
	Mana,
	Stamina,
	Poise,

	PoisonBuildup,
	BleedBuildup,
	FrostbiteBuildup,
	SleepBuildup,
	MadnessBuildup,
	CurseBuildup,

	Defence_Standard,
	Defence_Slash,
	Defence_Pierce,
	Defence_Strike,
	Resistance_Magic,
	Resistance_Ice,
	Resistance_Fire,
	Resistance_Holy,
	Resistance_Lightning,
	Immunity,
	Robustness,
	Focus,

	Negation_Standard,
	Negation_Slash,
	Negation_Pierce,
	Negation_Strike,
	Negation_Magic,
	Negation_Ice,
	Negation_Fire,
	Negation_Holy,
	Negation_Lightning,

	Damage_Standard,
	Damage_Slash,
	Damage_Pierce,
	Damage_Strike,
	Damage_Magic,
	Damage_Ice,
	Damage_Fire,
	Damage_Holy,
	Damage_Lightning,
	Curse,
	Bleed,
	Poison,
	Frostbite,
	Madness,
	Sleep,
};


/**
 *	A list of the attributes as an enum type 
 */
UENUM(BlueprintType)
enum class EPlayerAttributes : uint8
{
	None,
	Health,
	MaxHealth,
	HealthRegenRate,
	Mana,
	MaxMana,
	ManaRegenRate,
	Stamina,
	MaxStamina,
	StaminaRegenRate,
	Poise,
	MaxPoise,
	PoiseRegenRate,

	PoisonBuildup,
	MaxPoisonBuildup,
	BleedBuildup,
	MaxBleedBuildup,
	FrostbiteBuildup,
	MaxFrostbiteBuildup,
	SleepBuildup,
	MaxSleepBuildup,
	MadnessBuildup,
	MaxMadnessBuildup,
	CurseBuildup,
	MaxCurseBuildup,

	Vitality,
	Endurance,
	Mind,
	Intelligence,
	Faith,
	Dexterity,
	Strength,
	Arcane,
	Experience,

	Defence_Standard,
	Defence_Slash,
	Defence_Pierce,
	Defence_Strike,
	Resistance_Magic,
	Resistance_Ice,
	Resistance_Fire,
	Resistance_Holy,
	Resistance_Lightning,
	Immunity,
	Robustness,
	Focus,

	Discovery,
	Spells,
	Weight,
	EquipLoad,

	Negation_Standard,
	Negation_Slash,
	Negation_Pierce,
	Negation_Strike,
	Negation_Magic,
	Negation_Ice,
	Negation_Fire,
	Negation_Holy,
	Negation_Lightning,

	Damage_Standard,
	Damage_Slash,
	Damage_Pierce,
	Damage_Strike,
	Damage_Magic,
	Damage_Ice,
	Damage_Fire,
	Damage_Holy,
	Damage_Lightning,
	Bleed,
	Poison,
	Curse,
};
