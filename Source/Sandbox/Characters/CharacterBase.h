// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UInventoryComponent;
class UAdvancedMovementComponent;

/*
 * The universal class for characters, npc's, and enemies in the game
 */
UCLASS()
class SANDBOX_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	
//----------------------------------------------------------------------//
// Initialization functions and components								//
//----------------------------------------------------------------------//
protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;
	
	
//----------------------------------------------------------------------------------//
// Movement																			//
//----------------------------------------------------------------------------------//
protected:
	/** Templated convenience version for retrieving the movement component. */
	template<class T> T* GetMovementComp(void) const { return Cast<T>(GetMovementComponent()); }

	/** Retrieves the advanced movement component */
	UFUNCTION(BlueprintCallable, Category="Movement", DisplayName="Get Character Movement Component")
	virtual UAdvancedMovementComponent* GetAdvancedCharacterMovementComponent() const;

	
//--------------------------------------------------------------------------------------------------------------------------//
// OnRepPlayerState/PossessedBy -> Or AI PossessedBy -> To this initialization loop											//
//--------------------------------------------------------------------------------------------------------------------------//
protected:
	/** Initializes global information that's not specific to any character. This should happen before any other initialization logic as a safeguard */
	virtual void InitCharacterGlobals(UDataAsset* Data);

	/** Initialize character components -> Get access to all the pointers, nothing else */
	virtual void InitCharacterComponents(const bool bCalledFromPossessedBy);

	/** Init ability system and attributes -> These are the most important components that tie everything together */
	virtual void InitAbilitySystemAndAttributes(const bool bCalledFromPossessedBy);
	
	/** InitCharacterInformation -> Run any logic necessary for the start of any of the components */
	virtual void InitCharacterInformation();
	

//-------------------------------------------------------------------------------------//
// Peripheries																		   //
//-------------------------------------------------------------------------------------//
protected:

	
//-------------------------------------------------------------------------------------//
// Camera																			   //
//-------------------------------------------------------------------------------------//
protected:

	
//----------------------------------------------------------------------------------//
// Inventory																		//
//----------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UInventoryComponent> Inventory;
	
	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
protected:
	
	
};
