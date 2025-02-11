// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MeleeCombatController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ControllerLog, Log, All);

class APlayerCharacter;
enum class EArmamentStance : uint8;


/**
 * Combat functions that aren't specific to ability functions
 */
UCLASS()
class SANDBOX_API AMeleeCombatController : public APlayerController
{
	GENERATED_BODY()

protected:


//------------------------------------------------------------------------------------------------------//
// Multiplayer functions for handling combat logic														//
//------------------------------------------------------------------------------------------------------//
public:
	/** Equips the prev or next weapon from the list for a certain hand */
	UFUNCTION(BlueprintCallable, Category = "PlayerController|Combat")
	virtual void EquipWeapon(const bool bPrevWeapon = true, const bool bRightHand = false);

	/** Updates the player's current armament stance */
	UFUNCTION(BlueprintCallable, Category = "PlayerController|Combat")
	virtual void SetArmamentStance(EArmamentStance NextStance);

	// TODO: These should be unreliable, and only be updated on the client if it was successful (or handled through serialization
	/** Handles weapon equipping logic on the server */
	UFUNCTION(Server, Reliable)
	virtual void Server_EquipWeapon(const bool bPrevWeapon = true, const bool bRightHand = false);

	/** Handles weapon equipping logic on the server */
	UFUNCTION(Server, Reliable)
	virtual void Server_SetArmamentStance(const EArmamentStance NextStance);



	
//------------------------------------------------------------------------------------------------------//
// Utility																								//
//------------------------------------------------------------------------------------------------------//
public:
	/** Retrieves a reference to the character's base class for combat */
	UFUNCTION(BlueprintCallable, Category="PlayerController|Combat|Utility")
	virtual APlayerCharacter* GetPlayer() const;

	
};
