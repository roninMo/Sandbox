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
	/** Equips the next left hand weapon from the current left hand weapons */
	UFUNCTION(BlueprintCallable, Category = "PlayerController|Combat")
	virtual void EquipNextWeapon_LeftHand();

	/** Equips the previous left hand weapon from the current left hand weapons */
	UFUNCTION(BlueprintCallable, Category = "PlayerController|Combat")
	virtual void EquipPrevWeapon_LeftHand();
	
	/** Equips the next right hand weapon from the current right hand weapons */
	UFUNCTION(BlueprintCallable, Category = "PlayerController|Combat")
	virtual void EquipNextWeapon_RightHand();
	
	/** Equips the previous right hand weapon from the current right hand weapons */
	UFUNCTION(BlueprintCallable, Category = "PlayerController|Combat")
	virtual void EquipPrevWeapon_RightHand();

	/** Updates the player's current armament stance */
	UFUNCTION(BlueprintCallable, Category = "PlayerController|Combat")
	virtual void SetArmamentStance(EArmamentStance NextStance);

	/** Handles weapon equipping logic on the server */
	UFUNCTION(Server, Unreliable)
	virtual void Server_EquipWeapon(const bool bPrevWeapon = true, const bool bRightHand = false);

	/** Handles weapon equipping logic on the server */
	UFUNCTION(Server, Unreliable)
	virtual void Server_SetArmamentStance(const EArmamentStance NextStance);



	
//------------------------------------------------------------------------------------------------------//
// Utility																								//
//------------------------------------------------------------------------------------------------------//
public:
	/** Retrieves a reference to the character's base class for combat */
	UFUNCTION(BlueprintCallable, Category="PlayerController|Combat|Utility")
	virtual APlayerCharacter* GetPlayer();

	
};
