// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageCalculation_Default.h"

#include "AbilitySystemComponent.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"


// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct FDamageStatics
{
	/**** Damage info ****/
	// Physical
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Standard);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Slash);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Pierce);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Strike);

	// Magic
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Magic);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Ice);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Fire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Holy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage_Lightning);

	// Statuses
	DECLARE_ATTRIBUTE_CAPTUREDEF(Bleed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poison);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Curse);

	
	/**** Target's attributes ****/
	// Health, mana, etc.
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mana);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Stamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poise);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoisonBuildup);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BleedBuildup);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FrostbiteBuildup);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SleepBuildup);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MadnessBuildup);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurseBuildup);

	// Defences/Resistances
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence_Standard);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence_Slash);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence_Pierce);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence_Strike);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Magic);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Ice);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Fire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Holy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Lightning);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Immunity);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Robustness);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Focus);

	// Damage megatrons
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Standard);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Slash);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Pierce);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Strike);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Magic);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Ice);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Fire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Holy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Negation_Lightning);
	
	// Snapshot happens at time of GESpec creation
	FDamageStatics()
	{
		// Target information
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Health, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Mana, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Stamina, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Poise, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, PoisonBuildup, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, BleedBuildup, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, FrostbiteBuildup, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, SleepBuildup, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, MadnessBuildup, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, CurseBuildup, Target, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Defence_Standard, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Defence_Slash, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Defence_Pierce, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Defence_Strike, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Resistance_Magic, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Resistance_Ice, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Resistance_Fire, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Resistance_Holy, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Resistance_Lightning, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Immunity, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Robustness, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Focus, Target, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Standard, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Slash, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Pierce, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Strike, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Magic, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Ice, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Fire, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Holy, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Negation_Lightning, Target, false);


		// Damage information
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Standard, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Slash, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Pierce, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Strike, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Magic, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Ice, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Fire, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Holy, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Damage_Lightning, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Bleed, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Poison, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMOAttributeSet, Curse, Source, true);
	}
};


static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DStatics;
	return DStatics;
}


UDamageCalculation_Default::UDamageCalculation_Default()
{
	// Target information
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().ManaDef);
	RelevantAttributesToCapture.Add(DamageStatics().StaminaDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoiseDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoisonBuildupDef);
	RelevantAttributesToCapture.Add(DamageStatics().BleedBuildupDef);
	RelevantAttributesToCapture.Add(DamageStatics().FrostbiteBuildupDef);
	RelevantAttributesToCapture.Add(DamageStatics().SleepBuildupDef);
	RelevantAttributesToCapture.Add(DamageStatics().MadnessBuildupDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurseBuildupDef);

	RelevantAttributesToCapture.Add(DamageStatics().Defence_StandardDef);
	RelevantAttributesToCapture.Add(DamageStatics().Defence_SlashDef);
	RelevantAttributesToCapture.Add(DamageStatics().Defence_PierceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Defence_StrikeDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_IceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_FireDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_HolyDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_LightningDef);
	RelevantAttributesToCapture.Add(DamageStatics().ImmunityDef);
	RelevantAttributesToCapture.Add(DamageStatics().RobustnessDef);
	RelevantAttributesToCapture.Add(DamageStatics().FocusDef);

	RelevantAttributesToCapture.Add(DamageStatics().Negation_StandardDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_SlashDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_PierceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_StrikeDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_MagicDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_IceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_FireDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_HolyDef);
	RelevantAttributesToCapture.Add(DamageStatics().Negation_LightningDef);


	// Damage information
	RelevantAttributesToCapture.Add(DamageStatics().Damage_StandardDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_SlashDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_PierceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_StrikeDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_MagicDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_IceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_FireDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_HolyDef);
	RelevantAttributesToCapture.Add(DamageStatics().Damage_LightningDef);
	RelevantAttributesToCapture.Add(DamageStatics().BleedDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoisonDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurseDef);
}


