// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAdvancedMovementComponent>(ACharacter::CharacterMovementComponentName))
{
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}


UAdvancedMovementComponent* ACharacterBase::GetAdvancedCharacterMovementComponent() const
{
	return GetMovementComp<UAdvancedMovementComponent>();
}
