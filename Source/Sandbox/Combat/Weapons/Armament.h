// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/World/Props/Items/ItemBase.h"
#include "Armament.generated.h"

UCLASS(BlueprintType, Category = "Combat")
class SANDBOX_API AArmament : public AItemBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr <USkeletalMeshComponent> ArmamentMesh;

	/** The information for this armament */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armament Information") F_ArmamentInfoSource ArmamentInfoSource;
// 	UPROPERTY(BlueprintReadWrite, Category = "Armament Information") F_InventoryItem ItemInformation;
// 	UPROPERTY(Replicated, BlueprintReadWrite) EEquipSlot EquipSlot;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName IKHandSocketName;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName LeftHandIKHandSocketName;
// 	
//
// public:
// 	/** The ability handles for this armament */
// 	UPROPERTY(BlueprintReadWrite) TArray<FGameplayAbilitySpecHandle> ActiveArmamentAbilities;
// 	UPROPERTY(BlueprintReadWrite) TArray<FActiveGameplayEffectHandle> ActiveArmamentPassives;
//
// 	
// protected:
// 	/** Impact Settings */
// 	UPROPERTY(BlueprintReadWrite, Category = "Particles|Particle Impact") class UNiagaraComponent* Impact;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Impact") 
// 	TObjectPtr<UNiagaraSystem> ImpactClass;
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Impact", meta = (EditCondition = "ImpactClass != nullptr", EditConditionHides)) 
// 	FName ImpactAttachPointName = FName("None");
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Impact", meta = (EditCondition = "ImpactClass != nullptr", EditConditionHides)) 
// 	FVector ImpactScale = FVector(1.0f, 1.0f, 1.0f);
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Impact", meta = (EditCondition = "ImpactClass != nullptr", EditConditionHides)) 
// 	bool bUseProjectileRotation = true;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Impact", meta = (EditCondition = "ImpactClass != nullptr", EditConditionHides))
// 	FRotator ImpactRotationOffset = FRotator(0.0f, 0.0f, 0.0f);
// 	
// 	/** Sound Settings */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound") TObjectPtr<USoundBase> LoopingSound;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound") TObjectPtr<USoundBase> ImpactSound;
// 	UPROPERTY() TObjectPtr<UAudioComponent> LoopingSoundComponent;
// 	
// 	/** Replication values for handling specific situations */
// 	bool bAlreadyHit = false;


