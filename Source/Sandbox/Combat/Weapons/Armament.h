// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/World/Props/Items/Item.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Sandbox/Data/Structs/CombatInformation.h"
#include "Armament.generated.h"

enum class EHitStun : uint8;
DECLARE_LOG_CATEGORY_EXTERN(ArmamentLog, Log, All);


class ACharacterBase;
class UCombatComponent;
class UGameplayEffect;
struct F_ArmamentAbilityInformation;
struct FGameplayEffectInfo;
enum class EEquipSlot : uint8;
enum class EEquipStatus : uint8;
enum class ECharacterSkeletonMapping : uint8;
enum class EArmamentClassification : uint8;
enum class EDamageInformationSource : uint8;
enum class EEquipRestrictions : uint8;


/*

PrimaryAttack		- Lmb
SecondaryAttack		- Rmb
StrongAttack		- Shift + Lmb
SpecialAttack		- Shift + Rmb


	Left Hand Weapon
		- Attack (Secondary)

	Right Hand Weapon
		- Attack (PrimaryAttack / StrongAttack)
		- Special attack (various Stance/Combo chaining mixed with input abilities (just use the input component for your own combo configurations))

	Two handing a weapon
		- Attack (PrimaryAttack / StrongAttack)
		- Block (SecondaryAttack)
		- Special attack

	Dual wielding weapons
			- Combination attack (PrimaryAttack(L) / OtherPrimaryAttack(R) / StrongAttack(L) / SpecialAttack(L))
			- Individual attacks (PrimaryAttack(L, R) / StrongAttack(L, R) / StrongAttack(L) / SpecialAttack(L))


	What abilities are granted is updated every time another weapon is equipped, or the player changes his stance
		-


*/

/**
 *	An armament that's been spawned in the world and used for combat.
 */
UCLASS(BlueprintType, Category = "Combat")
class SANDBOX_API AArmament : public AItem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr <USkeletalMeshComponent> ArmamentMesh;

	/** The information for this armament */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Armament")
	F_ArmamentInformation ArmamentInformation;
	
	/** The current equip slot of the armament. Once the armament is created on the server, the updated equip slot is replicated, and we handle the rest based on the already replicated inventory info */
	UPROPERTY(ReplicatedUsing=OnRep_CreatedArmament, BlueprintReadWrite, Category = "Armament") EEquipSlot EquipSlot;
	
	/** The current equip slot of the armament @note this should be replicated using gameplay abilities */
	UPROPERTY(BlueprintReadWrite, Category = "Armament") EEquipStatus EquipStatus;

	/** The armament's ability handles */
	UPROPERTY(BlueprintReadWrite) TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	
	/** The armament's passive handles */
	UPROPERTY(BlueprintReadWrite) TArray<FActiveGameplayEffectHandle> PassiveHandles;
	
	/** The armament's state information handle */
	UPROPERTY(BlueprintReadWrite) FActiveGameplayEffectHandle StateInformationHandle;

	
	/**** Equip Information ****/
	/** The socket location for the left hand weapon when it's unequipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament|Equipping") FName LeftHandSheatheSocket;
	
	/** The socket location for the right hand weapon when it's unequipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament|Equipping") FName RightHandSheatheSocket;

	/** The socket location for the left hand weapon's holster */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament|Equipping") FName LeftHandHolsterSocket;
	
	/** The socket location for the right hand weapon's holster */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament|Equipping") FName RightHandHolsterSocket;

	/** We need a socket location for finding the center of the armament for impact calculations. This avoids sending extra information to the server */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament|Equipping") FName WeaponCenterLocationSocket = FName("center");;

	
//-------------------------------------------------------------------------------------//
// Armament Construction															   //
//-------------------------------------------------------------------------------------//
protected:
	AArmament();
	
	/** Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;
	
	
public:
	/** Function for equipping the armament. Add setup and teardown logic specific to this armament's configuration. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Init") virtual bool ConstructArmament();
	
	/** Function for unequipping the armament. Add setup and teardown logic specific to this armament's configuration. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Init") virtual bool DeconstructArmament();
	
	/** Function that checks whether the armament is valid to equip */
	UFUNCTION(BlueprintCallable, Category = "Armament|Init") virtual bool IsValidArmanent();


//-------------------------------------------------------------------------------------//
// Montages																			   //
//-------------------------------------------------------------------------------------//
protected:
	/** The ranged montages for the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TMap<EInputAbilities, UAnimMontage*> RangedMontages;

	/** The one handing melee montages for the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TMap<EInputAbilities, F_ArmamentComboInformation> MeleeMontages_OneHand;

	/** The two handing melee montages for the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TMap<EInputAbilities, F_ArmamentComboInformation> MeleeMontages_TwoHand;
	
	/** The dual wielding melee montages for the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TMap<EInputAbilities, F_ArmamentComboInformation> MeleeMontages_DualWield;
	
	/** The montages for the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TMap<FName, UAnimMontage*> Montages;

	
private:
	/** Dummy combo information in the event we don't have any montage info. This helps with having const functions that pass objects by reference (to prevent it from being costly) */
	F_ComboAttacks DummyMeleeComboInformation;


