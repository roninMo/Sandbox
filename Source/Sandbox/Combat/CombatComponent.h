// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Sandbox/Data/Enums/ArmamentTypes.h"
#include "Sandbox/Data/Structs/ArmorInformation.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "CombatComponent.generated.h"

class ACharacterBase;
enum class ECombatAttribute : uint8;
enum class EHitDirection : uint8;
enum class EHitStun : uint8;
DECLARE_LOG_CATEGORY_EXTERN(CombatComponentLog, Log, All);


class AArmament;
class UDataTable;
class UCharacterAbilityDataSet;
enum class ECharacterSkeletonMapping : uint8;
enum class EEquipSlot : uint8;
enum class EArmorSlot : uint8;
struct FGameplayAttribute;
struct F_ArmamentAbilityInformation;
struct F_ArmamentInformation;	
struct FGAttributeSetExecutionData;


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



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCombatNotificationSignature, ACharacterBase*, Character, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeathSignature, ACharacterBase*, Character, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRespawnSignature, ACharacterBase*, Character, FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCombatDamageNotificationSignature, ACharacterBase*, Character, AActor*, Instigator, float, AccumulatedDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FCombatDamagesNotificationSignature, ACharacterBase*, Character, AActor*, Instigator, UObject*, Source, float, AccumulatedDamage, const FGameplayAttribute&, DamageAffinities);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FCombatPoiseBrokenSignature, ACharacterBase*, Character, AActor*, Instigator, EHitStun, HitStun, EHitDirection, HitDirection, float, PoiseDamage);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FArmamentEquippedSignature, AArmament*, Armament, EEquipSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FArmamentUnequippedSignature, FName, Armament, FGuid, Id, EEquipSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FArmorEquippedSignature, F_Item, Information, F_Information_Armor, Abilities, EArmorSlot, EquipSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FArmorUnequippedSignature, F_Item, Information, F_Information_Armor, Abilities, EArmorSlot, EquipSlot);




/*


	Player 
		- Health: 300
		- Stamina: 90
		- Poise: 45


		

	Weapon
		Stats			1h / 2h


	Shortswords
		- Damage:		100
		- Stamina:		11 / 13
		- Poise:		5 / 6.4
		- Poise (PvP):	67 / 87  (110 / 143) fh

	Longswords
		- Damage:		110
		- Stamina:		12 / 15
		- Poise:		5 / 6.4
		- Poise (PvP):	67 / 87  (110 / 143) fh

	Greatswords
		- Damage:		150
		- Stamina:		16 / 17
		- Poise:		10 / 14 
		- Poise (PvP):	114 / 149  (228 / 297) fh

	UltraGreatsword
		- Damage:		164
		- Stamina:		20 / 27
		- Poise:		14 / 18 
		- Poise (PvP):	354 / 458  (504 / 655) fh


*/




/**
 * Combat component for characters and enemies in the game


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
		- Weapon creates a damage calculation and sends it to the attribute logic
		- AttributeLogic handles adjusting attributes



	Weapons have information and logic that links different characters montages and armament information based on the player's current stance to the attack, and uses the player's stats and equipment to handle damage calculations
	Everything's captured during the attack, and there's lots of innovative logic for how you go about handling everything. Aside from the normal attacks, there's a lot of logic you can customize for making your own intersting abilities and combat
	I don't know the specifics, there's just some things I don't agree with for how the ability system handles logic, however this is kind of wild and a good way for handling combat, it just takes a moment to build up

 
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
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentStance) EArmamentStance CurrentStance;

	/** The current combat ability handles that are specific to the player's combat stance */
	UPROPERTY(Transient, BlueprintReadWrite) TArray<FGameplayAbilitySpecHandle> CombatAbilityHandles;
	
	/** The current combo index the player is on */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Combat Component|Armaments") int32 ComboIndex = 0;

	// TODO: Should these be replicated, or should we just use the inventory events to handle this?
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

	/** Function called every frame on this ActorComponent. Override this function to implement custom logic */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
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
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
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
	virtual void UpdateArmamentStanceAndAbilities();

	/**
	 * Updates the armament's current abilities based on the current stance, only adding/removing abilities that are required
	 *
	 * @param PreviousStance					The previous armament stance. Use this if you only want to remove the abilities that aren't used
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Combat")
	virtual void UpdateArmamentCombatAbilities(EArmamentStance PreviousStance = EArmamentStance::None);
	
	/** OnRep function for handling updating the current stance */
	UFUNCTION() virtual void OnRep_CurrentStance();
	
	/**
	 * Retrieves the armament from one of the player's hands
	 * 
	 * @param bRightHand						Whether we're retrieving the right or left hand armament
	 * @returns									The currently equipped armament for a specified hand
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils")
	virtual AArmament* GetArmament(const bool bRightHand = true) const;
	
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
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual bool UnequipArmor(EArmorSlot ArmorSlot);

	/** Creates and equips the player's armor */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual bool EquipArmor(F_Item Armor);

	
