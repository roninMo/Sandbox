// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Sandbox/Data/Enums/InverseKinematicsState.h"
#include "Sandbox/Data/Enums/MovementTypes.h"
#include "AnimInstanceBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimationBlueprint, Log, NoLogging);

#define EMD_Forward EMovementDirection::MD_Forward
#define EMD_Backward EMovementDirection::MD_Backward
#define EMD_Left EMovementDirection::MD_Left
#define EMD_Right EMovementDirection::MD_Right
#define EMD_ForwardLeft EMovementDirection::MD_ForwardLeft
#define EMD_ForwardRight EMovementDirection::MD_ForwardRight
#define EMD_BackwardLeft EMovementDirection::MD_BackwardLeft
#define EMD_BackwardRight EMovementDirection::MD_BackwardRight
#define EMD_Neutral EMovementDirection::MD_Neutral


class UAbilitySystemComponent;
class ACharacterBase;
class UAdvancedMovementComponent;
class UCurveVector;


/**
 * 
 */
UCLASS()
class SANDBOX_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	// virtual void InitializeAbilitySystem(UAbilitySystemComponent* ASC);
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	UAnimInstanceBase(const FObjectInitializer& ObjectInitializer);
	

//----------------------------------------------------------------------------------------------------------------------------------//
// Movement																													//
//----------------------------------------------------------------------------------------------------------------------------------//
protected:
	/**** Input, Acceleration, and velocity ****/
	/** The player's input values */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector2D Input;
	
	/** The player's acceleration */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Acceleration;
	
	/** The player's normalized acceleration */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Acceleration_N;
	
	/** The player's velocity */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Velocity;
	
	/** The player's normalized velocity */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Velocity_N;
	
	/** The player's speed */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Speed;
	
	/** The player's normalized speed */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Speed_N;
	
	/** The player's movement direction */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") EMovementDirection MovementDirection;
	
	/** Whether the player is moving forward */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Forward;
	
	/** Whether the player is moving backward */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Backward;
	
	/** Whether the player is moving left */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Left;
	
	/** Whether the player is moving right */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Right;

	/** The speed based on the player's movement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector DirectionalVelocity;
	
	/** The normalized speed based on the player's movement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector RelativeVelocity;

	/** The walk/run blend space values */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector2D WalkRunValues;
	
	/** The crouch blend space values */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector2D CrouchWalkValues;
	
	
	/**** Rotations and Aim Values ****/
	/** The player's world space rotation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator Rotation;

	/** The player's up/down rotation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Pitch;

	/** The player's left/right rotation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Yaw;

	/** The rotation relative to the character's current rotation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator RelativeRotation;

	/** The character controller's current aim rotation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator SmoothedAimRotation;

	/** The character controller's current aim rotation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator AimRotation;

	/** The relative rotation interp of the player's movement. */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator MovementAimRotation;

	/** The radius of the aim rotation. This is for AO when the player is turning his head */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Aim") FVector2D AimRotationAngles;

	/** The interp speed of the aim rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Aim") float AimRotationInterpSpeed;
	

	/**** Lean calculations ****/
	/** The lean calculation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanCalculation;
	
	/** The lean amount */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanAmount;
	
	/** The lean interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lean") float LeanInterpSpeed;
	
	/** The wall run lean amount */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D WallRunLeanAmount;
	
	/** The wall run lean interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lean") float WallRunLeanInterpSpeed;

	
	/**** Character Movement State values ****/
	/** The current movement mode */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") TEnumAsByte<EMovementMode> MovementMode;
	
	/** The custom movement mode */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") uint8 CustomMovementMode;
	
	/** If the player is accelerating */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bIsAccelerating;
	
	/** If the player is moving */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bIsMoving;
	
	/** If the player is sprinting */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bSprinting;
	
	/** If the player is crouching */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bCrouching;
	
	/** If the player is walking */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bWalking;
	
	/** If the player's camera style */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") FName CameraStyle;


	/** Animation specific values */
	/** The start time of the animation if the player lands on their right foot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float LandingFootStartTime = 0.0f;
	
	/** The start time of the animation if the player lands on their left foot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float AlternateFootStartTime = 0.45f;
	

//----------------------------------------------------------------------------------------------------------------------------------//
// Curve Values	(setting the curve values to 0 prevents (Montages/Overlays/Ik) from influencing animations							//
//----------------------------------------------------------------------------------------------------------------------------------//
protected:
	/**** Montage Overrides ****/
	/** Used to determine whether montages should influence the head during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Head;

	/** Used to determine whether montages should influence the pelvis during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Pelvis;

	/** Used to determine whether montages should influence the spine during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Spine;

	/** Used to determine whether montages should influence the legs during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Legs;

	/** Used to determine whether montages should influence the left arm during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Arm_L;

	/** Used to determine whether montages should influence the right arm during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Arm_R;

	/** Used to determine whether montages should influence the left hand during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Hand_L;

	/** Used to determine whether montages should influence the right hand during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Montage Overrides") float Montage_Hand_R;

	
	/**** Overlay Overrides ****/
	/** Used to determine whether overlays should influence the head during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Overlay Overrides") float Layering_Head;

	/** Used to determine whether overlays should influence the pelvis during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Overlay Overrides") float Layering_Pelvis;

	/** Used to determine whether overlays should influence the spine during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Overlay Overrides") float Layering_Spine;

	/** Used to determine whether overlays should influence the legs during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Overlay Overrides") float Layering_Legs;

	/** Used to determine whether overlays should influence the left arm during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Overlay Overrides") float Layering_Arm_L;

	/** Used to determine whether overlays should influence the right arm during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Overlay Overrides") float Layering_Arm_R;

	
	/**** Inverse Kinematics ****/
	/** Used to determine whether ik should influence the head during animations. Aim offsets already handle this by default */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Head;
	
	/** Used to determine whether ik should influence the pelvis during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Pelvis;

	/** Used to determine whether ik should influence the spine during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Spine;

	/** Used to determine whether ik should influence the left foot's placement when the player is walking on uneven surfaces */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Feet;
	
	/** Arm ik. Used for determining when inverse kinematics should influence the arm's movements */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Arm_L;
	
	/** Arm ik. Used for determining when inverse kinematics should influence the arm's movements */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Arm_R;

	/** Ik specifically for hands. Used for adjusting the hands during specific logic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Hand_L;
	
	/** Ik specifically for hands. Used for adjusting the hands during specific logic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|IK Overrides") float IK_Hand_R;

	
	/**** Primary values ****/
	/** Value to store whether the left foot should be stuck to the ground to create a realistic movement feel. This way the player isn't just sliding everywhere based on other player's interpretations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Movement", meta=(DisplayPriority=0)) float Foot_Lock_L;

	/** Value to store whether the right foot should be stuck to the ground to create a realistic movement feel. This way the player isn't just sliding everywhere based on other player's interpretations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Movement", meta=(DisplayPriority=0)) float Foot_Lock_R;

	/** Value to store the rotation during turn in place animations. This isn't networking safe, and probably shouldn't be used */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Movement", meta=(DisplayPriority=0)) float Turn_RotationAmount; // -1, 1 = Left, Right
	
	/** Whether to prevent sprinting during certain animations. This isn't networking safe, and probably shouldn't be used */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Movement", meta=(DisplayPriority=0)) float Mask_Sprint; // 1 = Prevent sprint
	
	/** Whether to prevent leaning during certain animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Lean") float Mask_Lean; // 1 = Prevent lean

	
	/**** AO Overrides ****/
	/** Used to determine whether aim offsets should influence the head during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|AO Overrides") float AO_Head;
	
	/** Used to determine whether aim offsets should influence the pelvis during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|AO Overrides") float AO_Pelvis;
	
	/** Used to determine whether aim offsets should influence the spine during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|AO Overrides") float AO_Spine;
	
	/** Used to determine whether aim offsets should influence the legs during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|AO Overrides") float AO_Legs;
	
	/** Used to determine whether aim offsets should influence the left arm during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|AO Overrides") float AO_Arm_L;
	
	/** Used to determine whether aim offsets should influence the right arm during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|AO Overrides") float AO_Arm_R;

	
//------------------------------------------------------------------------------//
// Captured Movement															//
//------------------------------------------------------------------------------//
	/** The current max walk speed, stored here for blend space references */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxWalkSpeed;
	/** The current max crouch speed, stored here for blend space references */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxCrouchSpeed;
	/** The current max sprint speed, stored here for blend space references */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxRunSpeed;

	
