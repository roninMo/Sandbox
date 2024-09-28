// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/CombatComponent.h"


UCombatComponent::UCombatComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

