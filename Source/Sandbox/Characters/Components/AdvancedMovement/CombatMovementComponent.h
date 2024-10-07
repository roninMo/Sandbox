// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "CombatMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UCombatMovementComponent : public UAdvancedMovementComponent
{
	GENERATED_BODY()

protected:

	UCombatMovementComponent();


//------------------------------------------------------------------------------//
// State																		//
//------------------------------------------------------------------------------//
protected:
	/**
	 * Updates the character state in PerformMovement right before doing the actual position change
	 * This handles updating the movement mode updates from player inputs
	 */
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	
	
//------------------------------------------------------------------------------//
// Root Motion																	//
//------------------------------------------------------------------------------//
protected:
	/** Applies root motion from root motion sources to velocity (override and additive) */
	virtual void ApplyRootMotionToVelocity(float deltaTime) override;

	/**
	 * Calculate velocity from anim root motion.
	 * @param RootMotionDeltaMove	Change in location from root motion.
	 * @param DeltaSeconds			Elapsed time
	 * @param CurrentVelocity		Non-root motion velocity at current time, used for components of result that may ignore root motion.
	 * @see ConstrainAnimRootMotionVelocity
	 */
	virtual FVector CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaSeconds, const FVector& CurrentVelocity) const override;

	/**
	 * Removes the forwards acceleration during root motion animations
	 * This allows the movement component to still calculate velocity without exponential increases in the player's movement. For air attacks we don't want the player to stop moving during root motion animations
	 */
	virtual void AdjustAccelerationDuringRootMotion();

	
	
//------------------------------------------------------------------------------//
// Movement Component Functions													//
//------------------------------------------------------------------------------//
	// Movement functions broken out based on owner's network Role.
	// TickComponent calls the correct version based on the Role.
	// These may be called during move playback and correction during network updates.
	//

	/** Perform movement on an autonomous client @note I really don't want to adjust this function, it's very problematic and it causes problems with everything it's linked to */
	virtual void PerformMovement(float DeltaSeconds) override;
	


	
//------------------------------------------------------------------------------//
// Adjusted functions to add movement state tags								//
//------------------------------------------------------------------------------//
protected:
	// TODO: Non instanced abilities don't handle state tags, and for movement, abilities shouldn't handle tagging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag AttackingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag CrouchingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag SlidingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag SprintingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag FallingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag AimingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag JumpingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag WallRunningTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag WallClimbingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag WallMantlingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement|State Tags") FGameplayTag WallLedgeClimbingTag;

	
public:
	/**
	 * Checks if new capsule size fits (no encroachment), and call CharacterOwner->OnStartCrouch() if successful.
	 * In general you should set bWantsToCrouch instead to have the crouch persist during movement, or just use the crouch functions on the owning Character.
	 * @param	bClientSimulation	true when called when bIsCrouched is replicated to non owned clients, to update collision cylinder and offset.
	 */
	virtual void Crouch(bool bClientSimulation) override;
	
	/**
	 * Checks if default capsule size fits (no encroachment), and trigger OnEndCrouch() on the owner if successful.
	 * @param	bClientSimulation	true when called when bIsCrouched is replicated to non owned clients, to update collision cylinder and offset.
	 */
	virtual void UnCrouch(bool bClientSimulation) override;
	
	/** Add State tags for whether the character is crouching */
	virtual void HandleCrouchLogic();

		
};