//----------------------------------------------------------------------------------------------------------------------------------//
// Inverse Kinematics																												//
//----------------------------------------------------------------------------------------------------------------------------------//
protected:
	/**** Spine rotations ****/
	/**
	  * The individual values are for the main UE skeleton. These are all just adjusted using the turn in place rotation.
	  */
	/** Normalized spine rotation alternative if you don't want to use a depreciating version (PelvisRotaiton,Spine01-04) */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator NormalizedSpineRotation;

	/** The current rotation of the pelvis bone */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator PelvisRotation;

	/** The current rotation of the spine_01 bone */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine01Rotation;

	/** The current rotation of the spine_02 bone */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine02Rotation;

	/** The current rotation of the spine_03 bone */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine03Rotation;

	/** The current rotation of the spine_04 bone */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine04Rotation;

	
	/**** Pelvis ****/ // @note the pelvis is used in both spine and feet inverse kinematics!
	/** The current offset of the pelvis */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis") FVector PelvisOffset;
	
	/** The target offset of the pelvis */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis") FVector PelvisTarget;


	
//----------------------------------------------------------//
// Feet														//
//----------------------------------------------------------//
	/**** Foot Placement ****/
	/** The trace distance below the foot to trace for placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_TraceDistanceBelowFoot;

	/** The trace distance above the foot to trace for placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_TraceDistanceAboveFoot;

	/** The interp speed for foot placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_FootPlacementInterpSpeed;
	
	/** The interp speed for foot placement when the foot is out of place (in the ground on inclines) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_FootInterpSpeedTransition;

	/** When we should adjust the height to account for the angle of the foot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float FootOffsetRollAdjustHeight;

	/** The range (degrees) of the player's foot angle that we're adjusting the height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") FVector2D UpperFootOffsetInRange;
	
	/** The height adjustments based on the angle of the foot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") FVector2D UpperFootOffsetOutRange;

	/** The version of foot calculations we're using for foot placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") EFootInverseKinematics FootInverseKinematics;

	
	/**** Foot Locking ****/
	/** The interp speed for foot locking. This scales with the player's movement speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_FootLockInterpSpeed;

	/** Curve values transition because of animation blending, so during a blend space sometimes the values are offset when the foot is already in place. This helps with activating kinematics early to account for this */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet", meta=(UIMin = "0", UIMax = "0.5")) float FootLockActivationOffset;

	/** Curve values transition because of animation blending, so during a blend space sometimes the values are offset when the foot is already in place. This helps with activating kinematics early to account for this */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") bool AddRotationOffset;

	/** The left foot lock state */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") EInverseKinematicsState LeftFootLockState;
	
	/** The right foot lock state */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") EInverseKinematicsState RightFootLockState;
	
	
	/**** Left foot ****/
	/** The name of the left foot bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FName IKLeftFootBoneName;

	/** The bone rotation offset for translating ik_foot_bone rotations to the actual foot rotations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator IKLeftFootRotationOffset;
	
	/** Target locations */
	/** The foot lock target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLockTarget_L;
	
	/** The foot placement target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootPlacementTarget_L;
	
	/** The target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLocationTarget_L;
	
	/** Current locations */
	/** The foot lock offset location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLockLocation_L;
	
	/** The foot placement offset location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootPlacementLocation_L;
	
	/** The current location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLocation_L;
	
	/** Target rotations */
	/** The foot placement target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootPlacementRotationTarget_L;
	
	/** The foot lock target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootLockRotationTarget_L;
	
	/** The target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootRotationTarget_L;
	
	/** Current rotations */
	/** The foot lock rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootLockRotation_L;
	
	/** The foot placement rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootPlacementRotation_L;
	
	/** The current rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootRotation_L;


	
	/**** Right foot ****/
	/** The name of the right foot bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FName IKRightFootBoneName;
	
	/** The bone rotation offset for translating ik_foot_bone rotations to the actual foot rotations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator IKRightFootRotationOffset;
	
	/** Target locations */
	/** The foot lock target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLockTarget_R;
	
	/** The foot placement target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootPlacementTarget_R;
	
	/** The target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLocationTarget_R;
	
	/** Current locations */
	/** The foot lock offset location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLockLocation_R;
	
	/** The foot placement offset location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootPlacementLocation_R;
	
	/** The current location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLocation_R;
	
	/** Target rotations */
	/** The foot placement target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootPlacementRotationTarget_R;
	
	/** The foot lock target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootLockRotationTarget_R;
	
	/** The target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootRotationTarget_R;
	
	/** Current rotations */
	/** The foot lock rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootLockRotation_R;
	
	/** The foot placement rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootPlacementRotation_R;
	
	/** The current rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootRotation_R;

	
	

	

//----------------------------------------------------------//
// Arms														//
//----------------------------------------------------------//
	/** The alpha of the inverse kinematics for arms */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float ArmsInverseKinematicsAlpha;

	/** The version of arm calculations we're using for arm placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") EArmsInverseKinematics ArmsInverseKinematics;

	
	/**** Wall Running ****/
	/** The interp speed of inverse kinematics during wall running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunArmsInterpSpeed;
	
	/** The interp speed while transitioning out of inverse kinematics during wall running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunArmsInterpSpeedTransition;

	/** How quickly we transition out of inverse kinematics during wall running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunAlphaInterpSpeed;

	/** The trace distance from the center of the character towards the wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunTraceDistance;

	/** The length of the arm. Used for placing the arm during wall run inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunArmLength;
	
	/** The height offset of the arm during wall run inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunArmHeightOffset;
	
	/** The spacing offset for hand placement on the wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunHandSpacing;
	
	/** The left hand rotation for proper placement on the wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") FRotator WallRunLeftHandRotation;
	
	/** The right hand rotation for proper placement on the wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") FRotator WallRunRightHandRotation;
	
	/** The width offset of the arm during wall run inverse kinematics. This is only used if something happened and it isn't using the wall location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunArmWidthOffset;

	/** The initial location of the hand during wall running. We transition back to this to prevent weird rotations with differing animations at the end of the wall run  */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") FVector ResetLocationOffset;

	/** The initial rotation of the hand during wall running. We transition back to this to prevent weird rotations with differing animations at the end of the wall run  */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") FRotator ResetRotationOffset;

	/** Whether the inverse kinematics is for arms is wall running */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") bool bWallRunInverseKinematics;

	/** Whether the current wall run inverse kinematics is for the left or right hand */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") bool bRightHandWallRun;

	/** The current state of inverse kinematics for wall running */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") EInverseKinematicsState IK_WallRunState;
	
	
	/**** Pressed towards or alongside walls ****/
	/** The interp speed of inverse kinematics for arms */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float ArmsInterpSpeed;
	
	/** Whether the inverse kinematics is for arms is wall placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") bool bWallPlacementInverseKinematics;

	/** The current state of inverse kinematics for wall running */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") EInverseKinematicsState IK_WallPlacementState;
	
	
	/**** Left arm ****/
	/** The name of the left hand bone used for creating inverse kinematic effectors. The majority of solving algorithms we're using are forwards and backwards or full body */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FName LeftArmBoneName;

	/** The target location for the left arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmLocationTarget_L;

	/** The current location for the left arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmLocationOffset_L;

	/** The target rotation for the left arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmRotationTarget_L;

	/** The target location for the left arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmRotationOffset_L;

	
	/**** Right arm ****/
	/** The name of the right hand bone used for creating inverse kinematic effectors. The majority of solving algorithms we're using are forwards and backwards or full body */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FName RightArmBoneName;

	/** The target location for the right arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmLocationTarget_R;

	/** The current location for the right arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmLocationOffset_R;

	/** The target rotation for the right arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmRotationTarget_R;

	/** The current rotation for the right arm. This can either be in world space or component space */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmRotationOffset_R;
	
	
	/**** Other References ****/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Utility") TObjectPtr<UAdvancedMovementComponent> MovementComponent;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Utility") TObjectPtr<ACharacterBase> Character;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Utility") bool bLocallyControlled;

	
	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed
	// UPROPERTY(EditDefaultsOnly, Category = "GameplayTags") FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
	

