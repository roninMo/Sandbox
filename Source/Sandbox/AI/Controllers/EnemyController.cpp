// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Controllers/EnemyController.h"

#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"

AEnemyController::AEnemyController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MinNetUpdateFrequency = 33;
	NetUpdateFrequency = 66;
	bReplicates = false;
	
	// Base AI Configuration
	bStartAILogicOnPossess = 1;
	bAllowStrafe = 1;
	//bWantsPlayerState = 1; // this might be useful later for specific situations
	
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystem>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UMMOAttributeSet>(TEXT("Attributes"));
}


UAbilitySystemComponent* AEnemyController::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


UAttributeSet* AEnemyController::GetAttributeSet() const
{
	return AttributeSet;
}


void AEnemyController::OnPerceptionUpdated_Implementation(TArray<AActor*>& Actors)
{
	Super::OnPerceptionUpdated_Implementation(Actors);
}


void AEnemyController::OnTargetPerceptionUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation)
{
	Super::OnTargetPerceptionUpdated_Implementation(UpdateInformation);
}


void AEnemyController::OnTargetPerceptionForgotten_Implementation(AActor* Actor)
{
	Super::OnTargetPerceptionForgotten_Implementation(Actor);
}
