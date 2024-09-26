// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultAttributes.h"

#include "Net/UnrealNetwork.h"

void UDefaultAttributes::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, HealthRegenRate, COND_None, REPNOTIFY_Always);
	
	// DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, Mana, COND_None, REPNOTIFY_Always);
	// DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, MaxMana, COND_None, REPNOTIFY_Always);
	// DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, ManaRegenRate, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributes, StaminaRegenRate, COND_None, REPNOTIFY_Always);
}


void UDefaultAttributes::OnRep_Health(const FGameplayAttributeData& OldHealth) const {						GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, Health, OldHealth); }
void UDefaultAttributes::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const {				GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, MaxHealth, OldMaxHealth); }
void UDefaultAttributes::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) const {	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, HealthRegenRate, OldHealthRegenRate); }

// void UDefaultAttributes::OnRep_Mana(const FGameplayAttributeData& OldMana) const {						GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, Mana, OldMana); }
// void UDefaultAttributes::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const {					GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, MaxMana, OldMaxMana); }
// void UDefaultAttributes::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) const {		GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, ManaRegenRate, OldManaRegenRate); }

void UDefaultAttributes::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const {					GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, Stamina, OldStamina); }
void UDefaultAttributes::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const {				GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, MaxStamina, OldMaxStamina); }
void UDefaultAttributes::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate) const {	GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributes, StaminaRegenRate, OldStaminaRegenRate); }
