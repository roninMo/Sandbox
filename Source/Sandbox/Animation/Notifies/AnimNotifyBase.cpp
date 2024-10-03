// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Animation/Notifies/AnimNotifyBase.h"

#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(NotifyLog);


UAnimNotifyBase::UAnimNotifyBase()
{
}



bool UAnimNotifyBase::GetCharacterAndCombatComponent(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character, UCombatComponent*& CombatComponent) const
{
	Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (!Character)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the character!", *FString(__FUNCTION__), *GetNameSafe(MeshComp));
		return false;
	}
	
	CombatComponent = Character->GetCombatComponent<UCombatComponent>();
	if (!CombatComponent)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the combat base!", *FString(__FUNCTION__), *GetNameSafe(Character));
		return false;
	}

	return true;
}


bool UAnimNotifyBase::GetCharacterAndAbilitySystem(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character, UAbilitySystem*& AbilitySystem) const
{
	Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (!Character)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the character!", *FString(__FUNCTION__), *GetNameSafe(MeshComp));
		return false;
	}

	AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the ability system!", *FString(__FUNCTION__), *GetNameSafe(Character));
		return false;
	}

	return true;
}


bool UAnimNotifyBase::GetCharacterInformation(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character, UCombatComponent*& CombatComponent, UAbilitySystem*& AbilitySystem) const
{
	Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (!Character)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the character!", *FString(__FUNCTION__), *GetNameSafe(MeshComp));
		return false;
	}
	
	CombatComponent = Character->GetCombatComponent<UCombatComponent>();
	if (!CombatComponent)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the combat base!", *FString(__FUNCTION__), *GetNameSafe(Character));
		return false;
	}
	
	AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the ability system!", *FString(__FUNCTION__), *GetNameSafe(Character));
		return false;
	}

	return true;
}


bool UAnimNotifyBase::GetCharacter(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character) const
{
	Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (!Character)
	{
		UE_LOGFMT(NotifyLog, Error, "{0}() {1} failed to retrieve the character!", *FString(__FUNCTION__), *GetNameSafe(MeshComp));
		return false;
	}

	return true;
}
