// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Structs/NpcInformation.h"
// #include "Sandbox/Data/Structs/AISenseInformation.h" // Included in Npc Information
// #include "Sandbox/Data/Structs/InventoryInformation.h" // Included in Npc Information
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
	/** The id of this npc character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Attributes, Equipment, Abilities and Armor") FName Id;

	/** The data table containing the information on every npc character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Data Tables") UDataTable* NPCInformationTable;

	/** The information specific to an npc character */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Attributes, Equipment, Abilities and Armor") F_NpcInformation CharacterInformation;
	
	
	/**** Movement values ****/
	/** Helps with avoiding adjusting values with gameplay effects for different movement tasks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|AI") float MoveSpeed;

	
	/**** Other ****/
	/** Debug character information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Debug") bool bDebugCharacterInformation;

	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	ANpc(const FObjectInitializer& ObjectInitializer);

	

	
//----------------------------------------------------------------------//
// Initialization functions and components								//
//----------------------------------------------------------------------//
protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;
	
	/** 
	 * Called when this Pawn is possessed. Only called on the server (or in standalone).
	 * @param NewController The controller possessing this pawn
	 */
	virtual void PossessedBy(AController* NewController) override;

	/** PlayerState Replication Notification Callback */ // Only use on ai character's during custom games (tdm, or anything where we need to keep track of a player information)
	// virtual void OnRep_PlayerState() override;

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



	
//----------------------------------------------------------------------------------------------------------------------//
// NPC Radius Periphery																									//
//----------------------------------------------------------------------------------------------------------------------//
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Periphery")
	TObjectPtr<USphereComponent> PeripheryRadius;

	/** The periphery radius is used for handling networking information outside of net relevancy, and updates information for specific clients using the periphery */
	UPROPERTY(BlueprintReadWrite, Category = "Periphery") TArray<ACharacterBase*> CharactersInPeriphery;

public:
	/** Returns the characters in the npc's periphery */
	UFUNCTION(BlueprintCallable) virtual TArray<ACharacterBase*>& GetCharactersInPeriphery();
	
	
protected:
	/** The overlap function for entering the periphery radius */
	UFUNCTION() virtual void PeripheryEnterRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** The overlap function for entering the periphery radius */
	UFUNCTION() virtual void PeripheryExitRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/** This calculates whether an ai character has sensed this player, and uses the default logic with an offset for accurate traces  */
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate) override;

	

	
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

	virtual FString GetActorLevelId_Implementation() const override;

	
protected:
	/** Finds an inventory item and stores it in the player's inventory */
	UFUNCTION(BlueprintCallable) virtual void AddSavedItemToInventory(FS_Item Information);

	/** Gets the npc character information from the data table */
	UFUNCTION(BlueprintCallable) virtual F_NpcInformation GetNPCCharacterInformationFromTable(FName RowName);
	
	
};
