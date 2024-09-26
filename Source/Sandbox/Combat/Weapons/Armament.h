// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Armament.generated.h"

UCLASS()
class SANDBOX_API AArmament : public AActor
{
	GENERATED_BODY()
	
public:	
	AArmament(const FObjectInitializer& ObjectInitializer);
	

protected:
	virtual void BeginPlay() override;

	
public:	
	virtual void Tick(float DeltaTime) override;

};
