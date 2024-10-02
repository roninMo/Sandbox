// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/World/Props/Items/Item.h"
#include "Sandbox/Data/Structs/CombatInformation.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Armament.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ArmamentLog, Log, All);


class ACharacterBase;
class UCombatComponent;
class UGameplayEffect;
struct F_ArmamentAbilityInformation;
struct FGameplayEffectInfo;
enum class EEquipSlot : uint8;
enum class EAttackPattern : uint8;
enum class ECharacterSkeletonMapping : uint8;
enum class EArmamentClassification : uint8;
enum class EDamageInformationSource : uint8;
enum class EEquipRestrictions : uint8;


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armaments")
	F_ArmamentInformation ArmamentInformation;
	
	/** The current equip slot of the armament */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Armaments") EEquipSlot EquipSlot;

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
	UFUNCTION(BlueprintCallable, Category = "Armaments|Init") virtual bool ConstructArmament();
	
	/** Function for unequipping the armament. Add setup and teardown logic specific to this armament's configuration. */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Init") virtual bool DeconstructArmament();
	
	/** Function that checks whether the armament is valid to equip */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Init") virtual bool IsValidArmanent();


//-------------------------------------------------------------------------------------//
// Montages																			   //
//-------------------------------------------------------------------------------------//
protected:

	/** The montages for the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TMap<EAttackPattern, UAnimMontage*> ArmamentMontages;


public:
	/** Updates the armament's montages provided a character to montage reference */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Montages") virtual bool UpdateArmamentMontages(const ECharacterSkeletonMapping MontageMapping);
	
	
//-------------------------------------------------------------------------------------//
// Armament equipping and unequipping												   //
//-------------------------------------------------------------------------------------//
public:
	/** Sheathes the weapon for the character */ 
	UFUNCTION(BlueprintCallable, Category = "Armaments|Equipping") virtual bool SheatheArmament();

	/** Draw the character's sword */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Equipping") virtual bool UnsheatheArmament();


protected:
	/** Attach the armament to the character using socket locations. This works for both the character's hand, sheathe, or any other custom location */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual bool AttachArmamentToSocket(FName Socket);
	
	/** Retrieves the holster for a specific equipped armament */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual FName GetHolsterSocketName() const;
	
	/** Retrieves the sheathed for a specific equipped armament */
	UFUNCTION(BlueprintCallable, Category = "Combat Component|Equipping") virtual FName GetSheathedSocketName() const;

	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Retrieves the armament attack information. This includes information on the armament and attack, and it's combo information. */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual const F_ArmamentInformation& GetArmamentInformation() const;
	
	/** Retrieves the database id of the armament */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual FName GetArmamentId() const;
	
	/** Retrieves the armament classification */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual EArmamentClassification GetClassification() const;
	
	/** Retrieves how we're handling damage calculations */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual EDamageInformationSource GetDamageCalculation() const;
	
	/** Retrieves the armament's equip restrictions */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual EEquipRestrictions GetEquipRestrictions() const;
	
	/** Retrieves the armament's abilities */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual const TArray<F_ArmamentAbilityInformation>& GetAbilities() const;
	
	/** Retrieves the armament's passives */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual const TArray<FGameplayEffectInfo>& GetPassives() const;
	
	/** Retrieves the armament's equipped state information */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual FGameplayEffectInfo GetStateInformation() const;
	
	/** Retrieves the armament's base attack information */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual const TMap<FGameplayAttribute, float>& GetBaseDamageStats() const;
	
	/** Retrieves the armament's skeletal mesh */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") USkeletalMeshComponent* GetArmamentMesh() const;
	
	/** Retrieves the armament's equip slot */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") EEquipSlot GetEquipSlot() const;
	
	/** Returns the armament's overlap components for their armament. */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual TArray<UPrimitiveComponent*> GetArmamentHitboxes() const;
	
	
	/** Get the armament attack information. This includes information on the armament and attack, and it's combo information. */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual void SetArmamentInformation(const F_ArmamentInformation& Information);
	
	/** Set the slot this armament was equipped in. */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual void SetArmamentEquipSlot(EEquipSlot Slot);

	/** Retrieves the combat component from the character */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual UCombatComponent* GetCombatComponent(ACharacterBase* Character = nullptr) const;
	
	/** Retrieves the armament's current ability handles */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual const TArray<FGameplayAbilitySpecHandle>& GetAbilityHandles() const;
	
	/** Retrieves the armament's current passive handles */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual const TArray<FActiveGameplayEffectHandle>& GetPassiveHandles() const;
	
	/** Retrieves the armament's current state information handle */
	UFUNCTION(BlueprintCallable, Category = "Armaments|Utils") virtual const FActiveGameplayEffectHandle& GetStateInformationHandle() const;

	
};

