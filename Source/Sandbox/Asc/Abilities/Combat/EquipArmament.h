// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/CharacterGameplayAbility.h"
#include "EquipArmament.generated.h"


class AArmament;
class UCombatComponent;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;


/**
 * This equips the armament to the character, and should give state tags to the character to notify that the character has equipped the ability
 * To make this easier on the system, I'm not binding this to an input and calling this from the attack abilities of an armament
 */
UCLASS()
class SANDBOX_API UEquipArmament : public UCharacterGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag EquipEventTag;
	UPROPERTY(BlueprintReadWrite) bool bEquippedArmaments;

	UPROPERTY(BlueprintReadWrite) UAbilityTask_PlayMontageAndWait* MontageTaskHandle;
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayEvent* EquipGameplayTaskHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag EquipArmamentTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag EquipSecondaryArmamentTag;

	
public:
	UEquipArmament();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
protected:
	UFUNCTION(BlueprintCallable) virtual void OnEndMontage();
	UFUNCTION(BlueprintCallable) virtual void OnEquipEventReceived(FGameplayEventData EventData);
	UFUNCTION(BlueprintCallable) virtual bool GetEquipMontage();
	UFUNCTION(BlueprintCallable) virtual void HandleEquipArmamentLogic();
	UFUNCTION(BlueprintCallable) virtual bool ApplyEquippedStateInformation(AArmament* EquippedArmament);
	
	/** Retrieves the combat component from the character */
	UFUNCTION(BlueprintCallable) virtual UCombatComponent* GetCombatComponent() const;
	
};
