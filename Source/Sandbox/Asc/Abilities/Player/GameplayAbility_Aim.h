// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Asc/Abilities/Movement/MovementAbility.h"
#include "GameplayAbility_Aim.generated.h"

/**
 * Aiming logic to handle camera zooming for the player. Subclass this to handle different behaviors when the player has weapons equipped
 */
UCLASS()
class SANDBOX_API UGameplayAbility_Aim : public UMovementAbility
{
	GENERATED_BODY()

protected:
	/** The camera zoom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin = 0.1, UIMax = 10)) float CameraZoom;

	/** The camera zoom interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin = 0.0, UIMax = 100)) float CameraFovInterpSpeed;

	
public:
	UGameplayAbility_Aim();

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Input released function that's called if the player invokes the ability's input bind */
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
};