public:
// 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	AArmament(const FObjectInitializer& ObjectInitializer);
// 	
// 	
// //-------------------------------------------------------------------------------------//
// // Armament Construction															   //
// //-------------------------------------------------------------------------------------//
// 	/** Get the row name of this armament. This is to get the information for this armament during creation. */
// 	virtual const FName GetRowName() const;
//
// 	/** Function for equipping this armament. Add setup and teardown logic specific to this armament's configuration. */
// 	virtual bool ConstructArmament();
// 	
// 	/** Function for unequipping this armament. Add setup and teardown logic specific to this armament's configuration. */
// 	virtual bool DeconstructArmament();
//
// 	/** Function that checks whether the armament is valid to equip */
// 	virtual bool IsValidArmanent();
//
// 	
// //-------------------------------------------------------------------------------------//
// // Get armament information															   //
// //-------------------------------------------------------------------------------------//
// 	/** Get the armament attack information. This includes information on the armament and attack, and it's combo information. */
// 	virtual AArmament* GetArmament();
// 	
// 	/** Get the armament attack information. This includes information on the armament and attack, and it's combo information. */
// 	virtual F_ArmamentInfoSource GetArmamentInformation() const;
// 	
// 	/** Get the ability for this armament. */
// 	virtual TArray<TSubclassOf<UBaseGameplayAbility>> GetArmamentAbilities(bool bRightHand) const;
//
// 	/** Get the passives for this armament. */
// 	virtual TArray<TSubclassOf<UGameplayEffect>> GetArmamentPassives() const;
// 	
// 	/** Anim tag specific to each armament -> This is to configure all the animations for every armament. */
// 	virtual TSubclassOf<UGameplayEffect> GetEquippedStateInformation() const;
//
// 	/** Gets the armament type of this armament. This can be used to get information like the montage the player should use for this attack. */
// 	virtual EArmamentType GetArmamentType();
// 	
// 	/** Gets the equip type of this armament. */
// 	virtual EArmamentEquipType GetEquipType();
// 	
// 	/** Get the item information for this armament. */
// 	virtual F_InventoryItem GetItemInformation();
// 	
// 	/** Get the armament attack information. This includes information on the armament and attack, and it's combo information. */
// 	virtual void SetArmamentInformation(const F_ArmamentInfoSource& ArmamentInformation);
//
// 	/** Set the item information for this armament. This should be done once the armament is created for inventory management */
// 	virtual void SetItemInformation(const F_InventoryItem& ItemInformation);
// 	
// 	/** Set the slot this armament was equipped in. */
// 	virtual void SetArmamentEquipSlot(EEquipSlot EquippedSlot);
// 	
//
// //-------------------------------------------------------------------------------------//
// // Get combo information															   //
// //-------------------------------------------------------------------------------------//
// 	/** Gets the combo attacks for this armament. */
// 	virtual const TArray<F_ComboAttack> GetCombos(EComboType ComboType);
//
// 	/** Damage execution calculation for this armament. */
// 	virtual TSubclassOf<UGameplayEffect> GetDamageEffectClass(EComboType ComboType) const;
//
// 	/** Gets the active attack of this armament's combo based on the ability system's state. This is captured through gameplay tags. */
// 	virtual const F_ComboAttack GetActiveComboAttack(EComboType ComboType);
// 	
// 	/** Get the base attack information for armament damage based scaling attacks */
// 	// TODO: For the sake of refactoring is there a better way to access referenced data in a map safely? Either a dummy value as a fallback if the map doesn't contain this value,
// 	// that way helper functions are allowed and it puts ease on other things. I really don't want to copy all this information everywhere, it's tiring
// 	virtual F_AttackInformation GetBaseAttackInformation(EComboType ComboType);
//
// 	/** The activation and early completion of ai combo attacks */
// 	virtual F_AIComboActivationChance GetAIComboActivationChances(EComboType ComboType);
// 	
// 	/** Get the unqiue id of the item stored (this is to handle having multiple versions of an item in the inventory (and this item should be created if it hasn't been already). */
// 	virtual const FGuid GetId() const;
//
// 	/** Finds the specific combo data for the armament and the player's current stance */
// 	virtual const TMap<EComboType, F_ComboAndDamageData>& GetComboAndDamageData(bool bRightHand, EArmamentStance ArmamentStance) const;
// 	UPROPERTY() TMap<EComboType, F_ComboAndDamageData> DummyDamageData; // Dummy information for handling function references
// 	
// 	
// //-------------------------------------------------------------------------------------//
// // Get montage information															   //
// //-------------------------------------------------------------------------------------//
// 	/** Gets the attack montage for this armament. */
// 	virtual UAnimMontage* GetAttackMontage(EComboType ComboType);
//
// 	/** Gets the root motion block montage for the character of this armament. */
// 	virtual UAnimMontage* GetBlockRM_Montage(bool bRightHand);
// 	
// 	/** Gets the reload animations for the character of this armament. */
// 	virtual UAnimMontage* GetReloadMontage();
//
// 	/** Gets the equip montage for the character of this armament. */
// 	virtual UAnimMontage* GetHolsterMontage(bool bRightHand);
// 	
// 	/** Gets the equip montage for the character of this armament. */
// 	virtual UAnimMontage* GetEquipMontage(bool bRightHand);
// 	
// 	/** Helper function to get the correct montage based on the combo type */
// 	virtual UAnimMontage* HandleAttackMontageMapping(FM_MeleeArmamentMontages& MontageMap, EComboType ComboType);
//
// 	/** Finds the montages specific to the armament and the player's current stance */
// 	virtual FM_MeleeArmamentMontages& GetMontageMappings(bool bRightHand, EArmamentStance ArmamentStance);
// 	UPROPERTY() FM_MeleeArmamentMontages DummyMontages; // Dummy information for handling function references
//
// 	
// //-------------------------------------------------------------------------------------//
// // Armament equipping and unequipping												   //
// //-------------------------------------------------------------------------------------//
// 	/** Sheathes the armament for the character. This is what the character should call */ 
// 	virtual bool SheatheArmament();
//
// 	/** Draw the character's sword. This is what the character should call */
// 	virtual bool UnsheatheArmament();
// 	
// 	/** Attach the armament to the character's hand based on the equip slot */
// 	virtual bool AttachArmamentToEquipSlot();
//
// 	/** Attach the armament to the character's holster */
// 	virtual bool AttachArmamentToHolster(FName Socket);
// 	
// 	/** Gets the socket name of the left hand if this is a two handed armament. */
// 	virtual FName GetIKHandSocketName();
// 	
// 	
// //-------------------------------------------------------------------------------------//
// // Combat Specific functions														   //
// //-------------------------------------------------------------------------------------//
// 	/** Returns the armament's overlap components for their armament. */
// 	virtual TArray<UPrimitiveComponent*> GetArmamentHitboxes() const;
//
// 	/** Function what should happen if something overlaps with the armament. */
// 	virtual void OnOverlapArmament(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
//
// 	
// //-------------------------------------------------------------------------------------//
// // Other functions																	   //
// //-------------------------------------------------------------------------------------//
// public:
// 	USkeletalMeshComponent* GetArmamentMesh() const { return ArmamentMesh; }
// 	FORCEINLINE EEquipSlot GetArmamentEquipSlot() const { return EquipSlot; }
	

};



