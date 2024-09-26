// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageCalculation_Default.h"

#include "AbilitySystemComponent.h"


// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct FDamageStatics
{
	// Calculations based on target attributes
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxMana);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxStamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxPoise);

	// Damage calculations based on the source's attributes
	DECLARE_ATTRIBUTE_CAPTUREDEF(Atr_Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Atr_Endurance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Atr_Mind);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Atr_Dexterity);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Atr_Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Atr_Intelligence);

	// Calculations for damage reduction and amplification based on target attributes
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poise);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalArmor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalArmor);
	
	FDamageStatics()
	{
		// Snapshot happens at time of GESpec creation
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, MaxHealth, Target, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, MaxMana, Target, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, MaxStamina, Target, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, MaxPoise, Target, true);
		//
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, Poise, Target, false);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, PhysicalArmor, Target, false);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, MagicalArmor, Target, false);
		//
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, Atr_Health, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, Atr_Endurance, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, Atr_Mind, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, Atr_Dexterity, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, Atr_Strength, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSet, Atr_Intelligence, Source, true);
	}
};


static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DStatics;
	return DStatics;
}


UDamageCalculation_Default::UDamageCalculation_Default()
{
	// Capture the relevant attributes to run your execution calculations
	RelevantAttributesToCapture.Add(DamageStatics().MaxHealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().MaxManaDef);
	RelevantAttributesToCapture.Add(DamageStatics().MaxStaminaDef);
	RelevantAttributesToCapture.Add(DamageStatics().MaxPoiseDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().Atr_HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().Atr_EnduranceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Atr_MindDef);
	RelevantAttributesToCapture.Add(DamageStatics().Atr_DexterityDef);
	RelevantAttributesToCapture.Add(DamageStatics().Atr_StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().Atr_IntelligenceDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().PoiseDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalArmorDef);
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

	//----------------------------------------------------------------------------------------------//
	// Save the attribute information																//
	//----------------------------------------------------------------------------------------------//
	float MitigatedDamage = 0.0f;
	float PoiseDamage = 0.0f;
	float Damage = 0.0f;
	float RawDamage = 0.0f;
	
	float MaxHealth = 0.0f;
	float MaxMana = 0.0f;
	float MaxStamina = 0.0f;
	float MaxPoise = 0.0f;
	
	float Poise = 0.0f; // USanboxAscLibrary::GetPoiseDamage(EffectContextHandle);
	float PhysicalArmor = 0.0f;
	float MagicalArmor = 0.0f;
	
	// Get the tag data for this specific attack
	Damage += FMath::Max<float>(Spec->GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("IncomingPhysicalDamageTag")), false, -1.0f), 0.0f);
	RawDamage = Damage;
	
	// Get the captured attribute information for the source and target
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MaxHealthDef, EvaluationParameters, MaxHealth); 
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MaxManaDef, EvaluationParameters, MaxMana); 
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MaxStaminaDef, EvaluationParameters, MaxStamina); 
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MaxPoiseDef, EvaluationParameters, MaxPoise);
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PoiseDef, EvaluationParameters, Poise); 
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalArmorDef, EvaluationParameters, PhysicalArmor); 
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalArmorDef, EvaluationParameters, MagicalArmor);
	
	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation

	
	//----------------------------------------------------------------------------------------------//
	// Damage Calculations																			//
	//----------------------------------------------------------------------------------------------//
	// Armor mitigation // Set the Target's damage meta attribute
	MitigatedDamage = (RawDamage) * (100 / (100 + PhysicalArmor));

	// The only values we set here are persisted information based on the target attributes, everything else is done while prepping the exec calc
	// USanboxAscLibrary::SetPhysicalArmor(EffectContextHandle, PhysicalArmor);
	// USanboxAscLibrary::SetMagicalArmor(EffectContextHandle, MagicalArmor);

	// This information is captured for debugging purposes. There may be frame and replication differences between the target's tags for whether they're blocking and parrying, and I'd like to know what's happening here
	// So far I haven't had any complications with this, but I haven't done a lot of multiplayer testing with client prediction yet
	// if (TargetTags->HasTag(FGameplayTag::RequestGameplayTag(Tag_State_Blocking))) USanboxAscLibrary::SetBlockedAttack(EffectContextHandle, true);
	// if (TargetTags->HasTag(FGameplayTag::RequestGameplayTag(Tag_State_Parrying)))
	// {
	// 	if (TargetTags->HasTag(FGameplayTag::RequestGameplayTag(Tag_State_Parrying_Perfect))) USanboxAscLibrary::SetPerfectParriedAttack(EffectContextHandle, true);
	// 	else USanboxAscLibrary::SetParriedAttack(EffectContextHandle, true);
	// }
	
	
	// UE_LOGFMT(AbilityLog, Warning, "{0} {1}() RawDamage: {2}, MitigatedDamage: {3}, PoiseDamage: {4} calculated and dealt to {5}",
	// 	*FString(__FUNCTION__), *GetNameSafe(SourceActor),
	// 	RawDamage, MitigatedDamage, PoiseDamage,
	// 	*GetNameSafe(TargetActor)
	// );
	
	// Everything else related to damage gets passed along with the effect context handle, and is tied to this attribute
	if (MitigatedDamage > 0.f)
	{
		// const FGameplayModifierEvaluatedData EvaluatedData(UBaseAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, MitigatedDamage);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData());
	}
}

