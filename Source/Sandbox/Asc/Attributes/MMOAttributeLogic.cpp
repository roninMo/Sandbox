// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/MMOAttributeLogic.h"

#include "GameplayEffectExtension.h"
#include "Logging/StructuredLog.h"


bool UMMOAttributeLogic::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}


void UMMOAttributeLogic::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Any clamping that happens here does not permanently change the modifier on the ASC. It only changes the value returned from querying the modifier.
	// This means anything that recalculates the CurrentValue from all of the modifiers like GameplayEffectExecutionCalculations and ModifierMagnitudeCalculations need to implement clamping again.
	Super::PreAttributeChange(Attribute, NewValue);

	
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	if (Attribute == GetPoiseAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPoise());
	if (Attribute == GetManaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());

	
}


void UMMOAttributeLogic::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	FGAttributeSetExecutionData Props;
	GetExecutionData(Data, Props);
	
	/**
		Apply buildups / damages / poise damages, then the hit react, and handle any other logic after that

		Handling individual calculations isn't as complex as having everything together, there's a lot more of interesting things and calculations with multiple attributes.
		You just need to clear out the old attributes and help handle some of the basic values for handling damage

	*/


	
	
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
		

	}

	
	/**
		Damage calculations
			- Take damage
			- Hit reaction based on the attack
			- Handle taking damage / dying
	*/
	if (Data.EvaluatedData.Attribute == GetDamage_StandardAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_SlashAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_PierceAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_StrikeAttribute())
	{
		
	}

	if (Data.EvaluatedData.Attribute == GetDamage_MagicAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_IceAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_FireAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_HolyAttribute() ||
		Data.EvaluatedData.Attribute == GetDamage_LightningAttribute())
	{
		
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