//------------------------------------------------------------------------------//
// Other																		//
//------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Placement") bool bDebugIKFootPlacement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Placement") float IKFootPlacementTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Locking") bool bDebugIKFootLocking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Locking") float IKFootLockingTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Arms") bool bDebugArmTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Arms") float ArmTraceDuration = 0.1;
	
	
//------------------------------------------------------------------------------//
// Movement Calculations														//
//------------------------------------------------------------------------------//
protected:
	/** Calculates teh character's base movement values */
	virtual void GetCharacterMovementValues(float DeltaTime);
	
	/** Calculates the yaw, lean, and aim rotations for the character */
	virtual void CalculateYawAndLean(float DeltaTime);

	/** Calculates the movement direction for the character */
	virtual void CalculateMovementDirection();

	/** Finds the curve values and stores the information for bp reference */
	virtual void UpdateCurveValues();
	
	
//------------------------------------------------------------------------------//
// Inverse Kinematics															//
//------------------------------------------------------------------------------//
	/** Calculates the inverse kinematics for feet placement */
	virtual void CalculateFeetIK(float DeltaTime);

	/** Handles the inverse kinematic location and rotation offsets for foot placement */
	virtual void FootPlacementInverseKinematics(float DeltaTime, FName IKFootBone, FVector& CurrentOffset, FVector& TargetOffset, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset);

	/** Adjusts the pelvis offset for feet ik */
	virtual void PelvisOffsetForFootPlacement(float DeltaTime, FVector& LeftFootLocation, FVector& RightFootLocation, FVector& PelvisTargetLocation, FVector& PelvisOffsetLocation);
	
	/** Handles the inverse kinematic location and rotation offsets for foot locking */
	virtual void FeetLockingInverseKinematics(float DeltaTime);

	/** Calculates the target offset and interps the foot lock positions for a foot */
	virtual void FootLockInverseKinematics(float DeltaTime, FName IKFootBone, FVector& CurrentLocation, FVector& TargetLocation, FRotator& CurrentRotation, FRotator& TargetRotation, float& CurrentAlpha);
	
	/** Resets the feet and pelvis offsets for feet ik */
	virtual void ResetIKFeetAndPelvisOffsets(float DeltaTime);

	/** Whether we should calculate inverse kinematics for feet */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation|Inverse Kinematics") bool CanCalculateFeetIK();
	virtual bool CanCalculateFeetIK_Implementation();
	
	
	/** Determines which variation of inverse kinematics to use and then handles calculations */
	virtual void CalculateArmsIK(float DeltaTime);
	
	/** Calculates the arm locations during wall placement */
	// virtual void CalculateWallPlacementArmsIK(float DeltaTime);

	/** Handles the inverse kinematic location and rotation offsets for wall placement */
	virtual void WallPlacementArmInverseKinematics(float DeltaTime);

	/** Calculates the arm locations during wall running */
	virtual void CalculateWallRunArmsIK(float DeltaTime);

	/** Handles the inverse kinematic location and rotation offsets for wall running */
	virtual void WallRunArmInverseKinematics(float DeltaTime, FName IKHandBone, FVector& CurrentOffset, FVector& TargetOffset, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset);

	/** Resets the feet and pelvis offsets for feet ik @remarks Don't use during world transitions if you're setting the transforms of the actual bones */
	virtual void ResetArmIKOffsets(float DeltaTime, FVector& CurrentOffset, FVector& TargetOffset, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset, FVector Location = FVector::ZeroVector);


	

	/** If the movement conditions are valid for calculating foot placement inverse kinematics. This doesn't influence blending, and is used to activate/deactivate the state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation|Inverse Kinematics") bool ShouldCalculateFootPlacementIK() const;
	virtual bool ShouldCalculateFootPlacementIK_Implementation() const;
	
	/** If the movement conditions are valid for calculating foot locking inverse kinematics. This doesn't influence blending, and is used to activate/deactivate the state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation|Inverse Kinematics") bool ShouldCalculateFootLockIK() const;
	virtual bool ShouldCalculateFootLockIK_Implementation() const;
	
	
	
//------------------------------------------------------------------------------//
// Utility																		//
//------------------------------------------------------------------------------//
public:
	UFUNCTION(BlueprintCallable) virtual bool GetCharacterInformation();
	UFUNCTION(BlueprintCallable) virtual bool GetMovementComponent();
	UFUNCTION(BlueprintCallable) virtual bool GetCharacter();

	UFUNCTION(BlueprintCallable) virtual bool IsPlayingInEditor(UObject* WorldContextObject) const;
	UFUNCTION(BlueprintCallable) virtual bool IsPlayingInGame(UObject* WorldContextObject) const;

	
};
