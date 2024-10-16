// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/AI/Characters/Npc.h"
#include "Sandbox/Asc/Information/CharacterAbilityDataSet.h"
#include "Sandbox/Data/AbilityData.h"
#include "Sandbox/Data/AttributeData.h"
#include "Enemy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(EnemyLog, Log, All);



class UCaptainComponent;
class UWidgetComponent;
class UMMOAttributeSet;
class UEnemyEquipmentDataSet;
class UEquipmentData;
class UArmorData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayAttributeUpdated, float, Value);



/** Character Logic -> Combat Logic -> Group Ai logic


		Idle State
			-> Patrolling
			-> Resting/Gaurding

	-> has line of sight of enemy character
	-> if character moved away, use querying to navigate back towards the character
		Combat State
			-> Moving to Attack
			-> Strafing
			->> Attack combos


		

	Squad Attack Logic
		- When squad combat starts 
			-> Periphery senses if you're spotted or attack one of the character's in a squad

		- Different attacks
			-> Phlanx
			-> Multiple characters attack while another follows up with a strong attack
			-> Multiple characters attack while a mage activates a spell
			-> Phlanx around a mage while he charges a strong spell
			-> All characters teleport to surround the enemy and then attack the character
				

		- other logic
			-> Adjust formation (hurt character's fall back and healthy move forward)
				- When a character's health reaches a certain threshold

*/





/**
 * 
 */
UCLASS()
class SANDBOX_API AEnemy : public ANpc
{
	GENERATED_BODY()

protected:
	/** The enemy's current health, stamina, poise, and mana */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> StatsBarsWidgetComponent;


protected:
	/**** Combat information ****/
	/** The id of the attribute adjustments */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") FName AttributeInformationId;

	// Add ng+ leveling attribute adjustments

	// Add new region attribute adjustments

	/** The id of the equipment set to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") FName EquipmentInformationId;
	
	/** The id of the armor set to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") FName ArmorInformationId;

	/** The id of the armor set to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") FName AbilityDataId;

	// Add attack patterns

	
	/**** Database information ****/
	/** The id of the armor set to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") UDataTable* AbilityInformationTable;
	
	/** The data table containing the information on the stats for different characters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") UDataTable* AttributeInformationTable;
	
	/** The data table containing the information on equipment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") UDataTable* EquipmentInformationTable;

	/** The data table containing the information on armor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Information") UDataTable* ArmorInformationTable;

	
	
	/**** Stored references ****/
	/** The character's abilities and passive effects */
	UPROPERTY(Transient, BlueprintReadWrite) UAbilityData* AbilityData;
	
	/** The character's attributes */
	UPROPERTY(Transient, BlueprintReadWrite) UAttributeData* Stats;

	/** The character's equipment */
	UPROPERTY(Transient, BlueprintReadWrite) UEquipmentData* Equipment;
	
	/** The character's armor */
	UPROPERTY(Transient, BlueprintReadWrite) UArmorData* Armor;


	/** The handle for the character's abilities */
	UPROPERTY(Transient, BlueprintReadWrite) FAbilityDataHandle AbilityDataHandle;
	
	/** The handle for the character's attributes */
	UPROPERTY(Transient, BlueprintReadWrite) FAttributeDataHandle AttributeDataHandle;



	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UDataTable* CombatInformationTable;
	UPROPERTY(Transient, BlueprintReadWrite) UCharacterAbilityDataSet* AbilitiesAndStats;
	UPROPERTY(Transient, BlueprintReadWrite) UEnemyEquipmentDataSet* EquipmentAndArmor;
	UPROPERTY(Transient, BlueprintReadWrite) FCharacterAbilityDataSetHandle AbilitiesAndStatsHandle;

	
	// TODO: Refactor this into individual components to quickly test combat with different enemies.
	//			- I want variation with equipment, attributes, and attack patterns (and to easily be able to add/remove to multiple enemies with no problems)

	// Diverge combat characters into two different variations, and build for both. One with specific attack patterns, and one who's combat uses the weapon's attack patterns
	// Level and area information and adjustments


	
	/** The player controlled character */
	UPROPERTY(Transient, BlueprintReadWrite) TObjectPtr<ACharacterBase> Player;
	
