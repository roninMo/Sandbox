// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ForgeProp.generated.h"

enum class ELevelItemType : uint8;


/**
 * This probably needs to be refactored
 *		- either use a reference to retrieve items
 *		- or have a function for retrieving added objects (add an ItemType specification to props created in normal level design)
 *
 * Where this logic is confusing is allowing the player to adjust props that are saved in the level, or creating custom props in online play.
 *		- Not every object in the engine has built in functionality for replication, or creation, and we need logic for handling location adjustments while creating things in an efficient net safe fashion
 *			- Actors already have safe spawning and some replication functions for values (you still need to create net serialization for movement, and other logic)
 *
 */
UCLASS()
class SANDBOX_API UForgeProp : public UObject
{
	GENERATED_BODY()

protected:
	/** What kind of prop is this? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForgeProp") ELevelItemType ItemType;

	/** The spawned object, whether it be a Prop, Actor, Enemy, Weapon, GameMode Objects, Vehicles, FX, etc */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForgeProp") TObjectPtr<UObject> Prop;

	
public:
	/** Handles spawning and placing the item in the level */
	UFUNCTION(BlueprintCallable, Category = "UForgeProp") virtual bool InitProp(UObject* NewProp);

	/** Resets the item to it's saved state, effectively handling removing and respawning. Use while in editor  */
	UFUNCTION(BlueprintCallable, Category = "UForgeProp") virtual bool Respawn();

	/** Retrieves save information about the item. Used for saving information */ 
	UFUNCTION(BlueprintCallable, Category = "UForgeProp") virtual void RetrieveSaveInformation();

	
};