protected:
	/** Synchronization RPC's */
	UFUNCTION(Server, Reliable) virtual void Server_EquipArmor(const F_Item& Armor);


public:
	/** Retrieves the armor's information from the database */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual F_Item GetArmorItemInformation(EArmorSlot ArmorSlot);
	
	/** Retrieves the one of the current armor's ability information */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual F_Information_Armor GetArmorAbilityInformation(EArmorSlot ArmorSlot);
	
	/** Get's the skeletal mesh armor of a specific slot */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual USkeletalMeshComponent* GetArmorMesh(EArmorSlot ArmorSlot);
	
	/** Accesses the armor information from the database */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping")
	virtual const F_Information_Armor GetArmorFromDatabase(const FName Id) const;
	
	

	
//-------------------------------------------------------------------------------------//
// Combat																			   //
//-------------------------------------------------------------------------------------//
protected:
	/** The durations for the different HitStuns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") TMap<EHitStun, TSubclassOf<UGameplayEffect>> HitStunDurations;

	/** Gameplay effect for handling cleaning up/adding state and information when the player dies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* DeathEffect;

	/** Gameplay effect for handling cleaning up/adding state and information when the player respawns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* RespawnEffect;
	
	/** Status effect for state and information when the player is cursed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* CursedState;

	
	/** Status effect for state and information when the player has taken bleed damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* GE_Bled;

	/** Status effect for state and information when the player is poisoned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* GE_Poisoned;

	/** Status effect for state and information when the player is frostbitten */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* GE_Frostbitten;

	/** Status effect for state and information when the player is maddened */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* GE_Maddened;
	
	/** Status effect for state and information when the player has received the sleep effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Statuses") UGameplayEffect* GE_Slept;


	/**** Cached tags ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag HitStunEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag HitStunTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventHealthRegenEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventHealthRegen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventPoiseRegenEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventPoiseRegen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventStaminaRegenEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventStaminaRegen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventManaRegenEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventManaRegen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventCurseBuildupEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventCurseBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventBleedBuildupEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventBleedBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventPoisonBuildupEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventPoisonBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventFrostbiteBuildupEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventFrostbiteBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventMadnessBuildupEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventMadnessBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventSleepBuildupEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Tags") FGameplayTag PreventSleepBuildup;



	
public:
	/** On player attacked (player, instigator, accumulatedDamage, damageAffinities) */
	UPROPERTY(BlueprintAssignable) FCombatDamagesNotificationSignature OnPlayerAttacked;

	/** On player poise broken (player, instigator, hitStun, hitDirection) */
	UPROPERTY(BlueprintAssignable) FCombatPoiseBrokenSignature OnPoiseBroken;

	/** On player death (player, instigator, damage) */
	UPROPERTY(BlueprintAssignable) FOnDeathSignature OnDeath;

	/** On player respawn (player, location) */
	UPROPERTY(BlueprintAssignable) FOnRespawnSignature OnRespawn;

	
	/** On player cursed (player, instigator) */
	UPROPERTY(BlueprintAssignable) FCombatNotificationSignature OnCursed;
	
	/** On player bled (player, instigator, accumulatedDamage) */
	UPROPERTY(BlueprintAssignable) FCombatNotificationSignature OnBled;

	/** On player poisoned (player, instigator) */
	UPROPERTY(BlueprintAssignable) FCombatNotificationSignature OnPoisoned;

	/** On player frostbitten (player, instigator, accumulatedDamage) */
	UPROPERTY(BlueprintAssignable) FCombatNotificationSignature OnFrostBitten;

	/** On player maddened (player, instigator) */
	UPROPERTY(BlueprintAssignable) FCombatNotificationSignature OnMaddened;

	/** On player slept (player, instigator) */
	UPROPERTY(BlueprintAssignable) FCombatNotificationSignature OnSlept;


