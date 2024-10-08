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
	/** The friction for air movement when using root motion animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling", meta=(ClampMin="0.0", UIMin = "0.0", UIMax = "10"))
	float AnimRootMotionVelocityAirFriction = 1;
	
	/** Max Strafing Acceleration (how fast you move / strafe) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling", meta=(ClampMin="0.0", UIMin = "0.0", UIMax = "10000"))
	float AnimRootMotionMaxAcceleration = 450;
	
	/** The rotation rate during root motion air movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling", meta=(ClampMin="0.0", UIMin = "0.0", UIMax = "10000"))
	FRotator RootMotionAirRotationRate = FRotator(0, 150, 0);
	
	/** The velocity captured for when the player begins root motion */
	UPROPERTY(Transient, BlueprintReadWrite) FVector EnterAnimRootMotionAirVelocity;
	
	/** The velocity during root motion */
	UPROPERTY(Transient, BlueprintReadWrite) FVector RootMotionAirVelocity;

	UCombatMovementComponent();

	
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
	
	/** Captures the velocity before the player began anim root motion. This is used for if the player moving in air to preserve their current velocity */
	virtual void AirVelocityDuringRootMotion(float DeltaSeconds);
	
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag AttackingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag CrouchingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag SlidingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag SprintingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag FallingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag AimingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag JumpingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag WallRunningTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag WallClimbingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag WallMantlingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)|State Tags") FGameplayTag WallLedgeClimbingTag;

	
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
