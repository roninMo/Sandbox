// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Interfaces/PeripheryObject/PeripheryObjectInterface.h"
#include "Sandbox/World/Props/Items/Item.h"
#include "WorldItem.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API AWorldItem : public AItem, public IPeripheryObjectInterface
{
	GENERATED_BODY()
	
protected:
	/** The physical item that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr <UStaticMeshComponent> WorldItem;

	
public:
	AWorldItem();

	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;
	
	/** Logic when a character registers it within it's periphery */
	virtual void WithinPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;

	/** Logic when a character unregisters it within it's periphery */
	virtual void OutsideOfPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;
	
	/** Logic when a character's periphery cone registers the object */
	virtual void WithinPlayerConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;

	/** Logic when a character's periphery cone unregisters the object */
	virtual void OutsideOfConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;
	

	
};
