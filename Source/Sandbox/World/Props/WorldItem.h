// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Interfaces/PeripheryObject/PeripheryObjectInterface.h"
#include "Sandbox/Data/Interfaces/Save/LevelSaveInformationInterface.h"
#include "Sandbox/World/Props/Items/Item.h"
#include "WorldItem.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API AWorldItem : public AItem, public IPeripheryObjectInterface, public ILevelSaveInformationInterface
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

	/**
	 * Function that needs to be called when the object has been spawned in the world. Used for initializing information specific to the item. \n\n
	 * Initially intended for creating an Id for saving if it wasn't one that was spawned in the world already. \n  
	 *
	 * @remarks This function should be called once you've initialized it's values
	 */
	UFUNCTION(BlueprintCallable, Category = "Item|Initialization") virtual void OnSpawnedInWorld();


	
	
//----------------------------------------------------------------------------------//
// Periphery Logic																	//
//----------------------------------------------------------------------------------//
public:
	/** Logic when a character registers it within it's periphery */
	virtual void WithinPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;

	/** Logic when a character unregisters it within it's periphery */
	virtual void OutsideOfPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;
	
	/** Logic when a character's periphery cone registers the object */
	virtual void WithinPlayerConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;

	/** Logic when a character's periphery cone unregisters the object */
	virtual void OutsideOfConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;
	

	

//----------------------------------------------------------------------------------//
// Save Logic																		//
//----------------------------------------------------------------------------------//
public:
	/**
	 * Returns the current save state of the actor level during play
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	virtual F_LevelSaveInformation_Actor SaveToLevel_Implementation() override;

	/**
	 * Retrieves the level's currently saved state for this actor, and loads the saved information
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	virtual bool LoadFromLevel_Implementation(const F_LevelSaveInformation_Actor& PreviousSave) override;
	
	/**
	 * Retrieves the id of the actor. This is used for retrieving the proper save information for actors placed and spawned in the world
	 *
	 * @returns							The id of the actor   
	 */
	virtual FGuid GetActorLevelId_Implementation() const override;

	
};
