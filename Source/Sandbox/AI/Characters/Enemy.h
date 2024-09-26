// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/AI/Characters/Npc.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API AEnemy : public ANpc
{
	GENERATED_BODY()

protected:
	

public:
	AEnemy(const FObjectInitializer& ObjectInitializer);
	

protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;
	
	
};
