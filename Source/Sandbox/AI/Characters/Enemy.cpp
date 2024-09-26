// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Characters/Enemy.h"

AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AEnemy::InitCharacterComponents(const bool bCalledFromPossessedBy)
{
	Super::InitCharacterComponents(bCalledFromPossessedBy);
}

void AEnemy::InitAbilitySystem(const bool bCalledFromPossessedBy)
{
	Super::InitAbilitySystem(bCalledFromPossessedBy);
}

void AEnemy::InitCharacterInformation()
{
	Super::InitCharacterInformation();
}
