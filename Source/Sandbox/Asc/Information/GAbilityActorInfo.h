// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GAbilityActorInfo.generated.h"


/**
 *	FGameplayAbilityActorInfo
 *
 *	Cached data associated with an Actor using an Ability.
 *		-Initialized from an AActor* in InitFromActor
 *		-Abilities use this to know what to actor upon. E.g., instead of being coupled to a specific actor class.
 *		-These are generally passed around as pointers to support polymorphism.
 *		-Projects can override UAbilitySystemGlobals::AllocAbilityActorInfo to override the default struct type that is created.
 *
 */
USTRUCT(BlueprintType)
struct FGAbilityActorInfo : public FGameplayAbilityActorInfo
{
	GENERATED_USTRUCT_BODY()

	virtual ~FGAbilityActorInfo() override {}

	// access to the attribute set ?

	// also the player state ?

	
	/** Gameplay ability actor info base values
	 * TWeakObjectPtr<AActor>	OwnerActor;
	 * TWeakObjectPtr<AActor>	AvatarActor;
	 * TWeakObjectPtr<APlayerController>	PlayerController;
	 * TWeakObjectPtr<UAbilitySystemComponent>	AbilitySystemComponent;
	 * TWeakObjectPtr<USkeletalMeshComponent>	SkeletalMeshComponent;
	 * TWeakObjectPtr<UAnimInstance>	AnimInstance;
	 * TWeakObjectPtr<UMovementComponent>	MovementComponent;
	 * FName AffectedAnimInstanceTag; 
	 */

	/** Initializes the info from an owning actor. Will set both owner and avatar */
	virtual void InitFromActor(AActor *Owner, AActor *Avatar, UAbilitySystemComponent* InAbilitySystemComponent) override;

	/** Sets a new avatar actor, keeps same owner and ability system component */
	virtual void SetAvatarActor(AActor *Avatar) override;

	/** Clears out any actor info, both owner and avatar */
	virtual void ClearActorInfo() override;
};

