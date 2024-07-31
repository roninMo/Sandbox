// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Structs/AISenseInformation.h"
#include "Sandbox/Data/Structs/NpcInformation.h"
#include "Npc.generated.h"

class AAIControllerBase;

class UDataTable;
class USphereComponent;


/**
 * 
 */
UCLASS()
class SANDBOX_API ANpc : public ACharacterBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite) 
	TObjectPtr<AAIControllerBase> AIController;
	
	/**** The stats and equipment of the npc ****/
	/** The information specific to an npc character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment") F_NpcInformation CharacterInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment") FName Id; // The id of this npc character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment") UDataTable* NPCInformationTable; // The id of this npc character

	/**** Movement values ****/
	/** Helps with avoiding adjusting values with gameplay effects for different movement tasks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|AI") float MoveSpeed;
	
	/**** Other ****/
	/** Debug character information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bDebugCharacterInformation;

	
public:
	ANpc(const FObjectInitializer& ObjectInitializer);


protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;
	
	/** 
	 * Called when this Pawn is possessed. Only called on the server (or in standalone).
	 * @param NewController The controller possessing this pawn
	 */
	virtual void PossessedBy(AController* NewController) override;

	
//--------------------------------------------------------------------------------------------------------------------------//
// OnRepPlayerState/PossessedBy -> Or AI PossessedBy -> To this initialization loop											//
//--------------------------------------------------------------------------------------------------------------------------//
	/** Initializes global information that's not specific to any character. This should happen before any other initialization logic as a safeguard */
	virtual void InitCharacterGlobals(UDataAsset* Data) override;

	/** Initialize character components -> Get access to all the pointers, nothing else */
	virtual void InitCharacterComponents(const bool bCalledFromPossessedBy) override;

	/** Init ability system and attributes -> These are the most important components that tie everything together */
	virtual void InitAbilitySystemAndAttributes(const bool bCalledFromPossessedBy) override;
	
	/** InitCharacterInformation -> Run any logic necessary for the start of any of the components */
	virtual void InitCharacterInformation() override;

	/** Blueprint logic for character information initialization logic */
	// UFUNCTION(BlueprintImplementableEvent, Category = "AI", DisplayName = "Init Character Information (Blueprint)") void BP_InitCharacterInformation();

	
//----------------------------------------------------------------------------------------------------------------------//
// NPC Radius Periphery																									//
//----------------------------------------------------------------------------------------------------------------------//
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Periphery")
	TObjectPtr<USphereComponent> PeripheryRadius;

	/** The periphery radius is used for handling networking information, and updates information for specific clients using the periphery */ // TODO: Use net relevancy to handle this
	UPROPERTY(BlueprintReadWrite, Category = "Periphery") TArray<ACharacterBase*> CharactersInPeriphery;

public:
	/** Returns the characters in the npc's periphery */
	UFUNCTION(BlueprintCallable) virtual TArray<ACharacterBase*>& GetCharactersInPeriphery();
	
	
protected:
	/** The overlap function for entering the periphery radius */
	UFUNCTION() virtual void PeripheryEnterRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** The overlap function for entering the periphery radius */
	UFUNCTION() virtual void PeripheryExitRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/**
	 * Returns the npc character's information.
	 *
	 * @note the function is virtual to allow you to subclass the character information
	 */
	UFUNCTION(BlueprintCallable) virtual F_NpcInformation& GetNpcInformation();

	/**
	 * Sets the character information and other related values for the npc character
	 *
	 * @note the function is virtual to allow you to subclass the character information
	 */
	UFUNCTION(BlueprintCallable) virtual void SetNpcInformation(const F_NpcInformation& Information);
	
	/** Returns the AI Sense's configuration */
	UFUNCTION(BlueprintCallable) virtual F_AISenseConfigurations& GetSenseConfig();

	/** Returns the id of the npc character */
	UFUNCTION(Blueprintable) virtual FName GetId() const;
	
	
protected:
	/** Finds an inventory item and stores it in the player's inventory */
	UFUNCTION(BlueprintCallable) virtual void AddSavedItemToInventory(FS_Item Information);

	/** Gets the npc character information from the data table */
	UFUNCTION(BlueprintCallable) virtual F_NpcInformation GetNPCCharacterInformationFromTable(FName RowName);
	
	
};
