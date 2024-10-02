// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"

#include "Logging/StructuredLog.h"
// #include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"

UCombatAbility::UCombatAbility()
{
	// Combo attacks should be activated on every actor, and the information needs to be saved between attacks
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}


void UCombatAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}


void UCombatAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


bool UCombatAbility::GetComboAndArmamentInformation()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent) return false;
	
	// if (AArmament* EquippedArmament = CombatComponent->GetArmament(bUseRightHandArmament)) 
	// {
	// 	if (EquippedArmament == Armament && ArmamentStance == CombatBase->ArmamentStance) return true;
	// 	
	// 	// UE_LOGFMT(LogTemp, Warning, "{0}() {1}: Attack armament information", *FString(__FUNCTION__), *GetNameSafe(BaseCharacter));
	// 	ArmamentInformation = EquippedArmament->GetArmamentInformation();
	// 	ComboAttacks = EquippedArmament->GetCombos(AttackClassification);
	// 	AttackMontage = EquippedArmament->GetAttackMontage(AttackClassification);
	// 	ensure(!ComboAttacks.IsEmpty());
	// 	Armament = EquippedArmament;
	// 	ArmamentStance = CombatBase->ArmamentStance;
	// 	return true;
	// }
	
	return false;
}


UCombatComponent* UCombatAbility::GetCombatComponent() const
{
	// ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	// if (!Character)
	// {
	// 	UE_LOGFMT(AbilityLog, Error, "{0}::{1} Failed to retrieve the character while retrieving the combat component!!",
	// 		UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *GetNameSafe(GetOwningActorFromActorInfo()));
	// 	return nullptr;
	// }
	//
	// return Character->GetCombatComponent();
	return nullptr;
}


