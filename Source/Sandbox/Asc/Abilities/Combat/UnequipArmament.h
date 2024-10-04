// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/CharacterGameplayAbility.h"
#include "UnequipArmament.generated.h"

class UCombatComponent;
class AArmament;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;


/**
 * 
 */
UCLASS()
class SANDBOX_API UUnequipArmament : public UCharacterGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag UnequipEventTag;
	UPROPERTY(BlueprintReadWrite) bool bUnequippedArmaments;

	UPROPERTY(BlueprintReadWrite) UAbilityTask_PlayMontageAndWait* MontageTaskHandle;
	UPROPERTY(BlueprintReadWrite) UAbilityTask_WaitGameplayEvent* EquipGameplayTaskHandle;

	
public:
	UUnequipArmament();
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
protected:
	UFUNCTION(BlueprintCallable) virtual void OnEndMontage();
	UFUNCTION(BlueprintCallable) virtual void OnUnequipEventReceived(FGameplayEventData EventData);
	UFUNCTION(BlueprintCallable) virtual bool GetUnequipMontage();
	UFUNCTION(BlueprintCallable) virtual void HandleUnequipArmamentLogic();
	UFUNCTION(BlueprintCallable) virtual bool ApplyUnequippedStateInformation(AArmament* EquippedArmament);
	
	/** Retrieves the combat component from the character */
	UFUNCTION(BlueprintCallable) virtual UCombatComponent* GetCombatComponent() const;
	
};
