// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"
#include "MeleeCombatAbility.generated.h"

class AArmament;
class UAbilitySystem;
class UCombatComponent;
enum class EArmamentStance : uint8;
enum class EAttackFramesState : uint8;
enum class EEquipSlot : uint8;


/**
 * 
 */
UCLASS()
class SANDBOX_API UMeleeCombatAbility : public UCombatAbility
{
	GENERATED_BODY()
	
protected:
	/**** Armament information ****/
	/** The combo attacks for this attack pattern */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") F_ComboAttacks ComboAttacks;
	
	/** The attack information of the current swing */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") F_ComboAttack CurrentAttack;

	/** The attack information of the attack pattern's crouching attack */
	UPROPERTY(Transient, BlueprintReadWrite) F_ComboAttacks CrouchingAttackInformation;
	
	/** The attack information of the attack pattern's running attack */
	UPROPERTY(Transient, BlueprintReadWrite) F_ComboAttacks RunningAttackInformation;
	
	/** Whether this attack is a crouching attack */
	UPROPERTY(Transient, BlueprintReadWrite) bool bCrouchingAttack;
	
	/** Whether this attack is a running attack */
	UPROPERTY(Transient, BlueprintReadWrite) bool bRunningAttack;

	
	/**** Current attack information ****/
	/** The number of attacks for a specific attack pattern (Primary attack, Strong attack, etc.) This is fixed to the specific montage */
	UPROPERTY(Transient, BlueprintReadWrite) int32 ComboCount = 0;

	/** The current combo attack index */
	UPROPERTY(Transient, BlueprintReadWrite) int32 ComboIndex = 0; // This value needs to be sent to the client
	
	/** Whether this is the final attack of the attack pattern */
	UPROPERTY(Transient, BlueprintReadWrite) bool bIsFinalComboAttack = false;

	
	/**** Dual wielding information ****/
	/** If the player is wielding two armaments, this is a stored reference of the offhand armament */
	UPROPERTY(Transient, BlueprintReadWrite) AArmament* OffhandArmament;

	/** The equip slot of the offhand armament */
	UPROPERTY(Transient, BlueprintReadWrite) EEquipSlot OffhandEquipSlot;

	/** State information for the attack frames of the primary weapon */
	UPROPERTY(Transient, BlueprintReadWrite) EAttackFramesState PrimaryAttackState;
	
	/** State information for the attack frames of the secondary weapon */
	UPROPERTY(Transient, BlueprintReadWrite) EAttackFramesState SecondaryAttackState;
	
	/** The primary weapon's already hit actors */
	UPROPERTY(Transient, BlueprintReadWrite) TArray<AActor*> PrimaryHitActors;
	
	/** The secondary weapon's already hit actors */
	UPROPERTY(Transient, BlueprintReadWrite) TArray<AActor*> SecondaryHitActors;

	
	/**** Other Information ****/
	/** Whether this combat ability is based on a combo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") bool bComboAbility = true;
	
	/** Whether this attack pattern allows crouching attacks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") bool bUseCrouchingAttacks = true;
	
	/** Whether this attack pattern allows running attacks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") bool bUseRunningAttacks = true;

	
	/**** Combat Test Information ****/
	/** The attack information of the current swing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Test Information") F_ComboAttack TestAttackInformation;

	
	/**** Cached tags ****/
	/** The stamina cost's gameplay effect tag */
	UPROPERTY(Transient, BlueprintReadWrite) FGameplayTag StaminaCostEffectTag;
	
	/** When we should allow rotation movement during the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag HitStunTag;
	
	/** When we should allow rotation movement during the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AllowMovementTag;
	
	/** When we should actually trace for enemies during an attack animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AttackFramesTag;

	/** The tag to notify the end of attack frames for a specific animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AttackFramesEndTag;
	
	/** The tag to notify the end of attack frames for a specific left hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag LeftHandAttackFramesEndTag;

	/** The tag to notify the end of attack frames for a specific right hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag RightHandAttackFramesEndTag;

	/** The tag to notify the begin of attack frames for a specific animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag AttackFramesBeginTag;

	/** The tag to notify the begin of attack frames for a specific left hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag LeftHandAttackFramesBeginTag;

	/** The tag to notify the begin of attack frames for a specific right hand animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag RightHandAttackFramesBeginTag;

	
public:
	UMeleeCombatAbility();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/* Epic's comment: Projects should initiate passives or do other "BeginPlay" type of logic here. */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


//--------------------------------------------------------------------------------------//
// Initialization																		//
//--------------------------------------------------------------------------------------//
protected:
	/**
	 * Retrieves the armament's information for this attack pattern. Subclass this for armament information specific to melee/ranged \n\n
	 * 
	 * If the stance is updated or the player equips another armament, it clear's the old information. Use OnArmamentUnequipped to handle custom logic
	 *
	 * @returns									True if the armament and it's information was successfully retrieved
	 */
	virtual bool SetArmamentInformation_Implementation() override;

	
	/**
	 * Retrieves all the necessary information for an attack. The combat information, montage logic, damage calculations, etc.
	 * @remarks Just call this during ActivateAbility to retrieve the attack information for the current attack. Calculations for different abilities
	 */
	virtual void InitCombatInformation_Implementation() override;


protected:
	/** Increments the combo index based on the combo attacks, and safely handles transitioning between attack patterns */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Init") void SetComboIndex();
	virtual void SetComboIndex_Implementation();