void UDamageCalculation_Default::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* TargetAsc = ExecutionParams.GetTargetAbilitySystemComponent();
	const UAbilitySystemComponent* SourceAsc = ExecutionParams.GetSourceAbilitySystemComponent();
	const AActor* SourceActor = SourceAsc ? SourceAsc->GetAvatarActor() : nullptr;
	const AActor* TargetActor = TargetAsc ? TargetAsc->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec* Spec = ExecutionParams.GetOwningSpecForPreExecuteMod();
	FGameplayEffectContextHandle EffectContextHandle = Spec->GetContext();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec->CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec->CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Create your own combat calculations and configurations using this as a reference. I'd create individual execution calculations for different types of combat, or different kinds of attacks
	// This isn't something you need to handle right now, however the idea is use meta attributes for capturing damage from the player, and then capture the target's attributes and use it for calculations
	// Once you've captured the attributes you just add the output to a gameplay effect and this gets calculated on the target's attribute. If you need any additional info just attach it to a custom gameplay effect
	
	
	// Retrieve the attribute information
	FDamageCalculation_Attributes AttributeInformation;
	CalculateCapturedRelevantAttributes(EvaluationParameters, ExecutionParams, AttributeInformation);
	
	const FDamageCalculations_AttackInformation& AttackInfo = AttributeInformation.AttackInformation;
	const FDamageCalculations_Attributes_Target& PlayerStats = AttributeInformation.TargetAttributes;

	// Retrieve any information from gameplay effects
	

	
	
	//----------------------------------------------------------------------------------------------//
	// Damage Calculations																			//
	//----------------------------------------------------------------------------------------------//
	FDamageCalculations_AttackInformation CalculatedDamage;

	/**
		Weapon damage calculations (calculated before the execution calculation) -> needs server validation to be safe

			Player
				- Weapon stats
				- Weapon attribute scaling
				- Motion values (multipliers specific to motion -> multi hit, charged attacks, etc)
				- additional multipliers from equipment

			Enemies
				- Weapon stats
				- Weapon attribute scaling
				- Motion values (multipliers specific to motion -> multi hit, charged attacks, etc)
				- additonal multipliers from equipment
				- enemy level (NG+)
				- area level


		Armor calculations (factored in during the damage calculation)

			Player
				- Armor stats (Should armor scale with attributes or level?) 
				- damage negation

			Enemies
				- Armor stats
				- new game leveling
				- area level
				- damage negation


		// Just be careful that armor scales evenly with different attacks, and find a good way to add armor and damage reduction based on attributes/equipment
		// The primary attack by itself, when left to expire, detonates for 140 magic damage.
		// However, we're dealing with an entirely different beast here, because this number is not a motion value, but rather an instance of Flat Magic Damage.
		// The follow-up R2 has a flat damage modifier of 200 and the bullets from the R1 each deal a flat magic damage of 150.
		// The reason this is so important is because this vastly affects how the unique skill scales with certain stats.
		// Attack motion values are multiplicative and inherently tied to the overall damage of the weapon itself.
		// Leveling the appropriate stat still makes the skill stronger, sure, but that's only because scaling affects the entire weapon and not just the skill.
		// If a skill has something that is represented by a flat damage value in one of these categories here, that means it's independent from the weapon's damage and receives that much more power from leveling the appropriate stat.
		// The damage of Flame Ignition is almost entirely made up of flat damage numbers, meaning a very large chunk of its potential power will be determined by your character's intelligence.


	*/

	

	// damage - defence stats // remaining damage * damage negation
	CalculatedDamage.Damage_Standard = DamageCalculation(AttackInfo.Damage_Standard, PlayerStats.Defence_Standard, PlayerStats.Negation_Standard);
	CalculatedDamage.Damage_Slash = DamageCalculation(AttackInfo.Damage_Slash, PlayerStats.Defence_Slash, PlayerStats.Negation_Slash);
	CalculatedDamage.Damage_Pierce = DamageCalculation(AttackInfo.Damage_Pierce, PlayerStats.Defence_Pierce, PlayerStats.Negation_Pierce);
	CalculatedDamage.Damage_Strike = DamageCalculation(AttackInfo.Damage_Strike, PlayerStats.Defence_Strike, PlayerStats.Negation_Strike);
	
	CalculatedDamage.Damage_Magic = DamageCalculation(AttackInfo.Damage_Magic, PlayerStats.Resistance_Magic, PlayerStats.Negation_Magic);
	CalculatedDamage.Damage_Ice = DamageCalculation(AttackInfo.Damage_Ice, PlayerStats.Resistance_Ice, PlayerStats.Negation_Ice);
	CalculatedDamage.Damage_Fire = DamageCalculation(AttackInfo.Damage_Fire, PlayerStats.Resistance_Fire, PlayerStats.Negation_Fire);
	CalculatedDamage.Damage_Holy = DamageCalculation(AttackInfo.Damage_Holy, PlayerStats.Resistance_Holy, PlayerStats.Negation_Holy);
	CalculatedDamage.Damage_Lightning = DamageCalculation(AttackInfo.Damage_Lightning, PlayerStats.Resistance_Lightning, PlayerStats.Negation_Lightning);
	
	CalculatedDamage.Curse = StatusCalculation(AttackInfo.Curse, PlayerStats.Immunity, 0);
	CalculatedDamage.Bleed = StatusCalculation(AttackInfo.Bleed, PlayerStats.Robustness, 0);
	CalculatedDamage.Frostbite = StatusCalculation(AttackInfo.Frostbite, PlayerStats.Robustness, 0);
	CalculatedDamage.Poison = StatusCalculation(AttackInfo.Poison, PlayerStats.Immunity, 0);
	CalculatedDamage.Madness = StatusCalculation(AttackInfo.Madness, PlayerStats.Focus, 0);
	CalculatedDamage.Sleep = StatusCalculation(AttackInfo.Sleep, PlayerStats.Focus, 0);
	

	
	// If there's anything we need to add to the gameplay effect before the attribute handles it, here is the place to handle it

	
	// Gameplay effect executes handle attributes individually, and I still need to learn how it handles this properly


	// Add the calculated damages to output modifications
	const FGameplayModifierEvaluatedData Eval_Damage_Standard(UMMOAttributeSet::GetDamage_StandardAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Standard);
	const FGameplayModifierEvaluatedData Eval_Damage_Slash(UMMOAttributeSet::GetDamage_SlashAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Slash);
	const FGameplayModifierEvaluatedData Eval_Damage_Pierce(UMMOAttributeSet::GetDamage_PierceAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Pierce);
	const FGameplayModifierEvaluatedData Eval_Damage_Strike(UMMOAttributeSet::GetDamage_StrikeAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Strike);
				
	const FGameplayModifierEvaluatedData Eval_Damage_Magic(UMMOAttributeSet::GetDamage_MagicAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Magic);
	const FGameplayModifierEvaluatedData Eval_Damage_Ice(UMMOAttributeSet::GetDamage_IceAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Ice);
	const FGameplayModifierEvaluatedData Eval_Damage_Fire(UMMOAttributeSet::GetDamage_FireAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Fire);
	const FGameplayModifierEvaluatedData Eval_Damage_Holy(UMMOAttributeSet::GetDamage_HolyAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Holy);
	const FGameplayModifierEvaluatedData Eval_Damage_Lightning(UMMOAttributeSet::GetDamage_LightningAttribute(), EGameplayModOp::Override, CalculatedDamage.Damage_Lightning);
				
	const FGameplayModifierEvaluatedData Eval_Curse(UMMOAttributeSet::GetCurseAttribute(), EGameplayModOp::Override, CalculatedDamage.Curse);
	const FGameplayModifierEvaluatedData Eval_Bleed(UMMOAttributeSet::GetBleedAttribute(), EGameplayModOp::Override, CalculatedDamage.Bleed);
	const FGameplayModifierEvaluatedData Eval_Frostbite(UMMOAttributeSet::GetFrostbiteAttribute(), EGameplayModOp::Override, CalculatedDamage.Frostbite);
	const FGameplayModifierEvaluatedData Eval_Poison(UMMOAttributeSet::GetPoisonAttribute(), EGameplayModOp::Override, CalculatedDamage.Poison);
	const FGameplayModifierEvaluatedData Eval_Madness(UMMOAttributeSet::GetMadnessAttribute(), EGameplayModOp::Override, CalculatedDamage.Madness);
	const FGameplayModifierEvaluatedData Eval_Sleep(UMMOAttributeSet::GetSleepAttribute(), EGameplayModOp::Override, CalculatedDamage.Sleep);

	// Each calculation is done individually. The effect context handles these in the same order they're created
	// They however, use the same effect context which might be helpful for how you handle calculations
	/* if (CalculatedDamage.Damage_Standard != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Standard);
	/* if (CalculatedDamage.Damage_Slash != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Slash);
	/* if (CalculatedDamage.Damage_Pierce != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Pierce);
	/* if (CalculatedDamage.Damage_Strike != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Strike);
	/* if (CalculatedDamage.Damage_Magic != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Magic);
	/* if (CalculatedDamage.Damage_Ice != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Ice);
	/* if (CalculatedDamage.Damage_Fire != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Fire);
	/* if (CalculatedDamage.Damage_Holy != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Holy);
	/* if (CalculatedDamage.Damage_Lightning != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Damage_Lightning);
	/* if (CalculatedDamage.Curse != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Curse);
	/* if (CalculatedDamage.Bleed != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Bleed);
	/* if (CalculatedDamage.Frostbite != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Frostbite);
	/* if (CalculatedDamage.Poison != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Poison);
	/* if (CalculatedDamage.Madness != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Madness);
	/* if (CalculatedDamage.Sleep != 0) */ OutExecutionOutput.AddOutputModifier(Eval_Sleep);
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMMOAttributeSet::GetDamageCalculationAttribute(), EGameplayModOp::Multiplicitive, 1));
}


