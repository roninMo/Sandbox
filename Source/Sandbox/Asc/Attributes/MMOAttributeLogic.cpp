// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/MMOAttributeLogic.h"

#include "GameplayEffectExtension.h"
#include "Logging/StructuredLog.h"


bool UMMOAttributeLogic::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	/**
	 *  This should apply 'gamewide' rules. Such as clamping Health to MaxHealth or granting +3 health for every point of strength, etc
	 *	PreAttributeModify can return false to 'throw out' this modification.
	 */
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	return true;
}


void UMMOAttributeLogic::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Any clamping that happens here does not permanently change the modifier on the ASC. It only changes the value returned from querying the modifier.
	// This means anything that recalculates the CurrentValue from all of the modifiers like GameplayEffectExecutionCalculations and ModifierMagnitudeCalculations need to implement clamping again.
	Super::PreAttributeChange(Attribute, NewValue);
	AttributeClamping(Attribute, NewValue, false); // TODO: add attribute clamping for both instant effect and duration, and find out where to handle it
}


void UMMOAttributeLogic::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	FGAttributeSetExecutionData Props;
	/**
		Apply buildups / damages / poise damages, then the hit react, and handle any other logic after that

		Handling individual calculations isn't as complex as having everything together, there's a lot more of interesting things and calculations with multiple attributes.
		You just need to clear out the old attributes and help handle some of the basic values for handling damage

			https://fextralife.com/forums/t52683/poise-explanation-general-pvp-tips
			https://www.reddit.com/r/Eldenring/comments/pjemv1/which_poise_system_do_you_prefer_the_most_and/
			

	*/

	// Retrieve the player and target's information, and handle attribute clamping during damage calculations
	GetExecutionData(Data, Props);
	AttributeClamping(Data.EvaluatedData.Attribute, Data.EvaluatedData.Magnitude);
	
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		UE_LOGFMT(LogTemp, Log, "{0}::{1}() Stamina attribute adjustment: {2}!", *UEnum::GetValueAsString(Props.SourceActor->GetLocalRole()), *FString(__FUNCTION__), GetStamina());
	}


	
	const TArray<FGameplayEffectModifiedAttribute>& DamageAttributes = Data.EffectSpec.ModifiedAttributes;
	

	
	/**

		Status calculations
			- status buildup
				- Status effect (Take damage / slow / poison)
				- Hit reactionz
	*/
	if (Data.EvaluatedData.Attribute == GetCurseAttribute() ||
		Data.EvaluatedData.Attribute == GetBleedAttribute() ||
		Data.EvaluatedData.Attribute == GetFrostbiteAttribute() ||
		Data.EvaluatedData.Attribute == GetPoisonAttribute() ||
		Data.EvaluatedData.Attribute == GetMadnessAttribute() ||
		Data.EvaluatedData.Attribute == GetSleepAttribute())
	{

		if (Data.EvaluatedData.Attribute == GetCurseAttribute())
		{
			float Buildup = GetCurseBuildup() + Data.EvaluatedData.Magnitude;
			SetCurseBuildup(FMath::Clamp(GetCurseBuildup(), 0, Buildup));
		}

		if (Data.EvaluatedData.Attribute == GetBleedAttribute())
		{
			float Buildup = GetBleedBuildup() + Data.EvaluatedData.Magnitude;
			SetBleedBuildup(FMath::Clamp(GetBleedBuildup(), 0, Buildup));
		}

		if (Data.EvaluatedData.Attribute == GetFrostbiteAttribute())
		{
			float Buildup = GetFrostbiteBuildup() + Data.EvaluatedData.Magnitude;
			SetFrostbiteBuildup(FMath::Clamp(GetFrostbiteBuildup(), 0, Buildup));
		}
		
		if (Data.EvaluatedData.Attribute == GetPoisonAttribute())
		{
			float Buildup = GetPoisonBuildup() + Data.EvaluatedData.Magnitude;
			SetPoisonBuildup(FMath::Clamp(GetPoisonBuildup(), 0, Buildup));
		}
		
		if (Data.EvaluatedData.Attribute == GetPoisonAttribute())
		{
			float Buildup = GetMadnessBuildup() + Data.EvaluatedData.Magnitude;
			SetMadnessBuildup(FMath::Clamp(GetMadnessBuildup(), 0, Buildup));
		}
		
		if (Data.EvaluatedData.Attribute == GetPoisonAttribute())
		{
			float Buildup = GetSleepBuildup() + Data.EvaluatedData.Magnitude;
			SetSleepBuildup(FMath::Clamp(GetSleepBuildup(), 0, Buildup));
		}
	}

	
	/**
		Damage calculations
			- Take damage
			- Hit reaction based on the attack
			- Handle taking damage / dying
	*/
	float MagicDamageTaken = 0.0;
	float DamageTaken = 0.0;
	if (Data.EvaluatedData.Attribute == GetDamage_StandardAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_SlashAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_PierceAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_StrikeAttribute())
	{
		if (Data.EvaluatedData.Attribute == GetDamage_StandardAttribute()) DamageTaken += GetDamage_Standard();
		if (Data.EvaluatedData.Attribute == GetDamage_SlashAttribute()) DamageTaken += GetDamage_Slash();
		if (Data.EvaluatedData.Attribute == GetDamage_PierceAttribute()) DamageTaken += GetDamage_Pierce();
		if (Data.EvaluatedData.Attribute == GetDamage_StrikeAttribute()) DamageTaken += GetDamage_Strike();
	}

	if (Data.EvaluatedData.Attribute == GetDamage_MagicAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_IceAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_FireAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_HolyAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_LightningAttribute())
	{
		if (Data.EvaluatedData.Attribute == GetDamage_MagicAttribute()) MagicDamageTaken += GetDamage_Magic();
		if (Data.EvaluatedData.Attribute == GetDamage_IceAttribute()) MagicDamageTaken += GetDamage_Ice();
		if (Data.EvaluatedData.Attribute == GetDamage_FireAttribute()) MagicDamageTaken += GetDamage_Fire();
		if (Data.EvaluatedData.Attribute == GetDamage_HolyAttribute()) MagicDamageTaken += GetDamage_Holy();
		if (Data.EvaluatedData.Attribute == GetDamage_LightningAttribute()) MagicDamageTaken += GetDamage_Lightning();
	}

	// Damage multipliers for weapon stats and player equipment, and any other status effects should be handled here
	float CurrentHealth = GetHealth() - MagicDamageTaken - DamageTaken;

	if (CurrentHealth <= 0.0)
	{
		SetHealth(0);

		// Handle death
	}
	else
	{
		SetHealth(CurrentHealth);

		// Handle any other take damage logic
	}


	
	/**
		Poise damage
			- Damage poise
			- Handle poise break / effect for regenerating poise
			- Handle hit reactions
	*/

	
	/**
		Any other effects to attributes
			- stamina drain, etc.
	*/

	

	if (Data.EvaluatedData.Attribute == GetDamage_StandardAttribute())
	{
		UE_LOGFMT(LogTemp, Log, "{0}::{1}() {2} dealt {3} damage to {4}!", *UEnum::GetValueAsString(Props.SourceActor->GetLocalRole()), *FString(__FUNCTION__),
			*GetNameSafe(Props.SourceActor), GetDamage_Standard(), *GetNameSafe(Props.TargetActor));
	}
	

	OnPostGameplayEffectExecute.Broadcast(Props);
}


