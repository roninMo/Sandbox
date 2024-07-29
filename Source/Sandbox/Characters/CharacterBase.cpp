// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/CharacterBase.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	// : Super(ObjectInitializer.SetDefaultSubobjectClass<UAdvancedMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Peripheries = CreateDefaultSubobject<UPlayerPeripheriesComponent>(TEXT("Peripheries"));
	// Peripheries->GetPeripheryRadius()->SetupAttachment(RootComponent);
	// Peripheries->GetItemDetection()->SetupAttachment(RootComponent);
	// Peripheries->GetPeripheryCone()->SetupAttachment(CameraArm);
	//
	// Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// UAdvancedMovementComponent* ACharacterBase::GetAdvancedCharacterMovementComponent() const
// {
// 	return GetMovementComp<UAdvancedMovementComponent>();
// }
