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
