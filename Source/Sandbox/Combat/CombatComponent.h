// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AArmament;
enum class EEquipSlot : uint8;
enum class EArmamentClassification : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FArmamentEquippedSignature, AArmament*, Armament, EEquipSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FArmamentUnequippedSignature, FName, Id, EEquipSlot, EquipSlot);

/**
 * Combat component for characters and enemies in the game
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent(const FObjectInitializer& ObjectInitializer);
	
	/** Delegate for when a player equips an armament */
	UPROPERTY(BlueprintAssignable) FArmamentEquippedSignature OnEquippedArmament;

	/** Delegate for when a player unequips an armament */
	UPROPERTY(BlueprintAssignable) FArmamentUnequippedSignature OnUnequippedArmament;
	
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Retrieves the equipped socket for a specific armament */
	UFUNCTION(BlueprintCallable, Category = "Combat|Equipping") virtual FName GetEquippedSocket(EArmamentClassification Armament, EEquipSlot EquipSlot) const;
	
	/** Retrieves the holster for a specific equipped armament */
	UFUNCTION(BlueprintCallable, Category = "Combat|Equipping") virtual FName GetHolsterSocket(EArmamentClassification Armament, EEquipSlot EquipSlot) const;
	
	/** Retrieves the sheathed for a specific equipped armament */
	UFUNCTION(BlueprintCallable, Category = "Combat|Equipping") virtual FName GetSheathedSocket(EArmamentClassification Armament, EEquipSlot EquipSlot) const;

	
};




/*

	Weapon
		- Ranged Information / Damage
			- Weapon / Attachment config
			- Ranged damage information
			
		- Melee Information / Damage
			- Combo retrieval
			- Melee damage information

	->
	-> Damage information (varying attribute set modifiers)

	
	Weapon
		- Equip slot mapping
		- Anim montage mapping
		- Ability mapping
		- Character to Weapon information mapping
		- 







	Combat Component
		- Attribute damage calculation



	Combat
		- Weapon retrieves it's attack information
			- Branching logic is okay here, it ends up adjusting attributes which can easily be added to both
		- Weapon creates a damage calculation and sends it to the attribute logic
		- AttributeLogic handles adjusting attributes

 
*/

