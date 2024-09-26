// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Controllers/EnemyController.h"

AEnemyController::AEnemyController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MinNetUpdateFrequency = 33;
	NetUpdateFrequency = 66;
	bReplicates = false;
	
	// Base AI Configuration
	bStartAILogicOnPossess = 1;
	bAllowStrafe = 1;
	//bWantsPlayerState = 1; // this might be useful later for specific situations
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
