// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UAdvancedMovementComponent>(ACharacter::CharacterMovementComponentName)
)
{
	
}




#pragma region Character Initialization
void ACharacterBase::InitCharacterGlobals(UDataAsset* Data)
{
}

void ACharacterBase::InitCharacterComponents(const bool bCalledFromPossessedBy)
{
}

void ACharacterBase::InitAbilitySystemAndAttributes(const bool bCalledFromPossessedBy)
{
}

void ACharacterBase::InitCharacterInformation()
{
}

UAdvancedMovementComponent* ACharacterBase::GetAdvancedCharacterMovementComponent() const
{
	return GetMovementComp<UAdvancedMovementComponent>();
}
#pragma endregion




void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}
