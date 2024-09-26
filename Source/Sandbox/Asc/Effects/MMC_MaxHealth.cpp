// Fill out your copyright notice in the Description page of Project Settings.


#include "MMC_MaxHealth.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"


UMMC_MaxHealth::UMMC_MaxHealth()
{
	// Attr_HealthDef.AttributeToCapture = UAttributeSet::GetAtr_HealthAttribute();
	Attr_HealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	Attr_HealthDef.bSnapshot = false;

	RelevantAttributesToCapture.AddUnique(Attr_HealthDef);
}


float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	const UAbilitySystemComponent* SourceAsc = EffectContextHandle.GetInstigatorAbilitySystemComponent();
	ACharacter* SourceCharacter = Cast<ACharacter>(SourceAsc->GetAvatarActor());
	// const ICombatInterface* CombatInterface = Cast<ICombatInterface>(SourceCharacter);

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Health = 0.f;
	GetCapturedAttributeMagnitude(Attr_HealthDef, Spec, EvaluationParameters, Health);
	Health = FMath::Max<float>(Health, 0);
	return 80.f + 2.5f * Health + 10.f; // * CombatInterface->GetPlayerLevel();
}
