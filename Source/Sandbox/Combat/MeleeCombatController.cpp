// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCombatController.h"

#include "CombatComponent.h"
#include "Sandbox/Characters/Player/PlayerCharacter.h"
#include "Weapons/Armament.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(ControllerLog);


void AMeleeCombatController::EquipWeapon(const bool bPrevWeapon, const bool bRightHand)
{
	Server_EquipWeapon(bPrevWeapon, bRightHand);
}


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

	EEquipSlot WeaponSlot;
	if (bPrevWeapon) WeaponSlot = CombatComponent->GetPrevEquipSlot(bRightHand);
	else WeaponSlot = CombatComponent->GetNextEquipSlot(bRightHand);
	UE_LOGFMT(LogTemp, Log, "{0} WeaponSlot: {1}", bPrevWeapon ? *FString("Prev") : *FString("Next"), *UEnum::GetValueAsString(WeaponSlot));

	// Remove the currently equipped armament if we transition to an empty slot
	if (!CombatComponent->IsValidSlot(WeaponSlot))
	{
		CombatComponent->DeleteEquippedArmament(CombatComponent->GetArmament(bRightHand));
	}

	// TODO: Add logic per game to either skip this or only allow it to happen once

	else
	{
		CombatComponent->CreateArmament(WeaponSlot);
		// CombatComponent->UpdateArmamentStanceAndAbilities();
	}
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



APlayerCharacter* AMeleeCombatController::GetPlayer() const
{
	return Cast<APlayerCharacter>(GetCharacter());
}