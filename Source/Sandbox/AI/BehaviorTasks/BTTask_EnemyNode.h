// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_EnemyNode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(EnemyTask, Log, All);

class UInventoryComponent;
class UAdvancedMovementComponent;
class AEnemyController;
class AEnemy;


/**
 *  Base class for blueprint based task nodes. Do NOT use it for creating native c++ classes!
 *
 *  When task receives Abort event, all latent actions associated this instance are being removed.
 *  This prevents from resuming activity started by Execute, but does not handle external events.
 *  Please use them safely (unregister at abort) and call IsTaskExecuting() when in doubt.
 */
UCLASS()
class SANDBOX_API UBTTask_EnemyNode : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite) TObjectPtr<AEnemy> Enemy;
	UPROPERTY(BlueprintReadWrite) TObjectPtr<AEnemyController> Controller;
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAdvancedMovementComponent> MovementComponent;
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UInventoryComponent> Inventory;
	
	
protected:
	/** Retrieves the character's information and stores a reference to them within the blueprint */
	UFUNCTION(BlueprintCallable, Category = "AI|Get and Set Functions") virtual bool GetCharacterInformation(APawn* Pawn);
	
	/** Retrieves the character and stores a reference to them within the blueprint  */
	UFUNCTION(BlueprintCallable, Category = "AI|Get and Set Functions") virtual bool GetCharacter(APawn* Pawn);
	
	/** Retrieves the character's controller and stores a reference to them within the blueprint  */
	UFUNCTION(BlueprintCallable, Category = "AI|Get and Set Functions") virtual bool GetController(APawn* Pawn);
	
	/** Retrieves the character's inventory and stores a reference to them within the blueprint  */
	UFUNCTION(BlueprintCallable, Category = "AI|Get and Set Functions") virtual bool GetInventory(APawn* Pawn);
	
	/** Retrieves the character's movement component and stores a reference to them within the blueprint  */
	UFUNCTION(BlueprintCallable, Category = "AI|Get and Set Functions") virtual bool GetMovementComponent(APawn* Pawn);
	
	
};
