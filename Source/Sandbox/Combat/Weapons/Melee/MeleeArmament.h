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

public:
	/** The hitbox for this melee armament. We use the @ref Hitboxes array in the case that subclassed melee armaments might have multiple hitboxes used for combat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament")
	TObjectPtr<UCapsuleComponent> ArmamentCollision;

	/** The specific holster that should be spawned when the weapon is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament")
	TSubclassOf<ACharacterAttachment> HolsterClass;

	
protected:
	/** The armament's currently spawned holster */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<ACharacterAttachment> Holster;

	/** The hitboxes that are used during combat for this armament */
	UPROPERTY(BlueprintReadWrite) TArray<UPrimitiveComponent*> Hitboxes;
	

//-------------------------------------------------------------------------------------//
// Armament Construction															   //
//-------------------------------------------------------------------------------------//
	AMeleeArmament();
	virtual void BeginPlay() override;
	
	
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
	
	
};
