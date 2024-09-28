// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Structs/CombatInformation.h"
#include "Sandbox/World/Props/Items/ItemBase.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Armament.generated.h"


class UGameplayEffect;
struct F_ArmamentAbilityInformation;
struct FGameplayEffectMapping;
enum class EEquipSlot : uint8;
enum class EArmamentClassification : uint8;
enum class EDamageInformationSource : uint8;
enum class EEquipRestrictions : uint8;


UCLASS(BlueprintType, Category = "Combat")
class SANDBOX_API AArmament : public AItemBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr <USkeletalMeshComponent> ArmamentMesh;

	/** The information for this armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament") F_ArmamentInformation ArmamentInformation;

	/** The current equip slot of the armament */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Armament") EEquipSlot EquipSlot;

	/** The armament's ability handles */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	
	/** The armament's passive handles */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FActiveGameplayEffectHandle> PassiveHandles;
	
	/** The armament's state information handle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FActiveGameplayEffectHandle StateInformationHandle;

	
//-------------------------------------------------------------------------------------//
// Armament Construction															   //
//-------------------------------------------------------------------------------------//
protected:
	AArmament(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
	
public:
	// /** Function for equipping this armament. Add setup and teardown logic specific to this armament's configuration. */
	// UFUNCTION(BlueprintCallable) virtual bool ConstructArmament();
	//
	// /** Function for unequipping this armament. Add setup and teardown logic specific to this armament's configuration. */
	// UFUNCTION(BlueprintCallable) virtual bool DeconstructArmament();
	//
	// /** Function that checks whether the armament is valid to equip */
	// UFUNCTION(BlueprintCallable) virtual bool IsValidArmanent();


//-------------------------------------------------------------------------------------//
// Montages																			   //
//-------------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UDataTable> MontageInformationTable;
	// Montage db
	// GetMontage(EComboType ComboType, ECharacterToMontageMapping Mapping);

	// Montage map
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EComboType, UAnimMontage*> ArmamentMontages;

	// On equip -> Update montage mappings


public:

	
//-------------------------------------------------------------------------------------//
// Armament equipping and unequipping												   //
//-------------------------------------------------------------------------------------//
public:
// 	/** Sheathes the weapon for the character */ 
// 	virtual bool SheatheArmament();
//
// 	/** Draw the character's sword */
// 	virtual bool UnsheatheArmament();
//
//
// protected:
// 	/** Attach the armament to the character's hand based on the equip slot */
// 	virtual bool AttachArmamentToEquipSlot();
//
// 	/** Attach the armament to the character's holster */
// 	virtual bool AttachArmamentToHolster(FName Socket);
	
	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	// /** Retrieves the armament attack information. This includes information on the armament and attack, and it's combo information. */
	// virtual const F_ArmamentInformation& GetArmamentInformation() const;
	//
	// /** Retrieves the database id of the armament */
	// virtual const FName GetArmamentId() const;
	//
	// /** Retrieves the armament classification */
	// virtual const EArmamentClassification GetClassification() const;
	//
	// /** Retrieves how we're handling damage calculations */
	// virtual const EDamageInformationSource GetDamageCalculation() const;
	//
	// /** Retrieves the armament's equip restrictions */
	// virtual const EEquipRestrictions GetEquipRestrictions() const;
	//
	// /** Retrieves the armament's abilities */
	// virtual const TArray<F_ArmamentAbilityInformation>& GetAbilities() const;
	//
	// /** Retrieves the armament's passives */
	// virtual const TArray<FGameplayEffectMapping>& GetPassives() const;
	//
	// /** Retrieves the armament's equipped state information */
	// virtual const TSubclassOf<UGameplayEffect> StateInformation() const;
	//
	// /** Retrieves the armament's base attack information */
	// virtual const TMap<FGameplayAttribute, float>& GetBaseDamageStats() const;
	//
	// /** Retrieves the armament's skeletal mesh */
	// USkeletalMeshComponent* GetArmamentMesh() const;
	//
	// /** Retrieves the armament's equip slot */
	// const EEquipSlot GetEquipSlot() const;
	//
	// /** Returns the armament's overlap components for their armament. */
	// virtual TArray<UPrimitiveComponent*> GetArmamentHitboxes() const;
	//
	//
	//
	// /** Get the armament attack information. This includes information on the armament and attack, and it's combo information. */
	// virtual void SetArmamentInformation(const F_ArmamentInformation& Information);
	//
	// /** Set the slot this armament was equipped in. */
	// virtual void SetArmamentEquipSlot(EEquipSlot Slot);
	//
	//
	// /** Retrieves the armament's current ability handles */
	// virtual const TArray<FGameplayAbilitySpecHandle>& GetAbilityHandles() const;
	//
	// /** Retrieves the armament's current passive handles */
	// virtual const TArray<FActiveGameplayEffectHandle>& GetPassiveHandles() const;
	//
	// /** Retrieves the armament's current state information handle */
	// virtual const FActiveGameplayEffectHandle& GetStateInformationHandle() const;

	
};