void UDamageCalculation_Default::CalculateCapturedRelevantAttributes(
	const FAggregatorEvaluateParameters& EvaluationParameters,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FDamageCalculation_Attributes& AttributeInformation) const
{
	
	// Target information
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDef, EvaluationParameters, AttributeInformation.TargetAttributes.Health);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ManaDef, EvaluationParameters, AttributeInformation.TargetAttributes.Mana);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StaminaDef, EvaluationParameters, AttributeInformation.TargetAttributes.Stamina);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PoiseDef, EvaluationParameters, AttributeInformation.TargetAttributes.Poise);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PoisonBuildupDef, EvaluationParameters, AttributeInformation.TargetAttributes.PoisonBuildup);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BleedBuildupDef, EvaluationParameters, AttributeInformation.TargetAttributes.BleedBuildup);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().FrostbiteBuildupDef, EvaluationParameters, AttributeInformation.TargetAttributes.FrostbiteBuildup);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().SleepBuildupDef, EvaluationParameters, AttributeInformation.TargetAttributes.SleepBuildup);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MadnessBuildupDef, EvaluationParameters, AttributeInformation.TargetAttributes.MadnessBuildup);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurseBuildupDef, EvaluationParameters, AttributeInformation.TargetAttributes.CurseBuildup);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Defence_StandardDef, EvaluationParameters, AttributeInformation.TargetAttributes.Defence_Standard);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Defence_SlashDef, EvaluationParameters, AttributeInformation.TargetAttributes.Defence_Slash);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Defence_PierceDef, EvaluationParameters, AttributeInformation.TargetAttributes.Defence_Pierce);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Defence_StrikeDef, EvaluationParameters, AttributeInformation.TargetAttributes.Defence_Strike);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Resistance_MagicDef, EvaluationParameters, AttributeInformation.TargetAttributes.Resistance_Magic);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Resistance_IceDef, EvaluationParameters, AttributeInformation.TargetAttributes.Resistance_Ice);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Resistance_FireDef, EvaluationParameters, AttributeInformation.TargetAttributes.Resistance_Fire);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Resistance_HolyDef, EvaluationParameters, AttributeInformation.TargetAttributes.Resistance_Holy);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Resistance_LightningDef, EvaluationParameters, AttributeInformation.TargetAttributes.Resistance_Lightning);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ImmunityDef, EvaluationParameters, AttributeInformation.TargetAttributes.Immunity);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().RobustnessDef, EvaluationParameters, AttributeInformation.TargetAttributes.Robustness);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().FocusDef, EvaluationParameters, AttributeInformation.TargetAttributes.Focus);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_StandardDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Standard);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_SlashDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Slash);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_PierceDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Pierce);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_StrikeDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Strike);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_MagicDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Magic);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_IceDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Ice);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_FireDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Fire);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_HolyDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Holy);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Negation_LightningDef, EvaluationParameters, AttributeInformation.TargetAttributes.Negation_Lightning);


	// Damage information
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_StandardDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Standard);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_SlashDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Slash);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_PierceDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Pierce);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_StrikeDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Strike);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_MagicDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Magic);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_IceDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Ice);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_FireDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Fire);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_HolyDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Holy);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Damage_LightningDef, EvaluationParameters, AttributeInformation.AttackInformation.Damage_Lightning);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BleedDef, EvaluationParameters, AttributeInformation.AttackInformation.Bleed);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PoisonDef, EvaluationParameters, AttributeInformation.AttackInformation.Poison);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurseDef, EvaluationParameters, AttributeInformation.AttackInformation.Curse);
}


float UDamageCalculation_Default::DamageCalculation(const float IncomingDamage, const float Defence, const float DamageNegation) const
{
	float DamageAfterArmor = FMath::Clamp(IncomingDamage - Defence, 0, IncomingDamage);
	float DamageNegationCalc = (100 - DamageNegation) * 0.01;
	float MitigatedDamage = DamageAfterArmor * DamageNegationCalc;

	// UE_LOGFMT(LogTemp, Log, "DamageAfterArmor: {0}, DamageNegationCalc: {1}, MitigatedDamage: {3}", DamageAfterArmor, DamageNegationCalc, MitigatedDamage);
	return MitigatedDamage;
}


float UDamageCalculation_Default::StatusCalculation(const float StatusDamage, const float Resistance, const float StatusNegation) const
{
	return FMath::Clamp(StatusDamage - Resistance, 0, StatusDamage) * ((100 - StatusNegation) / 100); 
}

