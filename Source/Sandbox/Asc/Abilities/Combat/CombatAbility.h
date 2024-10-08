// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/CharacterGameplayAbility.h"
#include "Sandbox/Data/Structs/CombatInformation.h"
#include "CombatAbility.generated.h"

class AArmament;
class UCombatComponent;
enum class EArmamentStance : uint8;
enum class EEquipSlot : uint8;


/**
 * Utilities and logic for combat and combo attacks. Subclass this for abilities specific to combat
 */
UCLASS()
class SANDBOX_API UCombatAbility : public UCharacterGameplayAbility
{
	GENERATED_BODY()

protected:
	/** Armament information */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") EInputAbilities AttackPattern;
	UPROPERTY(Transient, BlueprintReadWrite) AArmament* Armament;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) EArmamentStance CurrentStance;
	UPROPERTY(Transient, BlueprintReadWrite) EEquipSlot EquipSlot;
	
	/** Combat information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") F_ComboAttacks ComboAttacks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") F_ComboAttack CurrentAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") TMap<FGameplayAttribute, float> AttackInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") FName DefaultMontageSection;
	UPROPERTY(Transient, BlueprintReadWrite) FName MontageStartSection;
	UPROPERTY(Transient, BlueprintReadWrite) bool bComboAbility = true;
	UPROPERTY(Transient, BlueprintReadWrite) bool bUseRightHandArmament = true;

	/** Event received tags for this ability's tasks to listen for multiple gameplay delegates */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") FGameplayTagContainer EventReceivedTags;
	
	/** Current attack information */
	UPROPERTY(Transient, BlueprintReadWrite) TArray<AActor*> AlreadyHitActors;
	UPROPERTY(Transient, BlueprintReadWrite) int32 ComboCount = 0;
	UPROPERTY(Transient, BlueprintReadWrite) int32 ComboIndex = 0; // This value needs to be sent to the client
	UPROPERTY(Transient, BlueprintReadWrite) bool bCancelledToEquipArmament = false;
	UPROPERTY(Transient, BlueprintReadWrite) bool bIsFinalComboAttack = false;
	

public:
	UCombatAbility();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/* Epic's comment: Projects should initiate passives or do other "BeginPlay" type of logic here. */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:
	/** Retrieves the armament's combat information for this ability. Only updates if the player switches weapons or updates his stance */
	UFUNCTION(BlueprintCallable) virtual bool SetComboAndArmamentInformation();
	
	/** Retrieves all the necessary information for an attack. Call this during ActivateAbility to retrieve the attack information for the current attack */
	UFUNCTION(BlueprintCallable) virtual void InitCombatInformation();

	/** Calculates the current attack information */
	UFUNCTION(BlueprintCallable) virtual void SetComboAttack();

	/** Retrieves the attack montage from the armament based on different conditions */
	UFUNCTION(BlueprintCallable) virtual void SetAttackMontage(AArmament* Weapon);
	
	/** Calculates the montage section for the current combo attack */
	UFUNCTION(BlueprintCallable) virtual void SetMontageStartSection(bool ChargeAttack = false);

	/** Increments the combo index based on the combo attacks */
	UFUNCTION(BlueprintCallable) virtual void SetComboIndex();
	
	/** Retrieves the attributes (mainly attack damages) and calculates the damage using multiple different calculations for attribute / weapon scaling. */
	UFUNCTION(BlueprintCallable) virtual void CalculateAttributeModifications();


protected:
	/** Handles melee attack target data and creating an exec calc effect context to pass to damage calculations */
	UFUNCTION(BlueprintCallable) virtual void HandleMeleeAttack(const FGameplayAbilityTargetDataHandle& TargetData, UAbilitySystem* TargetAsc);

	/** During the first frame of the character's attack, check if they've already attacked any characters and attacks if so */
	UFUNCTION(BlueprintCallable) virtual void CheckAndAttackIfAlreadyOverlappingAnything(TArray<AActor*>& Actors);

	
//--------------------------------------------------------------------------------------//
// Utility																				//
//--------------------------------------------------------------------------------------//
protected:
	/** Checks if it's the final attack of this combo, and returns true if that's the case */
	UFUNCTION(BlueprintCallable) virtual bool DetermineIfItsTheFinalAttack(FName MontageSection, int32 Combos) const;
	
	/** Retrieves the number of combo attacks or montage sections. This is a safety function */
	UFUNCTION(BlueprintCallable) virtual int32 GetNumComboCount() const;

	/** Retrieves the number of montage sections for the current montage */
	UFUNCTION(BlueprintCallable) virtual int32 GetNumMontageSections() const;

	/** Retrieves the actors we've already attacked on this swing */
	UFUNCTION(BlueprintCallable) virtual TArray<AActor*>& GetAlreadyHitActors();

	/** Returns true if the current ability is a right hand ability */
	UFUNCTION(BlueprintCallable) virtual bool IsRightHandAbility() const;
	
	/** Returns true if the current ability is a right hand ability. Use this for non instanced scenarios */
	UFUNCTION(BlueprintCallable) virtual bool IsRightHandAbilityInput(const EInputAbilities AbilityInput) const;

	/** Sets the armament that's used during the ability, and the equip slot for reference to know when it's unequipped */
	UFUNCTION(BlueprintCallable) virtual void SetArmament(AArmament* NewArmament);
	
	/** Handles removing the reference to the player's armament on unequip */
	UFUNCTION() virtual void OnUnequipArmament(FName ArmamentName, FGuid Id, EEquipSlot Slot);
	
	/** Retrieves the combat component from the character */
	UFUNCTION(BlueprintCallable) virtual UCombatComponent* GetCombatComponent() const;


	
};
