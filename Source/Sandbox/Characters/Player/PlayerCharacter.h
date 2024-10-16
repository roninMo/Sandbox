// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"
#include "Sandbox/Data/AbilityData.h"
#include "Sandbox/Data/AttributeData.h"
#include "PlayerCharacter.generated.h"



/**
 * 
 */
UCLASS()
class SANDBOX_API APlayerCharacter : public ACharacterCameraLogic
{
	GENERATED_BODY()

protected:
	/**** Character attributes and abilities ****/
	/** The id of the player's base attributes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Attributes and Abilities") FName AttributeInformationId;

	/** The id of the armor set to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Attributes and Abilities") FName AbilityDataId;

	
	/**** Database information ****/
	/** The id of the armor set to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Data Tables") UDataTable* AbilityInformationTable;
	
	/** The data table containing the information on the stats for different characters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Data Tables") UDataTable* AttributeInformationTable;

	
	/**** Stored references ****/
	/** The character's abilities and passive effects */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|Attributes and Abilities") UAbilityData* AbilityData;
	
	/** The character's attributes */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|Attributes and Abilities") UAttributeData* AttributeData;

	
	/** The handle for the character's abilities */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|Attributes and Abilities") FAbilityDataHandle AbilityDataHandle;
	
	/** The handle for the character's attributes */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|Attributes and Abilities") FAttributeDataHandle AttributeDataHandle;

	
	/**** Input Logic ****/	
	/** Input bindings for the ability pressed and released events. Don't forget to also add input mappings to the player's input mapping context */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Input") TArray<FInputActionAbilityMap> AbilityInputActions;

	
	
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
//----------------------------------------------------------------------//
// Initialization functions and components								//
//----------------------------------------------------------------------//
protected:
	virtual void BeginPlay() override;
	
	/**
	 * Initialized the Abilities' ActorInfo - the structure that holds information about who we are acting on and who controls us. \n\n
	 * 
	 * Invoked multiple times for both client / server, also depends on whether the Ability System Component lives on Pawns or Player States:
	 *		- Once for Server after component initialization
	 *		- Once for Server after replication of owning actor (Possessed by for Player State)
	 *		- Once for Client after component initialization
	 *		- Once for Client after replication of owning actor (Once more for Player State OnRep_PlayerState)
	 * 
	 * @param InOwnerActor			Is the actor that logically owns this component.
	 * @param InAvatarActor			Is what physical actor in the world we are acting on. Usually a Pawn but it could be a Tower, Building, Turret, etc, may be the same as Owner
	 */
	virtual void OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;


	
public:
	/** Creates an InputComponent that can be used for custom input bindings. Called upon possession by a PlayerController. Return null if you don't want one. */
	virtual UInputComponent* CreatePlayerInputComponent() override;

	/** Allows a Pawn to set up custom input bindings. Called upon possession by a PlayerController, using the InputComponent created by CreatePlayerInputComponent(). */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	

	
//----------------------------------------------------------------------------------//
// Movement																			//
//----------------------------------------------------------------------------------//
protected:

	
//-------------------------------------------------------------------------------------//
// Camera																			   //
//-------------------------------------------------------------------------------------//
protected:

	
//----------------------------------------------------------------------------------//
// Inventory																		//
//----------------------------------------------------------------------------------//
protected:
	
	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
	protected:
	/** Retrieves the attributes from the data table */
	UFUNCTION(BlueprintCallable) virtual UAttributeData* GetAttributeInformationFromTable(FName AttributeId);
	
	/** Retrieves the ability data from the data table */
	UFUNCTION(BlueprintCallable) virtual UAbilityData* GetAbilityInformationFromTable(FName AbilityId);

	
	
};
