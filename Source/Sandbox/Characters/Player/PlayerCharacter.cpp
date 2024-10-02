// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Player/PlayerCharacter.h"

#include "Components/SphereComponent.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Characters/Components/Periphery/PeripheryComponent.h"
#include "Sandbox/Characters/Components/Camera/TargetLockSpringArm.h"
#include "Sandbox/Combat/CombatComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Peripheries = CreateDefaultSubobject<UPlayerPeripheriesComponent>(TEXT("Peripheries"));
	Peripheries->GetPeripheryRadius()->SetupAttachment(RootComponent); 
	Peripheries->GetItemDetection()->SetupAttachment(RootComponent);
	Peripheries->GetPeripheryCone()->SetupAttachment(CameraArm);

	// Adjust the periphery cone logic while in editor/game
	Peripheries->SetUnusedPeripheryComponentsVisibility();
	Peripheries->AdjustPeripheryConeInEditor(CameraArm);

	// Inventory Component
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	
	// Inventory Component
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	CombatComponent->SetIsReplicated(true);
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}
