// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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
	
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float AimRotationAngle;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float AimRotationInterpSpeed;
	

	/**** Lean calculations ****/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanCalculation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanAmount;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") float LeanInterpSpeed;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D WallRunLeanAmount;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") float WallRunLeanInterpSpeed;

	
	/**** Character Movement State values ****/
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") EMovementDirection MovementDirection;
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


	/**** Feet ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_FootInterpSpeed_Slow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_FootInterpSpeed_Fast;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_TraceDistanceBelowFoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float IK_TraceDistanceAboveFoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") float FootOffsetRollAdjustHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") FVector2D UpperFootOffsetInRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet") FVector2D UpperFootOffsetOutRange;

	
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

	
	/**** Arms ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") FTransform LeftHandTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") FTransform RightHandTransform;
	
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") bool bDebugIKFootTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging") float IKFootTraceDuration = 0.1;
	
	
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
	/** Calculates the feet positions for IK control rigs */
	virtual void CalculateFootIK(float DeltaTime);

	/** Sets the offset location and rotation for ik feet placement */
	virtual void SetIKFootOffsets(float DeltaTime, FName IKFootBone, FVector& CurrentOffset, FVector& TargetOffset, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset);

	/** Sets the pelvis offset for feet ik */
	virtual void SetIKPelvisOffsetForFeet(float DeltaTime, FVector& CurrentOffset, FVector& TargetOffset);

	/** Resets the feet and pelvis offsets for feet ik */
	virtual void ResetIKFeetAndPelvisOffsets(float DeltaTime);

	/** Whether we should calculate inverse kinematics for feet */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation|Inverse Kinematics") bool ShouldCalculateFeetIK();
	virtual bool ShouldCalculateFeetIK_Implementation();

	
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
