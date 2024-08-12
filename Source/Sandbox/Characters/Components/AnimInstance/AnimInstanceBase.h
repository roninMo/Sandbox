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
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector2D Input;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Acceleration;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Acceleration_N;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Velocity;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector Velocity_N;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Speed;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Speed_N;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") EMovementDirection MovementDirection;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Forward;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Backward;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Left;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Right;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector DirectionalVelocity;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector RelativeVelocity;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector2D WalkRunValues;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FVector2D CrouchWalkValues;
	
	
	/**** Rotations and Aim Values ****/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator PreviousRotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator Rotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Pitch;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float Yaw;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator RelativeRotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator SmoothedAimRotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator AimRotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") FRotator MovementAimRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Aim") FVector2D AimRotationAngles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Aim") float AimRotationInterpSpeed;
	

	/**** Lean calculations ****/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanCalculation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lean") float LeanInterpSpeed;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D WallRunLeanAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lean") float WallRunLeanInterpSpeed;

	
	/**** Character Movement State values ****/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") TEnumAsByte<EMovementMode> MovementMode;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") uint8 CustomMovementMode;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bIsAccelerating;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bIsMoving;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bSprinting;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bCrouching;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bWalking;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Head;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Pelvis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Spine;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Legs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Arm_L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Arm_R;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Hand_L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Montage_Hand_R;

	
	/**** Overlay Overrides ****/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Layering_Head;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Layering_Pelvis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Layering_Spine;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Layering_Legs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Layering_Arm_L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Layering_Arm_R;

	
	/**** Inverse Kinematics ****/
	/** Used to determine whether ik should influence the head during animations. Aim offsets already handle this by default */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Head;
	
	/** Used to determine whether ik should influence the pelvis during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Pelvis;

	/** Used to determine whether ik should influence the spine during animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Spine;

	/** Used to determine whether ik should influence the left foot's placement when the player is walking on uneven surfaces */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Feet;
	
	/** Arm ik. Used for determining when inverse kinematics should influence the arm's movements */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Arm_L;
	
	/** Arm ik. Used for determining when inverse kinematics should influence the arm's movements */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Arm_R;

	/** Ik specifically for hands. Used for adjusting the hands during specific logic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Hand_L;
	
	/** Ik specifically for hands. Used for adjusting the hands during specific logic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float IK_Hand_R;

	
	/**** Primary values ****/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Feet_Plant; // -1 = Left foot, 1 = Right foot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Turn_RotationAmount; // -1, 1 = Left, Right
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Mask_Sprint; // 1 = Prevent sprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float Mask_Lean; // 1 = Prevent lean

	
	/**** AO Overrides ****/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float AO_Head;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float AO_Pelvis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float AO_Spine;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float AO_Legs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float AO_Arm_L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves") float AO_Arm_R;

	
//------------------------------------------------------------------------------//
// Captured Movement															//
//------------------------------------------------------------------------------//
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxWalkSpeed;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxCrouchSpeed;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxRunSpeed;

	
//----------------------------------------------------------------------------------------------------------------------------------//
// Inverse Kinematics																												//
//----------------------------------------------------------------------------------------------------------------------------------//
protected:
	/**** Spine rotations ****/
	/*** The individual values are for the main UE skeleton. These are all just adjusted using the turn in place rotation. **/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator NormalizedSpineRotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator PelvisRotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine01Rotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine02Rotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine03Rotation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Spine") FRotator Spine04Rotation;

	
	/**** Pelvis ****/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis") float PelvisAlpha;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis") FVector PelvisOffset;
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

	/** Whether the inverse kinematics is for foot placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") bool bFootPlacementInverseKinematics;

	
	/**** Foot Locking ****/
	/** The interp speed for foot locking. This scales with the player's movement speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_FootLockInterpSpeed;

	/** Whether the inverse kinematics is for foot locking */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") bool bFootLockingInverseKinematics;
	
	/** Used to capture the foot location for feet locking */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float PrevFeetPlantValue;
	
	
	/**** Left foot ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FName IKLeftFootBoneName;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FVector FootLocationTarget_L;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FVector FootLocationOffset_L;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FRotator FootRotationTarget_L;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FRotator FootRotationOffset_L;
	
	/**** Right foot ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FName IKRightFootBoneName;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FVector FootLocationTarget_R;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FVector FootLocationOffset_R;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FRotator FootRotationTarget_R;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FRotator FootRotationOffset_R;


//----------------------------------------------------------//
// Arms														//
//----------------------------------------------------------//
	/**** Wall Running ****/
	/** The interp speed of inverse kinematics during wall running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunArmsInterpSpeed;
	
	/** The interp speed while transitioning out of inverse kinematics during wall running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Run") float WallRunArmsInterpSpeedTransition;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FName LeftArmBoneName;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FVector ArmLocationTarget_L;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FVector ArmLocationOffset_L;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FRotator ArmRotationTarget_L;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FRotator ArmRotationOffset_L;

	
	/**** Right arm ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Left Foot") FName RightArmBoneName;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FVector ArmLocationTarget_R;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FVector ArmLocationOffset_R;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FRotator ArmRotationTarget_R;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Right Foot") FRotator ArmRotationOffset_R;
	
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") bool bDebugIKFootPlacement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") float IKFootPlacementTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") bool bDebugIKFootLocking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") float IKFootLockingTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") bool bDebugArmTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") float ArmTraceDuration = 0.1;
	
	
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
	virtual void PelvisOffsetForFootPlacement(float DeltaTime);
	
	/** Handles the inverse kinematic location and rotation offsets for foot locking */
	virtual void FeetLockingInverseKinematics(float DeltaTime);

	/** Calculates the target offset and interps the foot lock positions for a foot */
	virtual void FootLockInverseKinematics(float DeltaTime, FName IKFootBone, FVector& CurrentOffset, FVector& TargetLocation, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset);
	
	/** Resets the feet and pelvis offsets for feet ik */
	virtual void ResetIKFeetAndPelvisOffsets(float DeltaTime);

	/** Whether we should calculate inverse kinematics for feet */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation|Inverse Kinematics") bool ShouldCalculateFeetIK();
	virtual bool ShouldCalculateFeetIK_Implementation();
	
	
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
	
	/** Whether we should calculate inverse kinematics for the left arm */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation|Inverse Kinematics") bool ShouldCalculateLeftArmIK();
	virtual bool ShouldCalculateLeftArmIK_Implementation();

	/** Whether we should calculate inverse kinematics for the right arm */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation|Inverse Kinematics") bool ShouldCalculateRightArmIK();
	virtual bool ShouldCalculateRightArmIK_Implementation();
	
	
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