	/** Calculates the current attack information using the combo index and armament information  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Init") void SetComboAttack();
	virtual void SetComboAttack_Implementation();

	/**
	 * Retrieves the attack montage from the armament based on the attack pattern. Subclass this for different conditions.
	 *
	 * @param Weapon							The weapon we're retrieving the montages from
	 */
	virtual void SetAttackMontage_Implementation(AArmament* Weapon) override;
	
	/**
	 * Calculates the montage section for the current attack. Subclass this to adjust the montage section
	 *
	 * @param ChargeAttack						Whether the current attack is a charged attack
	 */
	virtual void SetMontageStartSection_Implementation(bool ChargeAttack = false) override;

	/**
	 *  Retrieves the combat calculations from the armament, player stats, equipment, and calculates the attack information for the current attack.
	 *  @remarks The default just retrieves the armament's base damage information, or the custom information on the ability for testing
	 */
	virtual void CalculateAttributeModifications_Implementation() override;


//--------------------------------------------------------------------------------------//
// Attack Frames Logic																	//
//--------------------------------------------------------------------------------------//
protected:
	/**
	 * Listens for left and right hand begin/end attack frames gameplay events, and calls OnBeginAttackFrames() and OnEndAttackFrames() to handle melee trace logic for combat. \n\n
	 *
	 * It uses the primary and secondary attack frame state values for handling whether the attack frames are disabled/enabled/finished \n\n
	 * 
	 * By default it only searches for a single frame, subclass the function for animations with multiple frames you'll have to adjust this logic, specifically for enemies
	 * 
	 * @param EventData							The event data that's sent from the montage. We're specifically searching for LeftHandAttackFrames and RightHandAttackFrames tags
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Attack Frames") void OnAttackFrameEvent(FGameplayEventData EventData);
	virtual void OnAttackFrameEvent_Implementation(FGameplayEventData EventData);

	/**
	 * Handles begin tracing logic for targets for a specific weapon, and checks if we're already overlapping with any targets
	 *
	 * @remarks Blueprint functions need to add the melee overlap task right now, or subclass this function to activate the overlap logic (if it hasn't already been activated)
	 *
	 * @param bRightHand						Whether we should be tracing for the left or right hand armament
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Attack Frames") void OnBeginAttackFrames(bool bRightHand);
	virtual void OnBeginAttackFrames_Implementation(bool bRightHand);
	
	/**
	 * Handles end tracing for targets for a specific weapon
	 *
	 * @remarks Blueprint functions need to remove the melee overlap task right now, or subclass this function to end the task
	 * 
	 * @param bRightHand						Whether we should be tracing for the left or right hand armament
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Attack Frames") void OnEndAttackFrames(bool bRightHand);
	virtual void OnEndAttackFrames_Implementation(bool bRightHand);

	/**
	 * Checks if the overlap occurred on valid attack frames, and calls HandleMeleeAttack() if that's the case. It also handles and checks if the player has already landed an attack on the target
	 *
	 * 
	 * @remarks By default this handles both weapons, so you shouldn't need to subclass this function
	 * @param TargetData						The weapon overlap target data
	 * @param OverlappedArmament				The armament that the player attacked with
	 * @param TargetAsc							The target's ability system component
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Attack Frames") void OnOverlappedTarget(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc);
	virtual void OnOverlappedTarget_Implementation(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc);
	

//--------------------------------------------------------------------------------------//
// Utility																				//
//--------------------------------------------------------------------------------------//
protected:
	/**
	 * Handles conditions on whether this counts for a valid crouch attack
	 *
	 * @returns									If the player should be performing a crouch attack
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool IsValidForCrouchAttack() const;
	virtual bool IsValidForCrouchAttack_Implementation() const;
	
	/**
	 * Handles conditions on whether this counts for a valid running attack
	 *
	 * @returns									If the player should be performing a running attack
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool IsValidForRunningAttack() const;
	virtual bool IsValidForRunningAttack_Implementation() const;

	/** Retrieves the armaments that are currently used for overlap tracing based on the player's current stance */
	UFUNCTION(BlueprintCallable, Category = "Ability|Combat|Utils") TArray<AArmament*> GetOverlapArmaments() const;

	/** Checks if it's the final attack of this combo, and returns true if that's the case */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") bool DetermineIfItsTheFinalAttack() const;
	virtual bool DetermineIfItsTheFinalAttack_Implementation() const;
	
	/** Retrieves the number of combo attacks or montage sections. This is a safety function */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") int32 GetNumComboCount() const;
	virtual int32 GetNumComboCount_Implementation() const;

	/** Retrieves the number of montage sections for the current montage */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|Combat|Utils") int32 GetNumMontageSections() const;
	virtual int32 GetNumMontageSections_Implementation() const;
	
		
};
