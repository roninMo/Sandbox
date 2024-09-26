// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/BehaviorTasks/BTTask_SetMoveSpeedAndMoveTo.h"

#include "Kismet/KismetMathLibrary.h"
#include "Sandbox/AI/Characters/Enemy.h"
#include "Sandbox/AI/Controllers/EnemyController.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"


UBTTask_SetMoveSpeedAndMoveTo::UBTTask_SetMoveSpeedAndMoveTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MoveSpeed = 340;
	MoveSpeedDeviation = 0;
	AcceptableRadius = 50;
}

EBTNodeResult::Type UBTTask_SetMoveSpeedAndMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyController* Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (Controller)
	{
		AEnemy* Character = Cast<AEnemy>(Controller->GetPawn());
		if (Character && Character->GetAdvancedMovementComp())
		{
			InitialSpeed = Character->GetAdvancedMovementComp()->GetMaxWalkSpeed();
			Character->GetAdvancedMovementComp()->SetMaxWalkSpeed(MoveSpeed + UKismetMathLibrary::RandomFloatInRange(-MoveSpeedDeviation, MoveSpeedDeviation));
		}
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_SetMoveSpeedAndMoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AEnemyController* Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (Controller)
	{
		AEnemy* Character = Cast<AEnemy>(Controller->GetPawn());
		if (Character && Character->GetAdvancedMovementComp())
		{
			Character->GetAdvancedMovementComp()->SetMaxWalkSpeed(InitialSpeed);
		}
	}
	
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
