// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "MeleeArmament.generated.h"

class ACharacterAttachment;
class UCapsuleComponent;


/**
 * 
 */
UCLASS()
class SANDBOX_API AMeleeArmament : public AArmament
{
	GENERATED_BODY()

protected:
	/** The hitbox for this melee armament. We use the @ref Hitboxes array in the case that subclassed melee armaments might have multiple hitboxes used for combat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament")
	TObjectPtr<UCapsuleComponent> ArmamentCollision;

	/** The specific holster that should be spawned when the weapon is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TSubclassOf<ACharacterAttachment> HolsterClass;
	
	/** The armament's currently spawned holster */
	UPROPERTY(BlueprintReadWrite, Replicated)
	TObjectPtr<ACharacterAttachment> Holster;
	
	/** The hitboxes that are used during combat for this armament */
	UPROPERTY(Transient, BlueprintReadWrite) TArray<UPrimitiveComponent*> Hitboxes;
	

//-------------------------------------------------------------------------------------//
// Armament Construction															   //
//-------------------------------------------------------------------------------------//
	AMeleeArmament();

	/** Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;

	/** Called before destroying the object. This is called immediately upon deciding to destroy the object, to allow the object to begin an asynchronous cleanup process. */
	virtual void BeginDestroy() override;
	
	
public:
	/** Function for equipping the armament. Add setup and teardown logic specific to this armament's configuration. */
	virtual bool ConstructArmament() override;
	
	/** Function for unequipping the armament. Add setup and teardown logic specific to this armament's configuration. */
	virtual bool DeconstructArmament() override;
	
	/** Function that checks whether the armament is valid to equip */
	virtual bool IsValidArmanent() override;

	
//-------------------------------------------------------------------------------------//
// Armament equipping and unequipping												   //
//-------------------------------------------------------------------------------------//
	/** Sheathes the weapon for the character */ 
	virtual bool SheatheArmament() override;

	/** Draw the character's sword */
	virtual bool UnsheatheArmament() override;

	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
	/** Shows or hides the armament and it's components for the player */
	virtual void SetOwnerNoSee(const bool bHide = true) override;

	/** Returns the armament's overlap components for their armament. */
	virtual TArray<UPrimitiveComponent*> GetArmamentHitboxes() const override;
	
	/**
	 * Print's this item's information on both server and client. \n\n
	 *
	 * This is intended to print both the CDO and the saved information pertaining to the player's instance of the object
	 *
	 * @note		you need to invoke this on both server and client
	 * @remarks		Override this function to add custom functionality specific to each item
	 */
	virtual void PrintItemInformation() override;

	
};
