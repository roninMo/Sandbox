// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/World/Props/EffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Sandbox/Characters/CharacterBase.h"

AEffectActor::AEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	const ACharacterBase* BaseCharacter = Cast<ACharacterBase>(TargetActor);
	UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetAsc || !BaseCharacter) return;
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle EffectContextHandle = TargetAsc->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetAsc->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	TargetAsc->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}


void AEffectActor::OnOverlap(AActor* TargetActor) { ApplyApplicationPolicyEffects(TargetActor, EEffectApplicationPolicy::ApplyOnOverlap); }
void AEffectActor::OnEndOverlap(AActor* TargetActor) { ApplyApplicationPolicyEffects(TargetActor, EEffectApplicationPolicy::ApplyOnEndOverlap); }
void AEffectActor::ApplyApplicationPolicyEffects(AActor* TargetActor, const EEffectApplicationPolicy ApplicationPolicy)
{
	if (InstantGameplayEffectClass && InstantEffectApplicationPolicy == ApplicationPolicy) ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	if (DurationGameplayEffectClass && DurationEffectApplicationPolicy == ApplicationPolicy) ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);

	// For infinite effects, apply on start, remove on end, or apply on end 
	if (!InfiniteGameplayEffectClass) return;
	UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetAsc == nullptr) return;

	// get the gameplay effect context
	FGameplayEffectContextHandle EffectContextHandle = TargetAsc->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetAsc->MakeOutgoingSpec(InfiniteGameplayEffectClass, 1.f, EffectContextHandle);

	// apply the effect
	if (ApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) InfiniteEffectHandle = TargetAsc->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	if (ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap) ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
		if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap && InfiniteEffectHandle.IsValid())
		{
			TargetAsc->RemoveActiveGameplayEffect(InfiniteEffectHandle);
			InfiniteEffectHandle.Invalidate();
		}
	}
}
