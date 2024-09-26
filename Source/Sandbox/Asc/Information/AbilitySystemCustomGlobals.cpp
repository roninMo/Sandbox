// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemCustomGlobals.h"

#include "GAbilityActorInfo.h"
#include "GE_Context.h"


FGameplayAbilityActorInfo* UAbilitySystemCustomGlobals::AllocAbilityActorInfo() const
{
	return new FGAbilityActorInfo();
}


FGameplayEffectContext* UAbilitySystemCustomGlobals::AllocGameplayEffectContext() const
{
	return new FGE_Context();
}


UGameplayCueManager* UAbilitySystemCustomGlobals::GetGameplayCueManager()
{
	return Super::GetGameplayCueManager();
}


void UAbilitySystemCustomGlobals::GlobalPreGameplayEffectSpecApply(FGameplayEffectSpec& Spec, UAbilitySystemComponent* AbilitySystemComponent)
{
	Super::GlobalPreGameplayEffectSpecApply(Spec, AbilitySystemComponent);
}


void UAbilitySystemCustomGlobals::PushCurrentAppliedGE(const FGameplayEffectSpec* Spec, UAbilitySystemComponent* AbilitySystemComponent)
{
	Super::PushCurrentAppliedGE(Spec, AbilitySystemComponent);
}


void UAbilitySystemCustomGlobals::SetCurrentAppliedGE(const FGameplayEffectSpec* Spec)
{
	Super::SetCurrentAppliedGE(Spec);
}


void UAbilitySystemCustomGlobals::PopCurrentAppliedGE()
{
	Super::PopCurrentAppliedGE();
}


void UAbilitySystemCustomGlobals::GameplayEffectPostEditChangeProperty(UGameplayEffect* GE, FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::GameplayEffectPostEditChangeProperty(GE, PropertyChangedEvent);
}
