// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageCalculation_Default.generated.h"


/**
 * The attacker's damage information used during the calculation
 *
 * https://steamcommunity.com/sharedfiles/filedetails/?id=3155117973
 */
USTRUCT()
struct FDamageCalculations_AttackInformation
{
	GENERATED_BODY()

	/** The standard physical damage */
	UPROPERTY() float Damage_Standard;
	
	/** The physical slash damage */
	UPROPERTY() float Damage_Slash;
	
	/** The physical pierce damage */
	UPROPERTY() float Damage_Pierce;
	
	/** The physical strike damage */
	UPROPERTY() float Damage_Strike;
	
	/** The magical damage */
	UPROPERTY() float Damage_Magic;
	
	/** The ice damage */
	UPROPERTY() float Damage_Ice;
	
	/** The fire damage */
	UPROPERTY() float Damage_Fire;
	
	/** The holy  damage */
	UPROPERTY() float Damage_Holy;
	
	/** The lightning damage */
	UPROPERTY() float Damage_Lightning;
	
	/** The bleed damage */
	UPROPERTY() float Bleed;

	/** The frostbite damage */
	UPROPERTY() float Frostbite;
	
	/** The poison damage */
	UPROPERTY() float Poison;
	
	/** The curse damage */
	UPROPERTY() float Curse;
	
	/** The madness damage */
	UPROPERTY() float Madness;
	
	/** The sleep damage */
	UPROPERTY() float Sleep;
};


/**
 * The target's current status and defences used during the calculation
 */
USTRUCT()
struct FDamageCalculations_Attributes_Target
{
	GENERATED_BODY()
	
	/** The target's current health */
	UPROPERTY() float Health;
	/** The target's current mana */
	UPROPERTY() float Mana;
	/** The target's current stamina */
	UPROPERTY() float Stamina;
	/** The target's current poise*/
	UPROPERTY() float Poise;
	
	/** The target's current poison buildup */
	UPROPERTY() float PoisonBuildup;
	/** The target's current bleed  buildup */
	UPROPERTY() float BleedBuildup;
	/** The target's current frostbite buildup */
	UPROPERTY() float FrostbiteBuildup;
	/** The target's current sleep buildup */
	UPROPERTY() float SleepBuildup;
	/** The target's current madness buildup */
	UPROPERTY() float MadnessBuildup;
	/** The target's current curse buildup */
	UPROPERTY() float CurseBuildup;
	
	/** The player's defence against standard physical attacks */
	UPROPERTY() float Defence_Standard;
	/** The player's defence against slashing physical attacks */
	UPROPERTY() float Defence_Slash;
	/** The player's defence against piercing physical attacks */
	UPROPERTY() float Defence_Pierce;
	/** The player's defence against striking physical attacks */
	UPROPERTY() float Defence_Strike;
	
	/** The player's resistance against magical attacks */
	UPROPERTY() float Resistance_Magic;
	/** The player's resistance against ice attacks */
	UPROPERTY() float Resistance_Ice;
	/** The player's resistance against fire attacks */
	UPROPERTY() float Resistance_Fire;
	/** The player's resistance against holy attacks */
	UPROPERTY() float Resistance_Holy;
	/** The player's resistance against lightning attacks */
	UPROPERTY() float Resistance_Lightning;
	
	/** The player's resistance against poison */
	UPROPERTY() float Immunity;
	/** The player's resistance against bleed and frostbite */
	UPROPERTY() float Robustness;
	/** The player's resistance against sleep and madness */
	UPROPERTY() float Focus;
	
	/** The player's damage negation against standard physical attacks */
	UPROPERTY() float Negation_Standard;
	/** The player's damage negation against slashing physical attacks */
	UPROPERTY() float Negation_Slash;
	/** The player's damage negation against piercing physical attacks */
	UPROPERTY() float Negation_Pierce;
	/** The player's damage negation against striking physical attacks */
	UPROPERTY() float Negation_Strike;
	/** The player's damage negation against magical attacks */
	UPROPERTY() float Negation_Magic;
	/** The player's damage negation against ice attacks */
	UPROPERTY() float Negation_Ice;
	/** The player's damage negation against fire attacks */
	UPROPERTY() float Negation_Fire;
	/** The player's damage negation against holy attacks */
	UPROPERTY() float Negation_Holy;
	/** The player's damage negation against lightning attacks */
	UPROPERTY() float Negation_Lightning;
};


/**
 * The player and target's attributes used during the damage calculations
 */
USTRUCT()
struct FDamageCalculation_Attributes
{
	GENERATED_BODY()

	UPROPERTY() FDamageCalculations_AttackInformation AttackInformation;
	UPROPERTY() FDamageCalculations_Attributes_Target TargetAttributes;
};


/**
 * 
 */
UCLASS()
class SANDBOX_API UDamageCalculation_Default : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDamageCalculation_Default();

	/** Handles damage calculations against a target */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	/** Retrieves the relevant attributes to capture for calculations */
	virtual void CalculateCapturedRelevantAttributes(const FAggregatorEvaluateParameters& EvaluationParameters, const FGameplayEffectCustomExecutionParameters& ExecutionParams, FDamageCalculation_Attributes& AttributeInformation) const;

	/** Handles the damage calculation for any specific attack */
	virtual float DamageCalculation(float IncomingDamage, float Defence, float DamageNegation) const;

	/** Handles the status damage calculation for any specific attack */
	virtual float StatusCalculation(float StatusDamage, float Resistance, float StatusNegation) const;

	
};
