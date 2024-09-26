// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_SetMoveSpeedAndMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UBTTask_SetMoveSpeedAndMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

protected:
	/** How fast should the character go during the task? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Node, meta=(ClampMin = "0.0", UIMin="0.0")) float MoveSpeed;

	/** The devation in the character's move speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Node, meta=(ClampMin = "0.0", UIMin="0.0")) float MoveSpeedDeviation;
	
	/** Cached speed for when the task is completed */
	UPROPERTY() float InitialSpeed;

	UBTTask_SetMoveSpeedAndMoveTo(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	
};
