// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sandbox/Data/Structs/ArmorInformation.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "CombatComponent.generated.h"

struct F_ArmamentInformation;
enum class EArmamentStance : uint8;
class AArmament;
class UDataTable;
struct FGAttributeSetExecutionData;
enum class EEquipSlot : uint8;
enum class EArmorSlot : uint8;
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

protected:
	// Armaments And equip slots
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat Component|Armaments") AArmament* PrimaryArmament;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat Component|Armaments") AArmament* SecondaryArmament;

	UPROPERTY(BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_One;
	UPROPERTY(BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_Two;
	UPROPERTY(BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_Three;
	UPROPERTY(BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_One;
	UPROPERTY(BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_Two;
	UPROPERTY(BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_Three;

	// Armor
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Gauntlets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Leggings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Helm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Chest;
	UPROPERTY(BlueprintReadWrite, Category = "Combat Component|Armors") TMap<EArmorSlot, F_Information_Armor_Handle> ArmorAbilityHandles;

	// Data table for information and retrieval
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament Information") UDataTable* ArmamentInformationTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament Information") UDataTable* ArmorInformationTable;
	
	
public:
	/** Delegate for when a player equips an armament */
	UPROPERTY(BlueprintAssignable) FArmamentEquippedSignature OnEquippedArmament;

	/** Delegate for when a player unequips an armament */
	UPROPERTY(BlueprintAssignable) FArmamentUnequippedSignature OnUnequippedArmament;
	
	
protected:
	UCombatComponent();
	
	/** Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;

	/** Called before destroying the object. This is called immediately upon deciding to destroy the object, to allow the object to begin an asynchronous cleanup process. */
	virtual void BeginDestroy() override;
	
	/** Handles combat calculations and logic */
	virtual void CombatCalculations(const FGAttributeSetExecutionData& Props);



	
//-------------------------------------------------------------------------------------//
// Armament Functionality															   //
//-------------------------------------------------------------------------------------//
public:

	/**
	 * Add an armament to one of the character's armament slots. Will only overwrite the current information if the armament is valid
	 *
	 * @param ArmamentInventoryInformation		A copy of the armament's inventory for quick access and armament information retrieval
	 * @param EquipSlot							The slot we're equipping the armament to
	 */
	virtual void AddArmamentToEquipSlot(const F_Item& ArmamentInventoryInformation, const EEquipSlot EquipSlot);
	
	/** Remove an armament from one of the character's armament slots */
	virtual void RemoveArmamentFromEquipSlot(const EEquipSlot EquipSlot);

	/**
	 * Creates an armament from one of it's equip slots and equips it to one of the character's active armament hands. Only call on authority. \n\n
	 *
	 * Handles spawning the armament, calling @ref ConstructArmament() and if it successfully creates and constructs the armament, it equips and returns the armament. Otherwise, reverts the creation and returns nullptr
	 * @note If you call this before unequipping the armament from the specified equip slot, it fails to equip the armament
	 *
	 * @param EquipSlot							The equip slot to retrieve the armament information from.
	 * @returns									The created armament
	 **/
	virtual AArmament* CreateArmament(const EEquipSlot EquipSlot);

	/**
	 * Unequips an equipped armament, removing the armament and it's abilities from the character. Only call on authority. \n\n
	 * Handles removing the armament, calling @ref DeconstructArmament() and then destroying the armament and handling any cleanup afterwards. If it fails to deconstruct, it returns false.
	 * 
	 * @param Armament							The armament we're removing
	 * @returns									Whether the armament was successfully removed
	 */
	virtual bool DeleteEquippedArmament(AArmament* Armament);
	
	/**
	 * Sets the Armament stance for the character. This should happen on equip and when the player sets how he's wielding the armament
	 * @param Stance							The player's current combat stance
	 */
	virtual void SetArmamentStance(EArmamentStance Stance);
	
	/**
	 * Retrieves the armament from one of the player's hands
	 * 
	 * @param bRightHand						Whether we're retrieving the right or left hand armament
	 * @returns									The currently equipped armament for a specified hand
	 */
	virtual AArmament* GetArmament(const bool bRightHand = true);
	
	/**
	 * Returns whether this is the right handed armament
	 *
	 * @param Slot								The equip slot
	 * @returns									True if it's a right hand slot
	 */
	virtual bool IsRightHandedArmament(EEquipSlot Slot);
	
	/**
	 * Retrieves the currently equipped slot for a specific hand
	 *
	 * @param bRightHand						Whether we're retrieving the current equip slot of the right or left hand
	 * @returns									The current equip slot for a specified hand
	 */
	virtual EEquipSlot GetCurrentlyEquippedSlot(bool bRightHand = true);
	
	/**
	 * Retrieves the next equipped slot for a specific hand
	 * 
	 * @param bRightHand						Whether we're retrieving the next equip slot of the right or left hand
	 * @returns									The next equip slot for a specified hand
	 */ 
	virtual EEquipSlot GetNextEquipSlot(bool bRightHand = true);
	
	/**
	 * Retrieves the inventory information of a specific armament slot
	 *
	 * @param Slot								The armament's equip slot
	 * @returns									The inventory information of the armament equipped to that slot
	 */
	virtual F_Item GetArmamentInventoryInformation(EEquipSlot Slot);

	/**
	 * Get the armament information of a specific slot
	 *
	 * @param Slot								The armament's equip slot
	 * @returns									The armament information of the armament equipped to that slot
	 */
	virtual F_ArmamentInformation GetArmamentInformation(EEquipSlot Slot);


protected:
	// 	/** Synchronization RPC's (Most of the combat data is not replicated and is handled during interaction or on save (weapons/inventory) */
	// 	UFUNCTION(Server, Reliable) void Server_AddArmamentToEquipSlot(F_InventoryItem ArmamentData, EEquipSlot EquipSlot);
	// 	UFUNCTION(Server, Reliable) virtual void Server_CreateArmament(EEquipSlot EquipSlot);



	
//--------------------------------------------------------------------------------------//
// Armor Functionality																	//
//--------------------------------------------------------------------------------------//
public:

	// EquipArmor
	// UnequipArmor
	
	// GetArmorItemInformation
	// GetArmorAbilityInformation
	// GetArmorMesh
	// GetArmorFromDatabase

	
// public:
// 	/** Add the abilities of this armor */
// 	virtual void AddArmorAbilitiesStatsAndBuffs(F_Information_Armor& ArmorInformation) override;
//
// 	/** Remove the abilities of this armor */
// 	virtual void RemoveArmorAbilitiesStatsAndBuffs(F_Information_Armor& ArmorInformation) override;
//
// 	/** Creates a gameplay effect to add or remove armor stats from the character */
// 	virtual bool CreateAndAddArmorStatsEffect(EArmorType ArmorType, TMap<EAttribute, float>& ArmorStats, bool bEquipping);
// 	
// 	/** Unequip an active armament from one of the character's currently equipped armor, removing the armor and it's abilities from the character. */
// 	virtual bool UnequipArmor(EArmorType ArmorSlot) override;
//
// 	/** Creates and equips the player's armor */
// 	virtual bool EquipArmor(F_InventoryItem Armor) override;
// 	
// 	/** Accesses the armor slot from the type of armor */
// 	virtual F_InventoryItem GetArmorItemInformation(EArmorType ArmorType) override;
// 	
// 	/** Accesses the armor information from the database */
// 	virtual const F_Information_Armor GetArmorInformationFromDatabase(const FName RowName) const override;
//
// 	/** Get's the skeletal mesh armor of a specific slot */
// 	virtual USkeletalMeshComponent* GetArmorMesh(EArmorType ArmorType);
//
// 	/** Adds the armor's inventory item to the combat component for reference */
// 	virtual void SetArmorInformation(F_InventoryItem Item, EArmorType ArmorType);
//
// 	/** Finds the armor information from the item */ // This might be helpful for widgets that need access to the armor information 
// 	virtual F_Information_Armor GetArmorInformationFromItem(F_InventoryItem Item);
// 	
// 	/** Synchronization RPC's (Most of the combat data is not replicated and is handled during interaction or on save (weapons/inventory) */
// 	UFUNCTION(Server, Reliable) virtual void Server_EquipArmor(const F_InventoryItem& Armor);
	
	
	
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

