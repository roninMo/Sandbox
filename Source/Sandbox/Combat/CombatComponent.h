// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sandbox/Data/Structs/ArmorInformation.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "CombatComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(CombatComponentLog, Log, All);

// Equip Sockets
#define Socket_LeftHandEquip FName("hand_lSocket")
#define Socket_RightHandEquip FName("hand_rSocket")

#define Socket_Sheathe_1h_blade_l FName("sheathe_1h_b_l");
#define Socket_Holster_1h_blade_l FName("holster_1h_b_l");
#define Socket_Sheathe_1h_blade_r FName("sheathe_1h_b_r");
#define Socket_Holster_1h_blade_r FName("holster_1h_b_r");

#define Socket_Sheathe_2h_blade FName("sheathe_2h_b");
#define Socket_Holster_2h_blade FName("holster_2h_b");

#define Socket_Holster_pistol FName("holster_pistol"); // with a silencer!
#define Socket_Holster_rifle FName("holster_rifle");
#define Socket_Holster_shotgun FName("holster_shotgun");


class AArmament;
class UDataTable;
struct F_ArmamentInformation;
struct FGAttributeSetExecutionData;
enum class EComboType : uint8;
enum class ECharacterToMontageMapping : uint8;
enum class EArmamentStance : uint8;
enum class EEquipSlot : uint8;
enum class EArmorSlot : uint8;
enum class EArmamentClassification : uint8;




DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FArmamentEquippedSignature, AArmament*, Armament, EEquipSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FArmamentUnequippedSignature, FName, Id, EEquipSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FArmorEquippedSignature, F_Item, Information, F_Information_Armor, Abilities, EArmorSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FArmorUnequippedSignature, F_Item, Information, F_Information_Armor, Abilities, EArmorSlot, EquipSlot);


/**
 * Combat component for characters and enemies in the game
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/**** Armaments And equip slots */
	/** The primary armament the player has equipped */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat Component|Armaments") AArmament* PrimaryArmament;
	
	/** The secondary armament the player has equipped */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat Component|Armaments") AArmament* SecondaryArmament;

	/** The player's current armament stance */
	UPROPERTY(Transient, BlueprintReadWrite) EArmamentStance CurrentStance;
	
	/** The armament information for the armament in the left hand's first equip slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_One;
	
	/** The armament information for the armament in the left hand's second equip slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_Two;
	
	/** The armament information for the armament in the left hand's third equip slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item LeftHandEquipSlot_Three;
	
	/** The armament information for the armament in the right hand's first equip slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_One;
	
	/** The armament information for the armament in the right hand's second equip slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_Two;
	
	/** The armament information for the armament in the right hand's third equip slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armaments") F_Item RightHandEquipSlot_Three;

	
	/**** Armor ****/
	/** The currently equipped gauntlets's information */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Gauntlets;
	
	/** The currently equipped leggings's information */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Leggings;
	
	/** The currently equipped helm's information */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Helm;
	
	/** The currently equipped chest's information */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Component|Armors") F_Item Chest;

	/** The ability handles for the currently equipped armors */
	UPROPERTY(Transient, BlueprintReadWrite) TMap<EArmorSlot, F_Information_Armor> ArmorAbilities;
	UPROPERTY(Transient, BlueprintReadWrite) TMap<EArmorSlot, F_Information_Armor_Handle> ArmorAbilityHandles;

	
	/**** Data tables for information and retrieval ****/
	/** The data table for retrieving an armament's combat information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component") UDataTable* ArmamentInformationTable;
	
	/** The data table for retrieving information for armor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component") UDataTable* ArmorInformationTable;
	
	/** The data table for retrieving armament montages for specific characters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component") TObjectPtr<UDataTable> MontageInformationTable;
	
	/**** Other ****/
	/** The primary armament index These are just used specifically for handling switching weapons */
	UPROPERTY(Transient, BlueprintReadWrite) int32 ArmamentIndex;

	/** The secondary armament index These are just used specifically for handling switching weapons */
	UPROPERTY(Transient, BlueprintReadWrite) int32 OffhandArmamentIndex;

	
public:
	/** Delegate for when a player equips an armament */
	UPROPERTY(BlueprintAssignable)
	FArmamentEquippedSignature OnEquippedArmament;

	/** Delegate for when a player unequips an armament */
	UPROPERTY(BlueprintAssignable)
	FArmamentUnequippedSignature OnUnequippedArmament;
	
	/** Delegate for when a player equips an armor */
	UPROPERTY(BlueprintAssignable)
	FArmorEquippedSignature OnEquippedArmor;

	/** Delegate for when a player unequips an armor */
	UPROPERTY(BlueprintAssignable)
	FArmorUnequippedSignature OnUnequippedArmor;
	
	
