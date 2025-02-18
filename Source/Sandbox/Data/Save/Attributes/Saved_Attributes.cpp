// Fill out your copyright notice in the Description page of Project Settings.


#include "Saved_Attributes.h"

#include "Sandbox/Asc/Attributes/DefaultAttributes.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"

void USaved_Attributes::RetrieveAttributesFromAttributeSet(UAttributeSet* AttributeSet)
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


UGameplayEffect* USaved_Attributes::GetCurrentAttributes(UAttributeSet* AttributeSet)
{
	if (!AttributeSet) return nullptr;

	const FName SavedAttributes = FName(*GetNameSafe(AttributeSet) + FString("_SavedAttributes"));
	UGameplayEffect* Attributes = NewObject<UGameplayEffect>(AttributeSet, SavedAttributes);

	
	UDefaultAttributes* DefaultAttributes = Cast<UDefaultAttributes>(AttributeSet);
	if (DefaultAttributes)
	{
		Attributes->Modifiers.Add(CreateModifierAttribute(DefaultAttributes->GetHealthAttribute(), Health));
		Attributes->Modifiers.Add(CreateModifierAttribute(DefaultAttributes->GetStaminaAttribute(), Stamina));
	}
	
	UMMOAttributeSet* MMOAttributes = Cast<UMMOAttributeSet>(AttributeSet);
	if (MMOAttributes)
	{
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetExperienceAttribute(), Experience));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetManaAttribute(), Mana));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetPoiseAttribute(), Poise));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetBleedBuildupAttribute(), BleedBuildup));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetFrostbiteBuildupAttribute(), FrostbiteBuildup));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetPoisonBuildupAttribute(), PoisonBuildup));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetMadnessBuildupAttribute(), MadnessBuildup));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetCurseBuildupAttribute(), CurseBuildup));
		Attributes->Modifiers.Add(CreateModifierAttribute(MMOAttributes->GetSleepBuildupAttribute(), SleepBuildup));
	}

	return Attributes;
}


FGameplayModifierInfo USaved_Attributes::CreateModifierAttribute(const FGameplayAttribute& Attribute, const float Value)
{
	FGameplayModifierInfo Modifier;
	Modifier.Attribute = Attribute;
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(Value);
	return Modifier;
}
