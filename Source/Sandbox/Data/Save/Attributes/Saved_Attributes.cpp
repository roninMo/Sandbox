// Fill out your copyright notice in the Description page of Project Settings.


#include "Saved_Attributes.h"

#include "Sandbox/Asc/Attributes/DefaultAttributes.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"

void USaved_Attributes::SaveFromAttributeSet(UAttributeSet* AttributeSet)
{
	if (!AttributeSet)
	{
		return;
	}
	
	UDefaultAttributes* DefaultAttributes = Cast<UDefaultAttributes>(AttributeSet);
	if (DefaultAttributes)
	{
		Health = DefaultAttributes->GetHealth();
		Stamina = DefaultAttributes->GetStamina();
	}
	
	UMMOAttributeSet* MMOAttributes = Cast<UMMOAttributeSet>(AttributeSet);
	if (MMOAttributes)
	{
		Experience = MMOAttributes->GetExperience();
		Mana = MMOAttributes->GetMana();
		Poise = MMOAttributes->GetPoise();
		BleedBuildup = MMOAttributes->GetBleedBuildup();
		FrostbiteBuildup = MMOAttributes->GetFrostbiteBuildup();
		PoisonBuildup = MMOAttributes->GetPoisonBuildup();
		MadnessBuildup = MMOAttributes->GetMadnessBuildup();
		CurseBuildup = MMOAttributes->GetCurseBuildup();
		SleepBuildup = MMOAttributes->GetSleepBuildup();
	}
}
