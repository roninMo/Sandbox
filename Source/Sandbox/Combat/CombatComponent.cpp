// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/CombatComponent.h"

#include "Sandbox/Asc/Attributes/AttributeLogic.h"
#include "Sandbox/Data/Enums/EquipSlot.h"
#include "Weapons/Armament.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicated(true);
	
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatComponent::BeginDestroy()
{
	// Remove the player's equipment
	// if (PrimaryArmament)
	// {
	// 	PrimaryArmament->DeconstructArmament();
	// 	PrimaryArmament->Destroy();
	// }
	//
	// if (SecondaryArmament)
	// {
	// 	SecondaryArmament->DeconstructArmament();
	// 	SecondaryArmament->Destroy();
	// }
	//
	//
	//
	// // Remove the delegate bindings 
	// OnEquippedArmament.RemoveAll(this);
	// OnUnequippedArmament.RemoveAll(this);
	//
	Super::BeginDestroy();
}


void UCombatComponent::CombatCalculations(const FGAttributeSetExecutionData& Props)
{
}





void UCombatComponent::AddArmamentToEquipSlot(const F_Item& ArmamentInventoryInformation, const EEquipSlot EquipSlot)
{
	if (!ArmamentInventoryInformation.IsValid()) return;
	
	if (EquipSlot == EEquipSlot::LeftHandSlotOne) LeftHandEquipSlot_One = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::LeftHandSlotTwo) LeftHandEquipSlot_Two = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::LeftHandSlotThree) LeftHandEquipSlot_Three = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotOne) RightHandEquipSlot_One = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotTwo) RightHandEquipSlot_Two = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotThree) RightHandEquipSlot_Three = ArmamentInventoryInformation;
}

void UCombatComponent::RemoveArmamentFromEquipSlot(const EEquipSlot EquipSlot)
{
	if (EquipSlot == EEquipSlot::LeftHandSlotOne) LeftHandEquipSlot_One = F_Item();
	else if (EquipSlot == EEquipSlot::LeftHandSlotTwo) LeftHandEquipSlot_Two = F_Item();
	else if (EquipSlot == EEquipSlot::LeftHandSlotThree) LeftHandEquipSlot_Three = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotOne) RightHandEquipSlot_One = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotTwo) RightHandEquipSlot_Two = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotThree) RightHandEquipSlot_Three = F_Item();
}

AArmament* UCombatComponent::CreateArmament(const EEquipSlot EquipSlot)
{
	return nullptr;
}

bool UCombatComponent::DeleteEquippedArmament(AArmament* Armament)
{
	return false;
}

void UCombatComponent::SetArmamentStance(EArmamentStance Stance)
{
}

AArmament* UCombatComponent::GetArmament(const bool bRightHand)
{
	return nullptr;
}

bool UCombatComponent::IsRightHandedArmament(EEquipSlot Slot)
{
	return false;
}

EEquipSlot UCombatComponent::GetCurrentlyEquippedSlot(bool bRightHand)
{
	return EEquipSlot::None;
}

EEquipSlot UCombatComponent::GetNextEquipSlot(bool bRightHand)
{
	return EEquipSlot::None;
}

F_Item UCombatComponent::GetArmamentInventoryInformation(EEquipSlot Slot)
{
	return F_Item();
}

F_ArmamentInformation UCombatComponent::GetArmamentInformation(EEquipSlot Slot)
{
	return F_ArmamentInformation();
}


void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


FName UCombatComponent::GetEquippedSocket(EArmamentClassification Armament, EEquipSlot EquipSlot) const
{
	return FName();
}


FName UCombatComponent::GetHolsterSocket(EArmamentClassification Armament, EEquipSlot EquipSlot) const
{
	return FName();
}


FName UCombatComponent::GetSheathedSocket(EArmamentClassification Armament, EEquipSlot EquipSlot) const
{
	return FName();
}

