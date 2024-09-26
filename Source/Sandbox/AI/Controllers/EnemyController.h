// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/AI/Controllers/AIControllerBase.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API AEnemyController : public AAIControllerBase
{
	GENERATED_BODY()


	AEnemyController(const FObjectInitializer& ObjectInitializer);
	

protected:
	/**
	 * Routes each of the senses to the perception sense functions
	 * 
	 * @note Bind this to the AIPerception's OnPerceptionUpdated in the blueprint
	 */
	virtual void OnPerceptionUpdated_Implementation(TArray<AActor*>& Actors);
	
	/**
	 * Returns Perception information when it senses something
	 * 
	 * @note Bind this to the AIPerception's OnPerceptionInfoUpdated in the blueprint
	 */
	virtual void OnTargetPerceptionUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation);

	/**
	 * Forgets a character once it's successfully unsensed
	 * 
	 * @note this doesn't work unless OnTargetPerceptionUpdated delegate binding is linked to AIPerception's OnPerceptionInfoUpdated in the blueprint
	 */
	virtual void OnTargetPerceptionForgotten_Implementation(AActor* Actor);

	
};
