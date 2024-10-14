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
	/** The attack information (damages and any other attribute modifications) for the armament, */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") TMap<FGameplayAttribute, float> AttackInfo;

	/** The default montage section used when a combat montage is activated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") FName DefaultMontageSection;
	
	/** The montage section of the current combo */
	UPROPERTY(Transient, BlueprintReadWrite) FName MontageStartSection;


	/**** Combat Test Information ****/
	/** Whether to use the blueprint's defined combat information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Test Information") bool bUseTestCombatInformation;

	/** The combat calculations for a specific weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Test Information") TMap<FGameplayAttribute, float> TestDamageStats;

	
public:
	UCombatAbility();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Adds a gameplay effect for stamina cost to the player */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat|Init")
	virtual void AddStaminaCostEffect(float Stamina);

	/* Epic's comment: Projects should initiate passives or do other "BeginPlay" type of logic here. */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:
	/**
	 * Retrieves the armament's information for this attack pattern. Subclass this for armament information specific to melee/ranged \n\n
	 * 
	 * If the stance is updated or the player equips another armament, it clear's the old information. Use OnArmamentUnequipped to handle custom logic
	 *
	 * @returns									True if the armament and it's information was successfully retrieved
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Init") bool SetArmamentInformation();
	virtual bool SetArmamentInformation_Implementation();
	
	/**
	 * Retrieves all the necessary information for an attack. The combat information, montage logic, damage calculations, etc.
	 * @remarks Just call this during ActivateAbility to retrieve the attack information for the current attack. Calculations for different abilities
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Init") void InitCombatInformation();
	virtual void InitCombatInformation_Implementation();

	/**
	 * Retrieves the attack montage from the armament based on the attack pattern. Subclass this for different conditions.
	 *
	 * @param Weapon							The weapon we're retrieving the montages from
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Init") void SetAttackMontage(AArmament* Weapon);
	virtual void SetAttackMontage_Implementation(AArmament* Weapon);
	
	/** Calculates the montage section for the current attack. Subclass this to adjust the montage section */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Init") void SetMontageStartSection(bool ChargeAttack = false);
	virtual void SetMontageStartSection_Implementation(bool ChargeAttack = false);

	/**
	 *  Retrieves the combat calculations from the armament, player stats, equipment, and calculates the attack information for the current attack.
	 *  @remarks The default just retrieves the armament's base damage information, or the custom information on the ability for testing
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat") void CalculateAttributeModifications();
	virtual void CalculateAttributeModifications_Implementation();


protected:
	/**
	 * Handles melee attack target data and creating an exec calc effect context to pass to damage calculations
	 *
	 * @param TargetData						Target data from the overlap trace
	 * @param OverlappedArmament				The armament used for this melee attack
	 * @param TargetAsc							The ability system component of the target
	 * @param DamageCalculation					The Exec Calc that handles damage calculations
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat")
	virtual void HandleMeleeAttack(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc, TSubclassOf<UGameplayEffect> DamageCalculation);

	/**
	 * Used during the first frame of the character's attack. Checks if they've already attacked any characters and attacks if that's the case
	*
	 * @param OverlappedArmament				The armament used for this melee attack
	 * @param AlreadyHitActors					The armament's array of already hit actors
	 * @param DamageCalculation					The Exec Calc that handles damage calculations
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat")
	virtual void CheckAndAttackIfAlreadyOverlappingAnything(AArmament* OverlappedArmament, TArray<AActor*>& AlreadyHitActors, TSubclassOf<UGameplayEffect> DamageCalculation);

	
//--------------------------------------------------------------------------------------//
// Utility																				//
//--------------------------------------------------------------------------------------//
protected:
	/**
	 * Checks if this is an ability from the player's right hand weapon using the attack pattern
	 * 
	 * @returns true if the current ability is a right hand ability
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool IsRightHandAbility() const;
	virtual bool IsRightHandAbility_Implementation() const;
	
	/**
	 * Checks if this is an ability from the player's right hand weapon using the attack pattern
	 *
	 * @remarks Safe to call on CDO/Non instanced abilities
	 * @returns true if the current ability is a right hand ability
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool IsRightHandAbilityInput(const EInputAbilities AbilityInput) const;
	virtual bool IsRightHandAbilityInput_Implementation(const EInputAbilities AbilityInput) const;

	/**
	 * Checks if the player has any stamina, and returns true if he doesn't have any more.
	 *
	 * @param AbilitySystemComponent			The player's ability system component 
	 * @returns whether the player is out of stamina
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool IsOutOfStamina(UAbilitySystemComponent* AbilitySystemComponent) const;
	virtual bool IsOutOfStamina_Implementation(UAbilitySystemComponent* AbilitySystemComponent) const;

	/**
	 * Checks if this instance of the ability has any armament information, or if we should activate the ability to retrieve the information and handle the rest of the validation logic
	 *
	 * @returns whether there's a valid armament 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool ShouldActivateAbilityToRetrieveArmament() const;
	virtual bool ShouldActivateAbilityToRetrieveArmament_Implementation() const;

	/**
	 * Uses the attack pattern to find the armament on the combat component, and check's whether the weapon is equipped.
	 * 
	 * @returns									True if the weapon for this ability is equipped.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool IsWeaponEquipped(EInputAbilities AbilityInput, UCombatComponent* CombatComponent) const;
	virtual bool IsWeaponEquipped_Implementation(EInputAbilities AbilityInput, UCombatComponent* CombatComponent) const;

	
	/** Sets the armament that's used during the ability, and the equip slot for reference to know when it's unequipped */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability|Combat|Utils") void SetArmament(AArmament* NewArmament);
	virtual void SetArmament_Implementation(AArmament* NewArmament);
	
	/** Handles removing the reference to the player's armament on unequip */
	UFUNCTION() virtual void OnUnequipArmament(FName ArmamentName, FGuid Id, EEquipSlot Slot);

	/** Logic when the player unequipped their weapon used for combat calculations */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability|Combat|Utils", DisplayName = "Unequipped Armament")
	void BP_OnUnequipArmament(FName ArmamentName, FGuid Id, EEquipSlot Slot);

	/**
	 * Retrieves the combat component from the character using the current actor information
	 * 
	 * @returns									The player's combat component
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat|Utils")
	virtual UCombatComponent* GetCombatComponent() const;

	
};