void UMMOAttributeLogic::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}


void UMMOAttributeLogic::AttributeClamping(const FGameplayAttribute& Attribute, float& NewValue, const bool bUpdateAttributes)
{
	if (bUpdateAttributes)
	{
		if (Attribute == GetHealthAttribute()) SetHealth( FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		if (Attribute == GetStaminaAttribute()) SetStamina( FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
		if (Attribute == GetPoiseAttribute()) SetPoise( FMath::Clamp(GetPoise(), 0.f, GetMaxPoise()));
		if (Attribute == GetManaAttribute()) SetMana( FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
		
		if (Attribute == GetPoisonBuildupAttribute()) SetPoisonBuildup( FMath::Clamp(GetPoisonBuildup(), 0.f, GetMaxPoisonBuildup() * 2));
		if (Attribute == GetBleedBuildupAttribute()) SetBleedBuildup( FMath::Clamp(GetBleedBuildup(), 0.f, GetMaxBleedBuildup() * 2));
		if (Attribute == GetFrostbiteBuildupAttribute()) SetFrostbiteBuildup( FMath::Clamp(GetFrostbiteBuildup(), 0.f, GetMaxFrostbiteBuildup() * 2));
		if (Attribute == GetSleepBuildupAttribute()) SetSleepBuildup( FMath::Clamp(GetSleepBuildup(), 0.f, GetMaxSleepBuildup() * 2));
		if (Attribute == GetMadnessBuildupAttribute()) SetMadnessBuildup( FMath::Clamp(GetMadnessBuildup(), 0.f, GetMaxMadnessBuildup() * 2));
		if (Attribute == GetCurseBuildupAttribute()) SetCurseBuildup( FMath::Clamp(GetCurseBuildup(), 0.f, GetMaxCurseBuildup() * 2));
	}
	else
	{
		if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
		if (Attribute == GetPoiseAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPoise());
		if (Attribute == GetManaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
		
		if (Attribute == GetPoisonBuildupAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPoisonBuildup() * 2);
		if (Attribute == GetBleedBuildupAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxBleedBuildup() * 2);
		if (Attribute == GetFrostbiteBuildupAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxFrostbiteBuildup() * 2);
		if (Attribute == GetSleepBuildupAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxSleepBuildup() * 2);
		if (Attribute == GetMadnessBuildupAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMadnessBuildup() * 2);
		if (Attribute == GetCurseBuildupAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxCurseBuildup() * 2);
	}
}
