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
struct FGameplayEffectMapping;
enum class EEquipSlot : uint8;
enum class EComboType : uint8;
enum class ECharacterToMontageMapping : uint8;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament")
	F_ArmamentInformation ArmamentInformation;
	
	/** The current equip slot of the armament */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Armament") EEquipSlot EquipSlot;

	/** The armament's ability handles */
	UPROPERTY(BlueprintReadWrite) TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	
	/** The armament's passive handles */
	UPROPERTY(BlueprintReadWrite) TArray<FActiveGameplayEffectHandle> PassiveHandles;
	
	/** The armament's state information handle */
	UPROPERTY(BlueprintReadWrite) FActiveGameplayEffectHandle StateInformationHandle;

	
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

	/** The montages for the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") TMap<EComboType, UAnimMontage*> ArmamentMontages;


public:
	/** Updates the armament's montages provided a character to montage reference */
	UFUNCTION(BlueprintCallable, Category = "Armament|Montages") virtual bool UpdateArmamentMontages(const ECharacterToMontageMapping MontageMapping);
	
	
//-------------------------------------------------------------------------------------//
// Armament equipping and unequipping												   //
//-------------------------------------------------------------------------------------//
public:
	/** Sheathes the weapon for the character */ 
	UFUNCTION(BlueprintCallable, Category = "Armament|Equipping and Unequipping") virtual bool SheatheArmament();

	/** Draw the character's sword */
	UFUNCTION(BlueprintCallable, Category = "Armament|Equipping and Unequipping") virtual bool UnsheatheArmament();


protected:
	/** Attach the armament to the character using socket locations. This works for both the character's hand, sheathe, or any other custom location */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual bool AttachArmamentToSocket(FName Socket);
	
	
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
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<F_ArmamentAbilityInformation>& GetAbilities() const;
	
	/** Retrieves the armament's passives */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<FGameplayEffectMapping>& GetPassives() const;
	
	/** Retrieves the armament's equipped state information */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual FGameplayEffectMapping GetStateInformation() const;
	
	/** Retrieves the armament's base attack information */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TMap<FGameplayAttribute, float>& GetBaseDamageStats() const;
	
	/** Retrieves the armament's skeletal mesh */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") USkeletalMeshComponent* GetArmamentMesh() const;
	
	/** Retrieves the armament's equip slot */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") EEquipSlot GetEquipSlot() const;
	
	/** Returns the armament's overlap components for their armament. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual TArray<UPrimitiveComponent*> GetArmamentHitboxes() const;
	
	
	/** Get the armament attack information. This includes information on the armament and attack, and it's combo information. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual void SetArmamentInformation(const F_ArmamentInformation& Information);
	
	/** Set the slot this armament was equipped in. */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual void SetArmamentEquipSlot(EEquipSlot Slot);

	/** Retrieves the combat component from the character */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual UCombatComponent* GetCombatComponent(ACharacterBase* Character = nullptr) const;
	
	/** Retrieves the armament's current ability handles */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<FGameplayAbilitySpecHandle>& GetAbilityHandles() const;
	
	/** Retrieves the armament's current passive handles */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const TArray<FActiveGameplayEffectHandle>& GetPassiveHandles() const;
	
	/** Retrieves the armament's current state information handle */
	UFUNCTION(BlueprintCallable, Category = "Armament|Utils") virtual const FActiveGameplayEffectHandle& GetStateInformationHandle() const;

	
};

