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
	/**** Armament information ****/
	/** The attack pattern that's specific to this ability (Primary attack, Strong attack, etc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") EInputAbilities AttackPattern;

	/** The armament that's used during this attack */
	UPROPERTY(Transient, BlueprintReadWrite) AArmament* Armament;

	/** The current armament stance. If we're dual wielding combat calculations are retrieved from both armaments */ // TODO Fix this
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) EArmamentStance CurrentStance;

	/** The equip slot of the armament */
	UPROPERTY(Transient, BlueprintReadWrite) EEquipSlot EquipSlot;

	
	/**** Combat information ****/
	/** Whether this combat ability is based on a combo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bComboAbility = true;
	
	/** The combo attacks for this attack pattern */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") F_ComboAttacks ComboAttacks;
	
	/** The attack information of the current swing */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") F_ComboAttack CurrentAttack;

	/** The attack information (damages and any other attribute modifications) for the armament, */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") TMap<FGameplayAttribute, float> AttackInfo;

	/** The default montage section used when a combat montage is activated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") FName DefaultMontageSection;
	
	/** The montage section of the current combo */
	UPROPERTY(Transient, BlueprintReadWrite) FName MontageStartSection;

	
	/**** Current attack information ****/
	/** The number of attacks for a specific attack pattern (Primary attack, Strong attack, etc.) This is fixed to the specific montage */
	UPROPERTY(Transient, BlueprintReadWrite) int32 ComboCount = 0;

	/** The current combo attack index */
	UPROPERTY(Transient, BlueprintReadWrite) int32 ComboIndex = 0; // This value needs to be sent to the client
	
	/** Whether this is the final attack of the attack pattern */
	UPROPERTY(Transient, BlueprintReadWrite) bool bIsFinalComboAttack = false;


	/**** ****/
	/** Whether to use the blueprint's defined combat information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") bool bUseBPCombatInformation;

	/** The combat calculations for a specific weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") TMap<FGameplayAttribute, float> BP_DamageStats;

	/** The attack information of the current swing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") F_ComboAttack BP_AttackInformation;

	
public:
	UCombatAbility();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Adds a gameplay effect for stamina cost based on the player's attack information from the current attack. If you want to add a custom stamina amount, that's alright, this is just a function to help with adding logic */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat")
	virtual void AddStaminaCostEffect(float Stamina = 0.0);

	/* Epic's comment: Projects should initiate passives or do other "BeginPlay" type of logic here. */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:
	/** Retrieves the armament's combat information for this ability. Only updates if the player switches weapons or updates his stance */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual bool SetComboAndArmamentInformation();
	
	/** Retrieves all the necessary information for an attack. Call this during ActivateAbility to retrieve the attack information for the current attack */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void InitCombatInformation();

	/** Calculates the current attack information */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void SetComboAttack();

	/** Retrieves the attack montage from the armament based on different conditions */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void SetAttackMontage(AArmament* Weapon);
	
	/** Calculates the montage section for the current combo attack */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void SetMontageStartSection(bool ChargeAttack = false);

	/** Increments the combo index based on the combo attacks */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void SetComboIndex();
	
	/** Retrieves the attributes (mainly attack damages) and calculates the damage using multiple different calculations for attribute / weapon scaling. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void CalculateAttributeModifications();


protected:
	/** Handles melee attack target data and creating an exec calc effect context to pass to damage calculations */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void HandleMeleeAttack(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc);

	/** During the first frame of the character's attack, check if they've already attacked any characters and attacks if so */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void CheckAndAttackIfAlreadyOverlappingAnything(AArmament* OverlappedArmament, TArray<AActor*>& AlreadyHitActors);

	
//--------------------------------------------------------------------------------------//
// Utility																				//
//--------------------------------------------------------------------------------------//
protected:
	/** Checks if it's the final attack of this combo, and returns true if that's the case */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual bool DetermineIfItsTheFinalAttack(FName MontageSection, int32 Combos) const;
	
	/** Retrieves the number of combo attacks or montage sections. This is a safety function */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual int32 GetNumComboCount() const;

	/** Retrieves the number of montage sections for the current montage */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual int32 GetNumMontageSections() const;

	/** Returns true if the current ability is a right hand ability */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual bool IsRightHandAbility() const;
	
	/** Returns true if the current ability is a right hand ability. Use this for non instanced scenarios */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual bool IsRightHandAbilityInput(const EInputAbilities AbilityInput) const;

	/** Returns whether the weapon for this ability is equipped. This is safe to check before we've retrieved the weapon, should be used during CanActivateAbility() */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual bool IsWeaponEquipped(EInputAbilities AbilityInput, UCombatComponent* CombatComponent) const;

	/** Sets the armament that's used during the ability, and the equip slot for reference to know when it's unequipped */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual void SetArmament(AArmament* NewArmament);
	
	/** Handles removing the reference to the player's armament on unequip */
	UFUNCTION() virtual void OnUnequipArmament(FName ArmamentName, FGuid Id, EEquipSlot Slot);

	/** Logic when the player unequipped their weapon used for combat calculations */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability|Combat", DisplayName = "Unequipped Armament")
	void BP_OnUnequipArmament(FName ArmamentName, FGuid Id, EEquipSlot Slot);
	
	/** Retrieves the combat component from the character */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat") virtual UCombatComponent* GetCombatComponent() const;


	
};