	/** Overlapping components */
	UPROPERTY(Transient, BlueprintReadWrite) TArray<AActor*> ActorsToIgnore;

	

	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	AEnemy(const FObjectInitializer& ObjectInitializer);
	


	
//----------------------------------------------------------------------//
// Initialization functions and components								//
//----------------------------------------------------------------------//
protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;

	/** Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame. */
	virtual void Tick(float DeltaSeconds) override;

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

	/** Equip's the enemy's weapons and armor */
	virtual void InitCharacterEquipment();

	
	/** This keeps the client in sync with the enemy's attributes for widget to have access to this information */
	virtual void BindAttributeValuesToAscDelegates();
	

	/** Update the attribute values during specific events (Players in view, or if they get close enough to the ai's periphreals) */
	virtual void UpdateAttributeValues();
	
	/** Logic when a character registers it within it's periphery */
	virtual void WithinPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;

	/** Logic when a character unregisters it within it's periphery */
	virtual void OutsideOfPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType) override;



	
//--------------------------------------------------------------------------------------//
// Replicated values																	//
//--------------------------------------------------------------------------------------//
protected:
	/**** Character information replicated to the clients ****/
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealthUpdated, BlueprintReadWrite) float CurrentHealth;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentManaUpdated, BlueprintReadWrite) float CurrentMana;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentPoiseUpdated, BlueprintReadWrite) float CurrentPoise;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentStaminaUpdated, BlueprintReadWrite) float CurrentStamina;
	UPROPERTY(ReplicatedUsing=OnRep_MaxHealthUpdated, BlueprintReadWrite) float MaxHealth;
	UPROPERTY(ReplicatedUsing=OnRep_MaxManaUpdated, BlueprintReadWrite) float MaxMana;
	UPROPERTY(ReplicatedUsing=OnRep_MaxPoiseUpdated, BlueprintReadWrite) float MaxPoise;
	UPROPERTY(ReplicatedUsing=OnRep_MaxStaminaUpdated, BlueprintReadWrite) float MaxStamina;


public:
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnMaxHealthUpdated;
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnMaxManaUpdated;
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnMaxPoiseUpdated;
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnMaxStaminaUpdated;
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnHealthUpdated;
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnManaUpdated;
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnPoiseUpdated;
	UPROPERTY(BlueprintAssignable) FOnGameplayAttributeUpdated OnStaminaUpdated;
	

protected:
	// Updated attribute delegates for clients
	UFUNCTION() virtual void OnRep_CurrentHealthUpdated();
	UFUNCTION() virtual void OnRep_CurrentManaUpdated();
	UFUNCTION() virtual void OnRep_CurrentPoiseUpdated();
	UFUNCTION() virtual void OnRep_CurrentStaminaUpdated();
	UFUNCTION() virtual void OnRep_MaxHealthUpdated();
	UFUNCTION() virtual void OnRep_MaxManaUpdated();
	UFUNCTION() virtual void OnRep_MaxPoiseUpdated();
	UFUNCTION() virtual void OnRep_MaxStaminaUpdated();



	
//--------------------------------------------------------------------------------------//
// Utility																				//
//--------------------------------------------------------------------------------------//
public:
	/** Retrieves the attributes from the data table */
	UFUNCTION(BlueprintCallable) virtual UAttributeData* GetAttributeInformationFromTable(FName AttributeId);
	
	/** Retrieves the equipment from the data table */
	UFUNCTION(BlueprintCallable) virtual UEquipmentData* GetEquipmentInformationFromTable(FName EquipmentId);
	
	/** Retrieves the armor from the data table */
	UFUNCTION(BlueprintCallable) virtual UArmorData* GetArmorInformationFromTable(FName ArmorId);
	
	/** Retrieves the ability data from the data table */
	UFUNCTION(BlueprintCallable) virtual UAbilityData* GetAbilityInformationFromTable(FName AbilityId);

	/** Gets the npc character information from the data table */
	UFUNCTION(BlueprintCallable) virtual void GetCombatInformationFromTable(FName EquipmentId);
	
	
};
