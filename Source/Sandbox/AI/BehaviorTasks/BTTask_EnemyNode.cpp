// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/BehaviorTasks/BTTask_EnemyNode.h"

#include "Logging/StructuredLog.h"
#include "Sandbox/AI/Characters/Enemy.h"
#include "Sandbox/AI/Controllers/EnemyController.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"

DEFINE_LOG_CATEGORY(EnemyTask);


bool UBTTask_EnemyNode::GetCharacterInformation(APawn* Pawn)
{
	if (Enemy && Controller && MovementComponent && Inventory) return true;
	
	if (!GetCharacter(Pawn)) return false;
	if (!GetController(Pawn)) return false;
	if (!GetMovementComponent(Pawn)) return false;
	if (!GetInventory(Pawn)) return false;

	return true;
}

bool UBTTask_EnemyNode::GetCharacter(APawn* Pawn)
{
	if (Enemy) return true;

	Enemy = Cast<AEnemy>(Pawn);
	if (!Enemy)
	{
		UE_LOGFMT(EnemyTask, Error, "{0} {1}() {2} failed to get the character!", *UEnum::GetValueAsString(Enemy->GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return false;
	}

	return true;
}

bool UBTTask_EnemyNode::GetController(APawn* Pawn)
{
	if (Inventory) return true;
	if (!GetCharacter(Pawn)) return false;

	Controller = Enemy->GetController<AEnemyController>();
	if (!Inventory)
	{
		UE_LOGFMT(EnemyTask, Error, "{0} {1}() failed to get {2}'s controller", *UEnum::GetValueAsString(Enemy->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Enemy));
		return false;
	}

	return true;
}

bool UBTTask_EnemyNode::GetInventory(APawn* Pawn)
{
	if (Inventory) return true;
	if (!GetCharacter(Pawn)) return false;

	Inventory = Enemy->GetInventoryComponent();
	if (!Inventory)
	{
		UE_LOGFMT(EnemyTask, Error, "{0} {1}() failed to get {2}'s inventory", *UEnum::GetValueAsString(Enemy->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Enemy));
		return false;
	}

	return true;
}

bool UBTTask_EnemyNode::GetMovementComponent(APawn* Pawn)
{
	if (MovementComponent) return true;
	if (!GetCharacter(Pawn)) return false;
	
	MovementComponent = Enemy->GetAdvancedMovementComp();
	if (!MovementComponent)
	{
		UE_LOGFMT(EnemyTask, Error, "{0} {1}() failed to get {2}'s movement component", *UEnum::GetValueAsString(Enemy->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Enemy));
		return false;
	}

	return true;
}
