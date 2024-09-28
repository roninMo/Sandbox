// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterAttachment.generated.h"

UCLASS()
class SANDBOX_API ACharacterAttachment : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Object;
	
	
protected:
	ACharacterAttachment();
	virtual void BeginPlay() override;

	
};