public:
	/** Logic for when a player takes damage */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleDamageTaken(ACharacterBase* Enemy, UObject* Source, float Value, const FGameplayAttribute Attribute);

	/** Handle varying hit reactions when a player's poise is broken */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PoiseBreak(ACharacterBase* Enemy, AActor* Source, float PoiseDamage, EHitStun HitStun, EHitDirection HitDirection);

	/** Logic for handling when the player dies */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleDeath(ACharacterBase* Enemy, AActor* Source, FName MontageSection);

	/** Logic for handling when the player respawns */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleRespawn(ACharacterBase* Enemy, AActor* Source, const UCharacterAbilityDataSet* RespawnInformation);


	// TODO: These can all be handled in the same function
	/** Handle the logic that happens when the player is cursed */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleCurse(ACharacterBase* Enemy, AActor* Source);

	/** Logic when the player takes bleed damage */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleBleed(ACharacterBase* Enemy, AActor* Source);
	
	/** Logic for when the player is poisoned */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandlePoisoned(ACharacterBase* Enemy, AActor* Source);
	
	/** Handle the logic when the player is frostbitten */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleFrostbite(ACharacterBase* Enemy, AActor* Source);
	
	/** Handle the logic when the player receives madness status */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleMadness(ACharacterBase* Enemy, AActor* Source);
	
	/** Handle the logic when the player receives sleep status */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleSleep(ACharacterBase* Enemy, AActor* Source);
	

	/**
	 * Returns a gameplay effect to prevent an attribute from regenerating for a specific duration
	 *
	 * @param Duration							The duration that the attribute isn't allowed to regenerate for
	 * @param Attribute							The attribute we're preventing from regenerating
	 * @returns									A gameplay effect for preventing attribute regeneration
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Utils")
	virtual UGameplayEffect* GetPreventAttributeAccumulationEffect(float Duration, const ECombatAttribute Attribute) const;
	
	/**
	 * Returns a gameplay effect for handling the hitstun duration once a player's been attacked.
	 * TODO: This needs to be a value that's sent across the net from the server, or we need to find a way to predict this
	 *
	 * @param HitStun							The variation of HitStun, used to determine the duration of a hitstun, to keep in sync with the montage
	 * @returns									A gameplay effect for adding a HitStun duration to the player
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Utils")
	virtual TSubclassOf<UGameplayEffect> GetHitStunDurationEffect(EHitStun HitStun) const;

	/**
	 * Returns a gameplay effect to prevent certain attributes from regenerating for a specific duration
	 *
	 * @param Duration							The duration that the attributes aren't allowed to regenerate for
	 * @param Attributes						The attributes we're preventing from regenerating
	 * @returns									A gameplay effect for preventing attribute regeneration
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Utils")
	virtual UGameplayEffect* GetPreventAttributesAccumulationEffect(float Duration, const TArray<ECombatAttribute>& Attributes) const;
	
	

	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Retrieves a socket from the character for equipping weapons, etc. */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual const USkeletalMeshSocket* GetSkeletalSocket(FName SocketName) const;
	
	/** Retrieves the equipped socket for a specific armament */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual FName GetEquippedSocketName(EArmamentClassification Armament, EEquipSlot EquipSlot) const;

	/** Retrieves the player's stance */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils") virtual EArmamentStance GetCurrentStance() const;

	/** Adds a combat ability if the ability is valid for the player's current stance. This is a utility function for adding the combat abilities */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Utils") virtual void AddCombatAbilityIfValidStance(TMap<EInputAbilities, F_ArmamentAbilityInformation>& Map, const F_ArmamentAbilityInformation& Ability);

	/** Retrieves the attribute prevention tag for a specific attribute */
	virtual FGameplayTag GetAttributePreventionTag(ECombatAttribute Attribute, bool bStateTag = true) const;
	
	/** Sets the player's current combo index */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Combat") virtual void SetComboIndex(const int32 Index);

	/** Retrieves the current combo index */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Combat") virtual int32 GetComboIndex() const;
	
	/** Retrieves the armament's montage table */
	UFUNCTION(BlueprintCallable) virtual UDataTable* GetArmamentMontageTable() const;
	
};





