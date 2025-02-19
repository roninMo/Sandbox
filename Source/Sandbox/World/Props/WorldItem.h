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

	/** The level save's reference id for actors spawned in the world. If this is null, the save system use default saving logic (for item's placed in the level by designers) */
	UPROPERTY(BlueprintReadWrite) FString ActorSaveLevelId;
	
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
	 * Saves the actors information using the SaveComponent interface
	 * returns							Whether it successfully saved the actor information
	 */
	virtual bool SaveActorData_Implementation(const F_LevelSaveInformation_Actor& SaveConfig) override;

	/**
	 * Retrieves the level's currently saved state for this actor, and loads the saved information
	 * @note							This is only specific to spawned and placed actors within the level, utilize their save components to save specific information   
	 */
	virtual bool LoadFromLevel_Implementation(const F_LevelSaveInformation_Actor& PreviousSave) override;
	
	/**
	 * Retrieves the id of the actor. This is used for retrieving the proper save information for actors placed and spawned in the world \n
	 *
	 * This is going to generic for handling multiple use cases for different scenarios, and might be adjusted later because this is kind of hacky
	 *	- Players:									The character's subsystem account / platform id
	 *	- Items Placed in Level:					The name of the object based on the level's construction
	 *	- Items Spawned in Level during Play:		The inventory's id. Uses the class reference to construct and spawn the item once the game begins
	 *	
	 * TODO: unreal's GetName naming convention varies between server and client and is based on what's placed in the world. Anything spawned will be respawned,
	 *		and we still need a convention for mapping the saved name to the actor's stored FGuid id for reference
	 *
	 * @returns							The id of the actor   
	 */
	virtual FString GetActorLevelId_Implementation() const override;

	/**
	 * Utility function for setting the ActorSaveLevelId when the inventory item is spawned in the world.
	 *
	 * This should be called when you spawn an actor during play that needs to be saved
	 * @note TODO: This is hacky
	 */
	UFUNCTION(BlueprintCallable, Category = "Level|Saving") virtual void SetActorSaveLevelId(const FString& Id); 

	
};
