// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCombatController.h"

#include "CombatComponent.h"
#include "Sandbox/Characters/Player/PlayerCharacter.h"
#include "Weapons/Armament.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(ControllerLog);


void AMeleeCombatController::EquipNextWeapon_LeftHand() { Server_EquipWeapon(false, false); }
void AMeleeCombatController::EquipPrevWeapon_LeftHand() { Server_EquipWeapon(true, false); }
void AMeleeCombatController::EquipNextWeapon_RightHand() { Server_EquipWeapon(false, true); }
void AMeleeCombatController::EquipPrevWeapon_RightHand() { Server_EquipWeapon(true, true); }


void AMeleeCombatController::SetArmamentStance(const EArmamentStance NextStance)
{
	Server_SetArmamentStance(NextStance);
}


void AMeleeCombatController::Server_EquipWeapon_Implementation(const bool bPrevWeapon, const bool bRightHand)
{
	// Sanity Checks
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter)
	{
		UE_LOGFMT(ControllerLog, Error, "{0}::{1}() {2} Failed to retrieve a valid reference to the player character class!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ControllerLog, Error, "{0}::{1}() {2} Failed to retrieve a valid reference to the player's combat component!",
			UEnum::GetValueAsString(PlayerCharacter->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(PlayerCharacter));
		return;
	}

	AArmament* CurrentWeapon = CombatComponent->GetArmament(bRightHand);
	if (!CurrentWeapon)
	{
		
	}

	EEquipSlot WeaponSlot = CombatComponent->GetNextEquipSlot(bRightHand);
	CombatComponent->CreateArmament(WeaponSlot);
	// CombatComponent->UpdateArmamentStanceAndAbilities();
}


void AMeleeCombatController::Server_SetArmamentStance_Implementation(const EArmamentStance NextStance)
{
	// Sanity Checks
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter)
	{
		UE_LOGFMT(ControllerLog, Error, "{0}::{1}() {2} Failed to retrieve a valid reference to the player character class!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ControllerLog, Error, "{0}::{1}() {2} Failed to retrieve a valid reference to the player's combat component!",
			UEnum::GetValueAsString(PlayerCharacter->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(PlayerCharacter));
		return;
	}

	CombatComponent->SetArmamentStance(NextStance);
}



APlayerCharacter* AMeleeCombatController::GetPlayer()
{
	return Cast<APlayerCharacter>(GetCharacter());
}