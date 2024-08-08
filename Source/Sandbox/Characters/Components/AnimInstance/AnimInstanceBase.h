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



//----------------------------------------------------------------------------------------------------------------------------------//
// Movement Values																													//
//----------------------------------------------------------------------------------------------------------------------------------//
protected:
	/** The character's input */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector2D Input;
	
	/** The character's acceleration */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector Acceleration;

	/** The character's normalized acceleration */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector Acceleration_N;
	
	/** The character's velocity */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector Velocity;
	
	/** The character's normalized velocity */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector Velocity_N;

	/** The character's speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float Speed;

	/** The character's normalized speed (used for blend space logic */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float Speed_N;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector2D WalkRunValues;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector2D CrouchWalkValues;

	/** The added velocity based on the player's direction */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector DirectionalVelocity;

	/** Movement inputs */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector RelativeVelocity;

	/** Which what the player is moving in first person */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") EMovementDirection MovementDirection;
	
	
	/**** Rotations and Aim Values ****/
	/** The character's previous rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator PreviousRotation;

	/** The character's rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator Rotation;

	/** The camera's rotation relative to the player's current rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator RelativeRotation;

	/** A smoothed version of the character rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator SmoothedAimRotation;

	/** The aim rotation, which is a rotation with less smoothing to follow the player's overall aim */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator AimRotation;

	/** The character's movement aim rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator MovementAimRotation;

	/** The blendspace version of the player's aim rotation. This helps normalize the values for handling in multiple blueprints without any errors */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator BlendSpaceAimRotation;

	/** The character's yaw */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float Yaw;

	/** The character's pitch */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float Pitch;

	/** The interp that handles player rotations. This is normally smoothed out to prevent any problems when the player's are moving around inSp third person */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float TurnInPlaceInterp;

	/**** Lean calculations ****/
	/** Lean calculations (we're just using additive animations to blend this dynamically anywhere)
	 * This is captured here because inverse values really mess up interps and everything comes out as negative if you start interp calculations with a positive
	 */
	/** The lean calculation used for adjusting the lean */
	FVector2D LeanCalculation;

	/** The lean amount used for calculating the player's lean */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector2D LeanAmount;

	/** The player's lean interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float LeanInterpSpeed = 3.4;

	/** The lean amount used for calculating the player's lean while wall running */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector2D WallRunLeanAmount;

	/** The player's lean interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float WallRunLeanInterpSpeed = 30;

	
	/**** Character Movement State values ****/
	/** Whether the player is accelerating */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") bool bIsAccelerating;
	
	/** Whether the player is moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") bool bIsMoving;
	
	/** Whether the player is sprinting */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") bool bSprinting;
	
	/** Whether the player is crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") bool bCrouching;
	
	/** Whether the player is walking */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") bool bWalking;
	
	/** The current movement mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") TEnumAsByte<EMovementMode> MovementMode;
	
	/** The custom movement mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") uint8 CustomMovementMode;

	/** The camera style */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FName CameraStyle;
	
	
//----------------------------------------------------------------------------------------------------------------------------------//
// Inverse Kinematics																												//
//----------------------------------------------------------------------------------------------------------------------------------//
protected:
	/**** Spine rotations ****/
	/** The character's spine rotations are being divided into individual bones, there's calculations for both of unreal's skeletons,
	 * the individual values are for the main UE skeleton. These are all just adjusted using the turn in place rotation.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator NormalizedSpineRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator PelvisRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator Spine01Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator Spine02Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator Spine03Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator Spine04Rotation;

	
	/**** Pelvis ****/
	/** The alpha of the pelvis, which influences adjusting the pelvis */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") float PelvisAlpha;
	
	/** The offset of the pelvis */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector PelvisOffset;
	
	/** The target location of the pelvis */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector PelvisTarget;


	/**** Feet ****/
	/** The animation's start time for foot placement during the walk/run animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Anim") float AlternateFootStartTime = 0.45f;
	
	/** The animation's start time for foot placement when the player lands */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Anim") float LandingFootStartTime = 0.0f;
	
	/** The trace distance above the foot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement|IK") float IK_TraceDistanceAboveFoot = 50.0f;
	
	/** The trace distance below the foot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement|IK") float IK_TraceDistanceBelowFoot = 45.0f;
	
	/** The height of the foot (used for ik calculations) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement|IK") float FootHeight = 13.5f;

	
	/**** Left foot ****/
	/** Anim curve to enable/disable the left foot ik during different animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement|IK") FName EnableFootIK_L_Curve = FName("Enable_FootIK_L");
	
	/** The name of the left foot ik bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement|IK") FName IK_Foot_L = FName("ik_foot_l");
	
	/** The ik's target location */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector FootOffsetL_Target;
	
	/** The foot's current location */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector FootOffsetL_Location;
	
	/** The foot's current rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator FootOffsetL_Rotation;

	
	/**** Right foot ****/
	/** Anim curve to enable/disable the right foot ik during different animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement|IK") FName EnableFootIK_R_Curve = FName("Enable_FootIK_R");
	
	/** The name of the right foot ik bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement|IK") FName IK_Foot_R = FName("ik_foot_r");
	
	/** The ik's target location */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector FootOffsetR_Target;
	
	/** The foot's current location */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FVector FootOffsetR_Location;
	
	/** The foot's current rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Movement") FRotator FootOffsetR_Rotation;

	
	/**** Hands ****/
	/** The left hand transform */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|AnimBlueprint|Combat") FTransform LeftHandTransform;
	
	
	/**** References ****/
	/** The character's movement component reference */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|AnimBlueprint") TObjectPtr<UAdvancedMovementComponent> MovementComponent;
	
	/** The character reference */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|AnimBlueprint") TObjectPtr<ACharacterBase> Character;
	
	/** If this is a controlled character or a proxy */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|AnimBlueprint") bool bLocallyControlled;

	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed
	// UPROPERTY(EditDefaultsOnly, Category = "GameplayTags") FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

//------------------------------------------------------------------------------//
// Captured Movement Values														//
//------------------------------------------------------------------------------//
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|AnimBlueprint") float MaxWalkSpeed;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|AnimBlueprint") float MaxCrouchSpeed;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Character|AnimBlueprint") float MaxRunSpeed;
	

//------------------------------------------------------------------------------//
// Movement Calculations														//
//------------------------------------------------------------------------------//
protected:
	/** Calculates teh character's base movement values */
	virtual void CalculateCharacterMovementValues(float DeltaTime);
	
	/** Calculates the yaw, lean, and aim rotations for the character */
	virtual void CalculateYawAndLean(float DeltaTime);

	/** Calculates the movement direction for the character */
	virtual void CalculateMovementDirection();

	
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


/*
 * Handling Spine rotations from the character's aim rotation is smoothed out for better transitions, but I don't know enough to do this completely correctly (share with me your secrets)
 * We're just starting with a third, and decrementing gradually for the rest of the spine bones, which helps with hip pivots, but that doesn't mean that these values don't persist
 * It might just be easier to divide these values evenly, but I'm more concerned about the leg placement than I am the rest of the character's movement in general

	50 = 16.65 + 13.35 + 10 + 6.65 + 3.35




*/