protected:
	UCombatComponent();
	
	/** Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;

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
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual void AddArmamentToEquipSlot(const F_Item& ArmamentInventoryInformation, const EEquipSlot EquipSlot);
	
	/** Remove an armament from one of the character's armament slots */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual void RemoveArmamentFromEquipSlot(const EEquipSlot EquipSlot);

	/**
	 * Creates an armament from one of it's equip slots and equips it to one of the character's active armament hands. Only call on authority. \n\n
	 *
	 * Handles spawning the armament, calling @ref ConstructArmament() and if it successfully creates and constructs the armament, it equips and returns the armament. Otherwise, reverts the creation and returns nullptr
	 * @note If you call this before unequipping the armament from the specified equip slot, it fails to equip the armament // TODO: Add validity checks with the player's inventory
	 *
	 * @param EquipSlot							The equip slot to retrieve the armament information from.
	 * @returns									The created armament
	 **/
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Init")
	virtual AArmament* CreateArmament(const EEquipSlot EquipSlot);

	/**
	 * Unequips an equipped armament, removing the armament and it's abilities from the character. Only call on authority. \n\n
	 * Handles removing the armament, calling @ref DeconstructArmament() and then destroying the armament and handling any cleanup afterwards. If it fails to deconstruct, it returns false.
	 * 
	 * @param Armament							The armament we're removing
	 * @returns									Whether the armament was successfully removed
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Init")
	virtual bool DeleteEquippedArmament(AArmament* Armament);
	
	/**
	 * Sets the armament stance for the character. This is more for the player's input driven events for how they want to wield the armament
	 * @param Stance							The player's current combat stance
	 */
	UFUNCTION(BlueprintCallable, Category = "Armaments")
	virtual void SetArmamentStance(EArmamentStance Stance);

	/** Updates the armament stance for the character based on the currently equipped armaments. */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Combat")
	virtual void UpdateArmamentStance();
	
	/**
	 * Retrieves the armament from one of the player's hands
	 * 
	 * @param bRightHand						Whether we're retrieving the right or left hand armament
	 * @returns									The currently equipped armament for a specified hand
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils")
	virtual AArmament* GetArmament(const bool bRightHand = true);
	
	/**
	 * Returns whether this is the right handed armament
	 *
	 * @param Slot								The equip slot
	 * @returns									True if it's a right hand slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils")
	virtual bool IsRightHandedArmament(EEquipSlot Slot) const;
	
	/**
	 * Retrieves the currently equipped slot for a specific hand
	 *
	 * @param bRightHand						Whether we're retrieving the current equip slot of the right or left hand
	 * @returns									The current equip slot for a specified hand
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils")
	virtual EEquipSlot GetCurrentlyEquippedSlot(bool bRightHand = true);
	
	/**
	 * Retrieves the next equipped slot for a specific hand
	 * 
	 * @param bRightHand						Whether we're retrieving the next equip slot of the right or left hand
	 * @returns									The next equip slot for a specified hand
	 */ 
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils")
	virtual EEquipSlot GetNextEquipSlot(bool bRightHand = true);
	
	/**
	 * Retrieves the inventory information of a specific armament slot
	 *
	 * @param Slot								The armament's equip slot
	 * @returns									The inventory information of the armament equipped to that slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils")
	virtual F_Item GetArmamentInventoryInformation(EEquipSlot Slot);

	/**
	 * Retrieves the armament's information from the database
	 *
	 * @param ArmamentId						The id of the armament
	 * @returns									The armament information
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils") 
	virtual F_ArmamentInformation GetArmamentInformationFromDatabase(FName ArmamentId);

	/**
	 * Retrieves the armament montage from the armament montage database. For ranged weapons, use EComboType::None
	 *
	 * @param ArmamentId						The id of the armament
	 * @param ComboType							The combo montage we're retrieving. If the armament doesn't use combos, just use EComboType::None to retrieve it's montage
	 * @param Mapping							The character skeleton to montage mapping reference  
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils")
	virtual UAnimMontage* GetArmamentMontageFromDB(FName ArmamentId, EComboType ComboType, ECharacterToMontageMapping Mapping);
	

protected:
	/**** Synchronization RPC's ****/
	UFUNCTION(Server, Reliable) virtual void Server_AddArmamentToEquipSlot(const F_Item& ArmamentData, const EEquipSlot EquipSlot);
	UFUNCTION(Server, Reliable) virtual void Server_RemoveArmamentFromEquipSlot(const EEquipSlot EquipSlot);
	UFUNCTION(Server, Reliable) virtual void Server_CreateArmament(const EEquipSlot EquipSlot);
	


	
//--------------------------------------------------------------------------------------//
// Armor Functionality																	//
//--------------------------------------------------------------------------------------//
public:
	/** Unequips one of the player's armors. */
	virtual bool UnequipArmor(EArmorSlot ArmorSlot);

	/** Creates and equips the player's armor */
	virtual bool EquipArmor(F_Item Armor);

	
	/** Retrieves the armor's information from the database */
	virtual F_Item GetArmorItemInformation(EArmorSlot ArmorSlot);
	
	/** Retrieves the one of the current armor's ability information */
	virtual F_Information_Armor GetArmorAbilityInformation(EArmorSlot ArmorSlot);
	
	/** Get's the skeletal mesh armor of a specific slot */
	virtual USkeletalMeshComponent* GetArmorMesh(EArmorSlot ArmorSlot);
	
	/** Accesses the armor information from the database */
	virtual const F_Information_Armor GetArmorFromDatabase(const FName Id) const;
	
	
protected:
	/** Synchronization RPC's */
	UFUNCTION(Server, Reliable) virtual void Server_EquipArmor(const F_Item& Armor);
	
	

	
	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Retrieves a socket from the character for equipping weapons, etc. */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual const USkeletalMeshSocket* GetSkeletalSocket(FName SocketName) const;
	
	/** Retrieves the equipped socket for a specific armament */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual FName GetEquippedSocketName(EArmamentClassification Armament, EEquipSlot EquipSlot) const;
	
	
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

