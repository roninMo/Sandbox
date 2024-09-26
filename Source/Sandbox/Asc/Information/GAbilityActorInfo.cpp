// Fill out your copyright notice in the Description page of Project Settings.


#include "GAbilityActorInfo.h"

void FGAbilityActorInfo::InitFromActor(AActor* Owner, AActor* Avatar, UAbilitySystemComponent* InAbilitySystemComponent)
{
	FGameplayAbilityActorInfo::InitFromActor(Owner, Avatar, InAbilitySystemComponent);
}

void FGAbilityActorInfo::SetAvatarActor(AActor* Avatar)
{
	FGameplayAbilityActorInfo::SetAvatarActor(Avatar);
}

void FGAbilityActorInfo::ClearActorInfo()
{
	FGameplayAbilityActorInfo::ClearActorInfo();
}