public:
	// TODO: Refactor because this is specific to melee armaments, or create functions to differentiate
	/**
	 * Retrieves the armament montages from the armament montage database, and adds them to the armament. If you need to retrieve a montage, use one of the get functions
	 *
	 * @param ArmamentMontageDB					The data table that contains the armament montages
	 * @param Link								The character skeleton to montage mapping reference  
	 */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils")
	virtual void SetArmamentMontagesFromDB(UDataTable* ArmamentMontageDB, ECharacterSkeletonMapping Link);

	/** Retrieves the attack montage for one of the armament's attacks */
	UFUNCTION(BlueprintCallable, Category = "Armament|Montages") virtual UAnimMontage* GetCombatMontage(const EInputAbilities AttackPattern);

	/** Retrieves the combo information for one of the armament's attacks based on the current weapon stance */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const F_ComboAttacks& GetComboAttacks(EInputAbilities AttackPattern) const;
	
	/** Retrieves one of the armament's montages */
	UFUNCTION(BlueprintCallable, Category = "Armament|Montages") virtual UAnimMontage* GetMontage(FName Montage);

	
//-------------------------------------------------------------------------------------//
// Armament equipping and unequipping												   //
//-------------------------------------------------------------------------------------//
public:
	/** Sheathes the weapon for the character */ 
	UFUNCTION(BlueprintCallable, Category = "Armament|Equipping") virtual bool SheatheArmament();

	/** Draw the character's sword */
	UFUNCTION(BlueprintCallable, Category = "Armament|Equipping") virtual bool UnsheatheArmament();


protected:
	/** Attach the armament to the character using socket locations. This works for both the character's hand, sheathe, or any other custom location */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual bool AttachArmamentToSocket(FName Socket);
	
	/** Retrieves the holster for a specific equipped armament */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual FName GetHolsterSocketName() const;
	
	/** Retrieves the sheathed for a specific equipped armament */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual FName GetSheathedSocketName() const;

	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Retrieves the armament attack information. This includes information on the armament and attack, and it's combo information. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const F_ArmamentInformation& GetArmamentInformation() const;
	
	/** Retrieves the database id of the armament */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual FName GetArmamentId() const;
	
	/** Retrieves the armament classification */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual EArmamentClassification GetClassification() const;
	
	/** Retrieves how we're handling damage calculations */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual EDamageInformationSource GetDamageCalculation() const;
	
	/** Retrieves the armament's equip restrictions */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual EEquipRestrictions GetEquipRestrictions() const;
	
	/** Retrieves the armament's abilities */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<F_ArmamentAbilityInformation>& GetMeleeAbilities() const;
	
	/** Retrieves the armament's abilities */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<FGameplayAbilityInfo>& GetAbilities() const;
	
	/** Retrieves the armament's passives */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<FGameplayEffectInfo>& GetPassives() const;
	
	/** Retrieves the armament's equipped state information */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual FGameplayEffectInfo GetStateInformation() const;
	
	/** Retrieves the armament's base attack information */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TMap<FGameplayAttribute, float>& GetBaseDamageStats() const;

	/** Returns different hitstuns based on the poise damage, the attack, the type of weapon, and other custom configurations */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual EHitStun GetHitStun(EInputAbilities AttackPattern, float PoiseDamage) const;
	
	/** Retrieves the armament's skeletal mesh */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual USkeletalMeshComponent* GetArmamentMesh() const;
	
	/** Retrieves the armament's equip slot */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual EEquipSlot GetEquipSlot() const;
	
	/** Retrieves the armament's equip status */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual EEquipStatus GetEquipStatus() const;

	/** Sets the armament's equip status */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual void SetEquipStatus(const EEquipStatus Status);
	
	/** Returns the armament's overlap components for their armament. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual TArray<UPrimitiveComponent*> GetArmamentHitboxes() const;

	/** Returns the location of the center of the armament */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual FVector GetCenterLocation() const;

	/** Shows or hides the armament and it's components for the player */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual void SetOwnerNoSee(const bool bHide = true);
	
	/** Get the armament attack information. This includes information on the armament and attack, and it's combo information. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual void SetArmamentInformation(const F_ArmamentInformation& Information);
	
	/** Set the slot this armament was equipped in. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual void SetArmamentEquipSlot(EEquipSlot Slot);

	/** Logic that happens on the client after the armament was created */
	UFUNCTION() virtual void OnRep_CreatedArmament();
	
	/** Retrieves the combat component from the character */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual UCombatComponent* GetCombatComponent(ACharacterBase* Character = nullptr) const;
	
	/** Retrieves the armament's current ability handles */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<FGameplayAbilitySpecHandle>& GetAbilityHandles() const;
	
	/** Retrieves the armament's current passive handles */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<FActiveGameplayEffectHandle>& GetPassiveHandles() const;
	
	/** Retrieves the armament's current state information handle */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const FActiveGameplayEffectHandle& GetStateInformationHandle() const;

	
};

