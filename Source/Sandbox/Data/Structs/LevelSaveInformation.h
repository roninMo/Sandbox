// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSaveInformation.generated.h"


// LevelSave_Actor
// LevelSave_Collectable
// LevelSave_Events
// LevelSave_Quests


/**
 * Information specific to an item for displaying in the inventory and spawning them in the world
 * Also contains the information to access and construct actors that have been spawned in the world
 */
USTRUCT(BlueprintType)
struct F_LevelSaveInformation_Actor
{
	GENERATED_USTRUCT_BODY()
		F_LevelSaveInformation_Actor(
			const FGuid& Id = FGuid(),
			const FVector& Location = FVector(),
			const FRotator& Rotation = FRotator(),
			const TSubclassOf<AActor> Class = AActor::StaticClass()
		) :
		Id(Id),
		Location(Location),
		Rotation(Rotation),
		Class(Class)
	{}

	virtual ~F_LevelSaveInformation_Actor() {}

	/** The id of the item that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGuid Id;

	/** The location of the actor that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Location;

	/** The rotation of the actor that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rotation;

	/** A reference to the class of the object spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> Class;
	

public:
	/** Convenience function to access the id without creating another value */
	virtual FGuid GetId() const
	{
		return this->Id;
	}

	/** Is this a valid item? */
	virtual bool IsValid() const
	{
		return this->Id.IsValid();
	}
};
