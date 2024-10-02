// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/CharacterGameplayAbility.h"
#include "Sandbox/Data/Structs/CombatInformation.h"
#include "CombatAbility.generated.h"

class UCombatComponent;
enum class EArmamentStance : uint8;
enum class EAttackPattern : uint8;


/**
 * Utilities and logic for combat and combo attacks. Subclass this for abilities specific to combat
 */
UCLASS()
class SANDBOX_API UCombatAbility : public UCharacterGameplayAbility
{
	GENERATED_BODY()

protected:
	/** Event received tags for this ability's tasks to listen for multiple gameplay delegates */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTagContainer EventReceivedTags;

	/** Armament information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EAttackPattern AttackPattern;
	UPROPERTY(BlueprintReadWrite) EArmamentStance ArmamentStance;
	UPROPERTY(BlueprintReadWrite) AArmament* Armament;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bComboAbility = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseRightHandArmament = true;
	

	/** Combat attack information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ArmamentInformation ArmamentInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<F_ComboAttack> ComboAttacks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ComboAttack ComboAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ComboCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ComboIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsFinalComboAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bCancelledToEquipArmament = false;
	

public:
	UCombatAbility();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:

	// TODO: Right now there's no configuration for attacking with multiple armaments or how to access what is the active equip slot. Fix this
	/** Get the combo attacks and armament information for the whole combo */
	UFUNCTION(BlueprintCallable) virtual bool GetComboAndArmamentInformation();
	
// 	/** Gets the attack and combo information for this swing. Use this for the start of a chain of abilities for a single swing of a character attacking Charge->Attack->etc */
// 	UFUNCTION(BlueprintCallable) virtual void InitCombatInformation();
// 	
//
//
// 	
//
// 	
// 	
// 	
// protected:
// 	/** Gets the attack information for the attack. Armaments only need to get this information once, and combo attacks should get this every frame. */
// 	UFUNCTION(BlueprintCallable) virtual F_AttackInformation GetAttackInfo();
// 	
// 	/** Checks if the armament is equipped and equips the armament if it isn't already equipped */
// 	// UFUNCTION(BlueprintCallable) virtual bool CheckIfShouldEquipArmament(); // Input abilities should handle this
//
// 	/** Get the current combo attack */
// 	UFUNCTION(BlueprintCallable) virtual F_ComboAttack GetComboAttack();
//
// 	/** Checks if the armament is equipped and equips the armament if it isn't already equipped */
// 	UFUNCTION(BlueprintCallable) virtual bool CheckIfShouldEquipArmament();
// 	
// 	/** Gets the attack and combo information for this swing. Use this for the start of a chain of abilities for a single swing of a character attacking Charge->Attack->etc */
// 	UFUNCTION(BlueprintCallable) virtual void InitCombatInformation();
// 	
// 	/** Handles melee attack target data and creating an exec calc effect context to pass to damage calculations */
// 	UFUNCTION(BlueprintCallable) virtual void HandleMeleeAttack(const FGameplayAbilityTargetDataHandle& TargetData, UBaseAbilitySystem* TargetAsc);
//
// 	/** During the first frame of the character's attack, check if they've already attacked any characters and attacks if so */
// 	UFUNCTION(BlueprintCallable) virtual void CheckAndAttackIfAlreadyOverlappingAnything(TArray<AActor*>& Actors);
// 	
// 	/** Gets the attack information for the attack. Armaments only need to get this information once, and combo attacks should get this every frame. */
// 	UFUNCTION(BlueprintCallable) virtual F_AttackInformation GetAttackInfo();
// 	
// 	/**
// 	 * Calculates the damage for this combo of the attack. This should be used with the task that's sending the execution calculations
// 	 * Subclass this for each attack to have different raw damage calculations. The majority of it should be done in the Exec Calc class
// 	 */
// 	UFUNCTION(BlueprintCallable) virtual float GetCalculatedDamage() const;
//
// 	/** Removes the combo duration tags through the combat component's effect handle */
// 	UFUNCTION(BlueprintCallable) virtual void RemoveComboTags();
//
// 	/** Add a combo duration tag of this attack */
// 	UFUNCTION(BlueprintCallable) virtual void AddComboDurationTag();
// 	
// 	/** Add gameplay tag state tags to the player */
// 	UFUNCTION(BlueprintCallable)
// 	virtual FActiveGameplayEffectHandle AddStateTags(const TSubclassOf<UGameplayEffect> StateInformation, float Level = 1);
//
// 	/** Function for handling tags on a character at the end of the attack */
// 	virtual void HandleTagsAtEndOfAttack() override;
// 	
// 	/**
// 	 * Add an infinite tag at the start of each attack to account for the delay in the client receiving the combo duration tag.
// 	 * This should be nulled out by the duration tag and is a safeguard for the character quickly chaining combos
// 	 * TODO: Find out why client side prediction isn't occurring here and implement that instead
// 	 */
// 	UFUNCTION(BlueprintCallable) virtual void AddComboInfiniteTag(FName MontageSection);
//
// 	/** Get the montage of this attack */
// 	UFUNCTION(BlueprintCallable) virtual FName DetermineMontageStartSection(bool ChargeAttack = false, EComboType ComboType = EComboType::CT_Primary) const;
// 	UFUNCTION(BlueprintCallable) virtual bool DetermineIfItsTheFinalAttack(FName MontageSection, int32 Combos) const;
// 	
// 	/** Get the combo count from the number of combo attacks or montage sections. This is a safety function */
// 	UFUNCTION(BlueprintCallable) virtual int32 GetNumComboCount() const;
// 	UFUNCTION(BlueprintCallable) virtual int32 GetNumMontageSections() const;
//
// 	
// public:
// 	UFUNCTION(BlueprintCallable) virtual TArray<AActor*>& GetAlreadyHitActors();
	
	UFUNCTION(BlueprintCallable) virtual UCombatComponent* GetCombatComponent() const;


	
};
