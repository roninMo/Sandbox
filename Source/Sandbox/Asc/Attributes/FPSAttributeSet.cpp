// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/FPSAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UFPSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPSAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPSAttributeSet, Damage, COND_None, REPNOTIFY_Always);
}


void UFPSAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSAttributeSet, Armor, OldArmor); }
void UFPSAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSAttributeSet, Damage, OldDamage); }
