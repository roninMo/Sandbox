// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Sandbox/Data/Enums/InverseKinematicsState.h"
#include "Sandbox/Data/Enums/MovementAnimCurveValues.h"
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

class ACharacterBase; 
class UAdvancedMovementComponent;


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
// Movement																															//
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
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") EMovementDirection MovementDirection = EMD_Neutral;
	
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
	
	
	/**** Rotations ****/
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Aim")
	FVector2D AimRotationAngles = FVector2D(80, 130);

	/** The interp speed of the aim rotation */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Aim")
	float AimRotationInterpSpeed = 6.4;
	

	/**** Lean calculations ****/
	/** The lean interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lean")
	float LeanInterpSpeed = 3.4;
	
	/** The lean calculation */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanCalculation;
	
	/** The lean amount */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D LeanAmount;
	
	/** The wall run lean interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lean")
	float WallRunLeanInterpSpeed = 34;
	
	/** The wall run lean amount */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Lean") FVector2D WallRunLeanAmount;

	
	/**** Character Movement State values ****/
	/** The current movement mode */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") TEnumAsByte<EMovementMode> MovementMode;
	
	/** The custom movement mode */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") uint8 CustomMovementMode;
	
	/** If the player is accelerating */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bIsAccelerating = true;
	
	/** If the player is moving */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bIsMoving;
	
	/** If the player is sprinting */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bSprinting;
	
	/** If the player is crouching */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bCrouching;
	
	/** If the player is walking */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") bool bWalking = true;
	
	/** If the player's camera style */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|State") FName CameraStyle = FName("FirstPerson");


	/**** Animation specific values ****/
	/** The start time of the animation if the player lands on their right foot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Utility", meta = (EditCondition = "bUseFootPlacement", EditConditionHides))
	float LandingFootStartTime = 0.0f;
	
	/** The start time of the animation if the player lands on their left foot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Utility", meta = (EditCondition = "bUseFootPlacement", EditConditionHides))
	float AlternateFootStartTime = 0.45f;

	
//------------------------------------------------------------------------------//
// Captured Movement															//
//------------------------------------------------------------------------------//
	/** The current max walk speed, stored here for blend space references */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxWalkSpeed = 100;
	/** The current max crouch speed, stored here for blend space references */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxCrouchSpeed = 50;
	/** The current max sprint speed, stored here for blend space references */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement") float MaxRunSpeed = 164;



	
//----------------------------------------------------------------------------------------------------------------------------------//
// Inverse Kinematics																												//
//----------------------------------------------------------------------------------------------------------------------------------//
protected:

	
//----------------------------------------------------------------------------------------------------------------------//
// Spine																												//
//----------------------------------------------------------------------------------------------------------------------//
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

	
	/**** Pelvis ****/
	/** The target offset of the pelvis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis")
	float IK_PelvisInterpSpeed = 10;

	/** The height limit from the foot to the pelvis. Used for preventing the leg from overextending during inverse kinematics on inclines */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis", meta = (EditCondition = "bUseFootPlacement"))
	float StandingHeight = 94;

	/** The movement and height (x/y) multiplier for adding an offset based on the leg's movement while walking down inclines. This lets the character lean with the movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis", meta = (EditCondition = "bUseFootPlacement && bUseFootInclinePlacement"))
	FVector2D PelvisInclineInfluence = FVector2D(1, 0.0);

	/** The current offset of the pelvis */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis") FVector PelvisOffset;
	
	/** The target offset of the pelvis */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Pelvis") FVector PelvisTarget;

	

//----------------------------------------------------------------------------------------------------------------------//
// Feet																													//
//----------------------------------------------------------------------------------------------------------------------//
#pragma region Foot Inverse Kinematics
protected:
	/** The name of the left foot bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet")
	FName IKLeftFootBoneName = FName("ik_foot_l");

	/** The name of the right foot bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet")
	FName IKRightFootBoneName = FName("ik_foot_r");
	
	/** The name of the left knee bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet")
	FName LeftKneeBoneName = FName("calf_l");

	/** The name of the right knee bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet")
	FName RightKneeBoneName = FName("calf_r");
	
	/** The name of the left toe bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet")
	FName LeftToeBoneName = FName("ball_l");

	/** The name of the right toe bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet")
	FName RightToeBoneName = FName("ball_r");
	
	/** The target location offset reach for arms inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet", meta = (UIMin = "0.0", UIMax = "100")) 
	float FeetLength = 90;
	
	
//----------------------------------------------------------//
// Foot Placement											//
//----------------------------------------------------------//
#pragma region Foot Placement
protected:
	/** Whether to calculate foot placement inverse kinematics  @remarks Inverse Kinematics for feet is influenced by the IK_Feet value  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Foot Placement")
	bool bUseFootPlacement = true;
	
	/** The trace distance above the foot to trace for placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement", meta = (UIMin = "0", UIMax = "50"))
	float IK_TraceDistanceAboveFoot = 45;
	
	/** The trace distance below the foot to trace for placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement", meta = (UIMin = "0", UIMax = "50"))
	float IK_TraceDistanceBelowFoot = 45;
	
	/** The distance the foot is allowed to be from the floor to activate foot placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement") float ValidDistanceFromFloor;

	/** The distance the foot is allowed to be from the floor during idle to activate foot placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement", meta = (UIMin = "0", UIMax = "25"))
	float ValidDistanceFromFloor_Idle = 8;

	/** The distance the foot is allowed to be from the floor while walking to activate foot placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement", meta = (UIMin = "0", UIMax = "30"))
	float ValidDistanceFromFloor_Walking = 14;

	/** The distance the foot is allowed to be from the floor while running to activate foot placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement", meta = (UIMin = "0", UIMax = "45"))
	float ValidDistanceFromFloor_Running = 25;
	
	/** The interp speed for foot placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement", meta = (UIMin = "0", UIMax = "2"))
	float IK_FootPlacementInterpSpeed = 0.4;

	
	/** An additional offset for adjusting the foot placement (to prevent clipping in the ground). */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement") FVector FootPlacementLocationOffset;

	/** The foot placement offset location for the idle animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement")
	FVector FootPlacementLocationOffset_Idle = FVector(0.0, 0.0, 2);

	/** The foot placement offset location for the walking animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement")
	FVector FootPlacementLocationOffset_Walking = FVector(0.0, 0.0, 2);

	/** The foot placement offset location for the running animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement")
	FVector FootPlacementLocationOffset_Running = FVector(0.0, 0.0, 6.4);

#pragma endregion
	
	
//----------------------------------------------------------//
// Incline Foot Placement									//
//----------------------------------------------------------//
#pragma region Incline Foot Placement
protected:
	/** Whether to calculate foot placement inverse kinematics while walking down inclines  @remarks Inverse Kinematics for feet is influenced by the IK_Feet value  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Foot Placement|Inclines")
	bool bUseFootInclinePlacement = true;
	
	/** The angle of the floor used on the lower foot for adding a height adjustment for fixing foot placement inverse kinematics that use root location for height references */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Inclines|Height")
	FVector2D InclineHeightLowerAdjustmentInRange = FVector2D(0.0, -0.7);
	
	/** The angle of the floor used on the upper foot for adding a height adjustment for fixing foot placement inverse kinematics that use root location for height references */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Inclines|Height")
	FVector2D InclineHeightUpperAdjustmentInRange = FVector2D(0.0, 0.7);
	
	/** The offset used on the lower foot for adding a height adjustment for fixing foot placement inverse kinematics that use root location for height references */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Inclines|Height")
	FVector2D InclineHeightLowerAdjustmentOutRange = FVector2D(-4, -25);

	/** The offset used on the upper foot for adding a height adjustment for fixing foot placement inverse kinematics that use root location for height references */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Inclines|Height")
	FVector2D InclineHeightUpperAdjustmentOutRange = FVector2D(-4, 3);

	/** The trace distance from the foot forward to search for an incline. This also adjusts the map range for foot height  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Inclines|Height")
	float InclineForwardTraceDistance = 54;

	/** The foot height adjustments based on the distance the foot is from an incline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement|Inclines|Height")
	FVector2D InclineForwardAdjustmentRange = FVector2D(20, 5);

#pragma endregion
	
	
//----------------------------------------------------------//
// Foot Movement											//
//----------------------------------------------------------//
#pragma region Foot Movement
protected:
	/** Whether to calculate foot movement inverse kinematics  @remarks Inverse Kinematics for feet is influenced by the IK_Feet value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Foot Movement")
	bool bUseFootMovement = true;
	
	/** How quickly the foot moves towards the character's acceleration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Movement", meta = (UIMin = "0", UIMax = "45"))
	float FootMovementDirectionSpeed = 10;
	
	/** How quickly the foot moves towards the character's acceleration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Movement", meta = (UIMin = "0", UIMax = "1"))
	float FootMovementRotationInfluence = 0.8;
	
	/** The foot movement interp speed, which is used in constant interp functions that are influenced by the player's current speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Movement", meta = (UIMin = "0", UIMax = "1"))
	float IK_FootMovementInterpSpeed = 1;
	
	/** The dot product of the player's velocity and acceleration, to find whether they're changing directions */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Placement") float FootMovementDirectionFactor;
	
#pragma endregion
	
	
//----------------------------------------------------------//
// Foot Locking												//
//----------------------------------------------------------//
#pragma region Foot Locking
protected:
	/** Whether to calculate foot locking inverse kinematics   @remarks Inverse Kinematics for feet is influenced by the IK_Feet value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Foot Locking")
	bool bUseFootLocking = true;
	/** The interp speed for foot locking */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Locking", meta=(UIMin = "0", UIMax = "2"))
	float IK_FootLockInterpSpeed = 0.25;

	/** Curve values transition because of animation blending, so during a blend space sometimes the values are offset when the foot is already in place. This helps with activating kinematics early to account for this */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Locking", meta=(UIMin = "0", UIMax = "0.5"))
	float FootLockActivationOffset = 0.1;

	/** The left foot lock state */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Locking") EInverseKinematicsState LeftFootLockState;
	
	/** The right foot lock state */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Foot Locking") EInverseKinematicsState RightFootLockState;

#pragma endregion
	
	
//----------------------------------------------------------//
// Foot Wall Climbing										//
//----------------------------------------------------------//
#pragma region Foot Wall Climbing
protected:
	/** Whether to calculate wall climbing for feet inverse kinematics   @remarks Inverse Kinematics for arms is influenced by the Inverse Kinematics Arm_L and Arm_R values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Wall Climbing")
	bool bUseFootWallClimbing = true;

	/** The interp speed for the feet while wall climbing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Wall Climbing", meta = (UIMin = "0", UIMax = "1"))
	float IK_WallClimbFootInterpSpeed = 1;

	/** Foot Wall Climbing's Forward trace distance for the hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Wall Climbing", meta = (UIMin = "0.0", UIMax = "20"))
	float FootWallClimbForwardTraceDistance = 5;
	
	/** Foot Wall Climbing's Backwards trace distance for the hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Wall Climbing", meta = (UIMin = "-64", UIMax = "0.0"))
	float FootWallClimbBackwardTraceDistance = -25;
	
	/** Foot Wall Climbing's second (horizontal) trace distance trace distance for the hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Feet|Wall Climbing", meta = (UIMin = "0.0", UIMax = "50"))
	float FootWallClimbHorizontalTraceDistance = 35;

#pragma endregion 

	
//----------------------------------------------------------//
// Foot Joint Targets										//
//----------------------------------------------------------//
#pragma region Foot Joint Targets
protected:
	/** The interp speed for arm's joint targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets", meta = (UIMin = "0.0", UIMax = "30"))
	float JointTargetFootInterpSpeed = 15;

	/** The distance the target's are from the player's knee. This is influenced by the distance from the foot to the pelvis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets") 
	FVector2D JT_FootTargetLocationDistanceRange = FVector2D(100, 164);

	/** Creates a target offset based on the leg's rotation. You shouldn't adjust the rotations unless you want to exaggerate the rotations based on the leg's movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets|Walking") 
	FVector2D JT_FootRotation_InRange = FVector2D(-30, 30);

	/** Creates a target offset based on the leg's rotation. You shouldn't adjust the rotations unless you want to exaggerate the rotations based on the leg's movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets|Walking") 
	FVector2D JT_FootRotation_OutRange = FVector2D(-30, 30);

	/** When the leg pivots forwards/backwards while walking, rotate the joint target to follow the player's movement. For example, forwards and left would rotate -30 to the left, and backwards would go 30 to the right */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets|Walking") 
	FVector2D JT_FootMovementHorizontalRotation = FVector2D(-45, 20);

	
	/** Adjusts the target rotation during wall climbing when the player moves to the left or right */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets|Wall Climbing") 
	FVector2D JT_FootClimbHorizontalRotations = FVector2D(-25, 25);

	/** Adjusts the target rotation during wall climbing based on the legs rotations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets|Wall Climbing") 
	FVector2D JT_FootClimbLegRotations = FVector2D(-5, 60);

	/** Adjusts the target rotation during wall climbing based on the leg's height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets|Wall Climbing") 
	FVector2D JT_FootClimbHeightRotations = FVector2D(0, 90);

	/** When the feet are above the pelvis add an offset to prevent bad rotations. This helps for when the player is falling and the feet slide up and create bad rotations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Joint Targets|Wall Climbing") 
	FVector2D JT_FootClimbPlacementFix = FVector2D(0, 100);

	
#pragma endregion 

	
//----------------------------------------------------------//
// Left Foot												//
//----------------------------------------------------------//
#pragma region Left Foot
protected:
	/**** Current locations ****/
	/** The coordinated movement offset location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootMovementLocation_L;
	
	/** The foot placement offset location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootPlacementLocation_L;
	
	/** The foot lock offset location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLockLocation_L;
	
	/** The wall climb offset location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector WallClimbFootLocation_L;
	
	/** The ledge placement offset location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector LedgePlacementFootLocation_L;
	
	/** The current location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLocation_L;

	
	/**** Target locations ****/
	/** The coordinated movement target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootMovementTarget_L;
	
	/** The foot placement target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootPlacementTarget_L;
	
	/** The foot lock target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLockTarget_L;
	
	/** The wall climb target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector WallClimbFootTarget_L;
	
	/** The ledge placement target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector LedgePlacementFootTarget_L;
	
	/** The target location for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FVector FootLocationTarget_L;

	
	/**** Current rotations ****/
	/** The coordinated movement rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootMovementRotation_L;
	
	/** The foot placement rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootPlacementRotation_L;
	
	/** The foot lock rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootLockRotation_L;
	
	/** The wall climb rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator WallClimbFootRotation_L;
	
	/** The ledge placement rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator LedgePlacementFootRotation_L;
	
	/** The current rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootRotation_L;

	
	/**** Target rotations ****/
	/** The coordinated movement target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootMovementRotationTarget_L;
	
	/** The foot placement target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootPlacementRotationTarget_L;
	
	/** The foot lock target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootLockRotationTarget_L;
	
	/** The wall climb target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator WallClimbFootRotationTarget_L;
	
	/** The ledge placement target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator LedgePlacementFootRotationTarget_L;
	
	/** The target rotation for the left foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Left Foot") FRotator FootRotationTarget_L;
#pragma endregion 

	
//----------------------------------------------------------//
// Right Foot												//
//----------------------------------------------------------//
#pragma region Right Foot
protected:
	/**** Current locations ****/
	/** The coordinated movement offset location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootMovementLocation_R;
	
	/** The foot placement offset location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootPlacementLocation_R;
	
	/** The foot lock offset location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLockLocation_R;
	
	/** The wall climb offset location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector WallClimbFootLocation_R;
	
	/** The ledge placement location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector LedgePlacementFootLocation_R;
	
	/** The current location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLocation_R;

	
	/**** Target locations ****/
	/** The coordinated movement target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootMovementTarget_R;
	
	/** The foot placement target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootPlacementTarget_R;
	
	/** The foot lock target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLockTarget_R;
	
	/** The wall climb target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector WallClimbFootTarget_R;
	
	/** The ledge placement location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector LedgePlacementFootTarget_R;
	
	/** The target location for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FVector FootLocationTarget_R;
	
	
	/**** Current rotations ****/
	/** The coordinated movement rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootMovementRotation_R;
	
	/** The foot placement rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootPlacementRotation_R;
	
	/** The foot lock rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootLockRotation_R;
	
	/** The wall climb rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator WallClimbFootRotation_R;
	
	/** The ledge placement rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator LedgePlacementFootRotation_R;
	
	/** The current rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootRotation_R;

	
	/**** Target rotations ****/
	/** The coordinated movement target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootMovementRotationTarget_R;
	
	/** The foot lock target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootLockRotationTarget_R;
	
	/** The foot placement target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootPlacementRotationTarget_R;
	
	/** The wall climb target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator WallClimbFootRotationTarget_R;
	
	/** The ledge placement rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator LedgePlacementFootRotationTarget_R;
	
	/** The target rotation for the right foot */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Right Foot") FRotator FootRotationTarget_R;
	
#pragma endregion

#pragma endregion 

	
	

//------------------------------------------------------------------------------------------------------------------//
// Arms																												//
//------------------------------------------------------------------------------------------------------------------//
protected:
	/** The name of the left hand bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms")
	FName IKLeftHandBoneName = FName("ik_hand_l");

	/** The name of the right hand bone used for creating inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms")
	FName IKRightHandBoneName = FName("ik_hand_r");

	/** The name of the left shoulder bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms")
	FName LeftElbowBoneName = FName("lowerarm_l");

	/** The name of the right shoulder bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms")
	FName RightElbowBoneName = FName("lowerarm_r");

	/** The name of the left shoulder bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms")
	FName LeftShoulderBoneName = FName("upperarm_l");

	/** The name of the right shoulder bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms")
	FName RightShoulderBoneName = FName("upperarm_r");

	/** The target location offset reach for arms inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement", meta = (UIMin = "0.0", UIMax = "100")) 
	float ArmsLength = 57;
	

//----------------------------------------------------------//
// Wall Placement											//
//----------------------------------------------------------//
#pragma region Wall Placement
protected:
	/** Whether to calculate wall placement for arms inverse kinematics  @remarks Inverse Kinematics for arms is influenced by the Inverse Kinematics Arm_L and Arm_R values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Wall Placement")
	bool bUseArmsWallPlacement = true;
	
	/** Whether to calculate wall running for arms inverse kinematics while walking down inclines  @remarks Inverse Kinematics for arms is influenced by the Inverse Kinematics Arm_L and Arm_R values  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Wall Placement")
	bool bUseArmsWallRunning = true;

	
	/**** Calculations ****/
	/** The interp speed for arms wall placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement", meta = (UIMin = "0.0", UIMax = "2"))
	float ArmsWallPlacementInterpSpeed = 2;
	
	/** The outwards trace distance for arms inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement", meta = (UIMin = "0.0", UIMax = "100")) 
	float ArmsTraceDistance = 25;
	
	/** The trace radius for arms inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement", meta = (UIMin = "0.0", UIMax = "100")) 
	float ArmsTraceRadius = 50;
	
	/** The forwards, sideways, and upwards trace distance offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement") 
	FVector ArmsTraceOffset = FVector(10, 5, 55);
	
	/** For finding valid placement, deals with the acceptable angle when the player's arm crosses their body */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement", meta = (UIMin = "0.0", UIMax = "0.34")) 
	float ArmsValidCrossingAngle = 0.134;

	/** For finding valid placement, deals with the acceptable angle when the player's arm is extended back to their side */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement", meta = (UIMin = "0.0", UIMax = "1")) 
	float ArmsValidTurningAngle = 0.6;

	
	/**** Locations and Rotations ****/
	/** The arm's rotation during wall placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement")
	FRotator ArmsWallPlacementRotation = FRotator(180, 45, 0);
	
	/** The spacing offset for hand placement on the wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement", meta = (UIMin = "0.0", UIMax = "10"))
	float ArmsWallHandSpacing = 5;
	
	/** The offsets for moving the arm forwards/backwards based on the rotation of the player. If they're facing the wall, then move it away from the character, and away pushes the arm away */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement") 
	FVector2D ArmsPlayerRotationPlacementOffset = FVector2D(-35, 25);
	
	/** The offsets for moving the arm forwards/backwards based on the movement speed of the player  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement") 
	FVector2D ArmsMovementOffset = FVector2D(-25, 25);
	
	/** The multiplier for the movement offset when the player is sprinting  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement") 
	float ArmsMovementOffsetSprintMultiplier = 1.25;
	
	/** The offsets for adjusting the height of the arm the closer it is to the wall when the player isn't moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement") 
	FVector2D ArmsMovementHeightPlacementOffset = FVector2D(0, -35);
	
	/** The offsets for adjusting the height of the arm the closer it is to the wall when the player isn't moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Wall Placement") 
	FVector2D ArmsHeightPlacementOffset = FVector2D(10, 0);

#pragma endregion 
	
	
//----------------------------------------------------------//
// Wall Climbing											//
//----------------------------------------------------------//
#pragma region Wall Climbing
protected:
	/** Whether to calculate wall climbing for arms inverse kinematics   @remarks Inverse Kinematics for arms is influenced by the Inverse Kinematics Arm_L and Arm_R values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Wall Climbing")
	bool bUseArmsWallClimbing = true;
	
	/** The interp speed for the arms while wall climbing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Feet|Wall Climbing", meta = (UIMin = "0", UIMax = "2"))
	float IK_WallClimbArmsInterpSpeed = 1.34;

	/** Arms Wall Climbing's Forward trace distance for the hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Wall Climbing", meta = (UIMin = "0.0", UIMax = "20"))
	float ArmsWallClimbForwardTraceDistance = 5;
	
	/** Arms Wall Climbing's Backwards trace distance for the hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Wall Climbing", meta = (UIMin = "-45", UIMax = "0.0"))
	float ArmsWallClimbBackwardTraceDistance = -20;
	
	/** Arms Wall Climbing's second (horizontal) trace distance trace distance for the hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Wall Climbing", meta = (UIMin = "0.0", UIMax = "50"))
	float ArmsWallClimbHorizontalTraceDistance = 35;

#pragma endregion 
	
	
//----------------------------------------------------------//
// Ledge Placement											//
//----------------------------------------------------------//
#pragma region Ledge Placement
protected:
	/** Whether to calculate ledge placement for arms inverse kinematics   @remarks Inverse Kinematics for arms is influenced by the Inverse Kinematics Arm_L and Arm_R values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Ledge Placement")
	bool bUseArmsLedgePlacement = true;

	/** Arm's trace radius for ledge placement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Inverse Kinematics|Arms|Ledge Placement", meta = (UIMin = "0.0", UIMax = "25"))
	float ArmsLedgePlacementTraceRadius = 10;
	
#pragma endregion 

	
//----------------------------------------------------------//
// Joint Targets											//
//----------------------------------------------------------//
#pragma region Arm Joint Targets
protected:
	/** The interp speed for arm's joint targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets", meta = (UIMin = "0.0", UIMax = "2"))
	float JointTargetArmInterpSpeed = 0.75;
	
	/** How we should rotate the targets based on the player's rotation to the wall. If the player turns away then we want the arms to be rotated back */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	FVector2D JT_RotationToWallRange = FVector2D(-6.4, -80);

	/** How we should adjust the height based on the player's rotation to the wall. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	FVector2D JT_RotationToWallHeightRange = FVector2D(34, -100);
	
	/** The angle the elbow should be based on the player's distance from the wall. The closer they are to the wall the more we want the arms to bend */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	FVector2D JT_DistanceToWallRotationRange = FVector2D(0, 90);

	
	/** The arm should go back and up when the hand is behind the shoulder. This helps with walking away from corners and any movements where the arm is stuck behind the shoulder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	FVector2D JT_ArmBehindBackOffsetRange_Default = FVector2D(0, 15);
	
	/** The arm should go back and up when the hand is behind the shoulder. This is a multiplier for how far back the offset should go. It helps have precedence over other influences  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	float JT_ArmBehindBackDistance_Default = -5;

	/** The arm rotations based on the current angle of the arm, added to the safe joint target offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	FVector2D JT_ArmHorizontalRotations = FVector2D(20, -25);
	
	/** The arm should go back and up when the hand is behind the shoulder. This helps with walking away from corners and any movements where the arm is stuck behind the shoulder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	FVector2D JT_ArmBehindBackOffsetRange_Transition = FVector2D(0, 45);
	
	/** The arm should go back and up when the hand is behind the shoulder. This is a multiplier for how far back the offset should go. It helps have precedence over other influences  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	float JT_ArmBehindBackDistance_Transition = -4.3;

	/** A height offset that the target location adjusts to the more the arm bends up. This fixes any upper arm movement twists  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	float JT_ArmBendOffsetHeightAdjustLimit = -100;

	
	/** The idle angle of the arm. Used for calculating offset rotations based on how the arm naturally bends, and it's natural idle rotation. Use arm angles for reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets", meta = (UIMin = "0.0", UIMax = "90")) 
	float JT_ArmIdleAngle = 50;
	
	/** Rotate the target location based on the difference in the arms idle angle so the target location is in sync with the animation. This prevents twists during transitions for inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets", meta = (UIMin = "0.0", UIMax = "64")) 
	float JT_ArmRotationLimits = 50;
	
	/** The rotation ranges based on the arm's horizontal rotation during transitions. This keeps the target location in sync with animations and prevents twists during transitions for inverse kinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") 
	FVector2D JT_ArmRotationTransitionRange = FVector2D(20, -45);

	
	/** The current location of joint target for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") FVector CurrentJointTargetLocation_L;
	
	/** The joint target location of the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") FVector JointTargetLocation_L;
	
	/** The current location of joint target for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") FVector CurrentJointTargetLocation_R;
	
	/** The joint target location of the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") FVector JointTargetLocation_R;

	/** The left arm's x and y angles (used for handling joint target offset rotations for the character. This helps adjusting the joint target while blending between random arm angles */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") FVector2D ArmAngles_L;
	
	/** The right arm's x and y angles (used for handling joint target offset rotations for the character. This helps adjusting the joint target while blending between random arm angles */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Joint Targets") FVector2D ArmAngles_R;
	
#pragma endregion 
	

	
//----------------------------------------------------------//
// Inverse Kinematic States									//
//----------------------------------------------------------//
#pragma region Arms Inverse Kinematic States
protected:
	/** The different versions of inverse kinematic calculations for arms */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EArmsInverseKinematics ArmsInverseKinematicsState;

	/** The inverse kinematics state during wall placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState LeftArmWallPlacementState;
	
	/** The inverse kinematics state during wall placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState RightArmWallPlacementState;

	/** The inverse kinematics state during wall climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState LeftArmWallClimbingState;
	
	/** The inverse kinematics state during wall climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState RightArmWallClimbingState;

	/** The inverse kinematics state during wall climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState LeftFootWallClimbingState;
	
	/** The inverse kinematics state during wall climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState RightFootWallClimbingState;

	/** The inverse kinematics state during ledge placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState LeftArmLedgePlacementState;
	
	/** The inverse kinematics state during ledge placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState RightArmLedgePlacementState;

	/** The inverse kinematics state during ledge placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState LeftFootLedgePlacementState;
	
	/** The inverse kinematics state during ledge placement */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms") EInverseKinematicsState RightFootLedgePlacementState;

#pragma endregion 
	
	
//----------------------------------------------------------//
// Left Arm													//
//----------------------------------------------------------//
#pragma region Left Arm
protected:
	/**** Current Locations ****/
	/** The wall placement offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmWallPlacementLocation_L;
	
	/** The arms interaction offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ActionArmLocation_L;
	
	/** The wall running offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmWallRunningLocation_L;
	
	/** The wall climbing offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmWallClimbingLocation_L;
	
	/** The ledge placement offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmLedgePlacementLocation_L;

	/** The current location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmLocation_L;
	

	/**** Target Locations ****/
	/** The wall placement target offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmWallPlacementTargetLocation_L;
	
	/** The arms interaction target offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ActionTargetArmLocation_L;
	
	/** The wall running target offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmWallRunningTargetLocation_L;
	
	/** The wall climbing target offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmWallClimbingTargetLocation_L;
	
	/** The ledge placement target offset location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmLedgePlacementTargetLocation_L;

	/** The target location for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FVector ArmTargetLocation_L;

	
	/**** Current Rotations ****/
	/** The wall placement offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmWallPlacementRotation_L;
	
	/** The arms interaction offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ActionArmRotation_L;
	
	/** The wall running offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmWallRunningRotation_L;
	
	/** The wall climbing offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmWallClimbingRotation_L;
	
	/** The ledge placement offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmLedgePlacementRotation_L;

	/** The current rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmRotation_L;


	/**** Target Rotations ****/
	/** The wall placement target offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmWallPlacementTargetRotation_L;
	
	/** The arms interaction target offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ActionTargetArmRotation_L;
	
	/** The wall running target offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmWallRunningTargetRotation_L;
	
	/** The wall climbing target offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmWallClimbingTargetRotation_L;
	
	/** The ledge placement target offset rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmLedgePlacementTargetRotation_L;

	/** The target rotation for the left arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Left Arm") FRotator ArmTargetRotation_L;

#pragma endregion 
	
	
//----------------------------------------------------------//
// Right Arm												//
//----------------------------------------------------------//
#pragma region Right Arm
protected:
	/**** Current Locations ****/
	/** The wall placement offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmWallPlacementLocation_R;
	
	/** The arms interaction offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ActionArmLocation_R;
	
	/** The wall running offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmWallRunningLocation_R;
	
	/** The wall climbing offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmWallClimbingLocation_R;
	
	/** The ledge placement offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmLedgePlacementLocation_R;

	/** The current location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmLocation_R;
	

	/**** Target Locations ****/
	/** The wall placement target offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmWallPlacementTargetLocation_R;
	
	/** The arms interaction target offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ActionTargetArmLocation_R;
	
	/** The wall running target offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmWallRunningTargetLocation_R;
	
	/** The wall climbing target offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmWallClimbingTargetLocation_R;
	
	/** The ledge placement target offset location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmLedgePlacementTargetLocation_R;

	/** The target location for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FVector ArmTargetLocation_R;

	
	/**** Current Rotations ****/
	/** The wall placement offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmWallPlacementRotation_R;
	
	/** The arms interaction offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ActionArmRotation_R;
	
	/** The wall running offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmWallRunningRotation_R;
	
	/** The wall climbing offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmWallClimbingRotation_R;
	
	/** The ledge placement offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmLedgePlacementRotation_R;

	/** The current rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmRotation_R;


	/**** Target Rotations ****/
	/** The wall placement target offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmWallPlacementTargetRotation_R;
	
	/** The arms interaction target offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ActionTargetArmRotation_R;
	
	/** The wall running target offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmWallRunningTargetRotation_R;
	
	/** The wall climbing target offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmWallClimbingTargetRotation_R;
	
	/** The ledge placement target offset rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmLedgePlacementTargetRotation_R;

	/** The target rotation for the right arm */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Inverse Kinematics|Arms|Right Arm") FRotator ArmTargetRotation_R;

#pragma endregion 
	
	

	
//------------------------------------------------------------------------------//
// Debugging																	//
//------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Movement") bool bDebugIKFootMovementInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Movement") bool bDebugIKFootMovementTraces;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Movement") float IKFootMovementTraceDuration = 0.1; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Placement") bool bDebugIKFootPlacementInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Placement") bool bDebugIKFootPlacementTraces;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Foot Placement") float IKFootPlacementTraceDuration = 0.2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Pelvis") bool bDebugPelvisOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Placement") bool bDebugArmWallPlacementInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Placement") bool bDebugArmWallPlacementTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Placement") float ArmWallPlacementTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Climbing") bool bDebugArmWallClimbingInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Climbing") bool bDebugArmWallClimbingTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Climbing") float ArmWallClimbingTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Climbing") bool bDebugFootWallClimbingInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Climbing") bool bDebugFootWallClimbingTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Wall Climbing") float FootWallClimbingTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Ledge Placement") bool bDebugLedgePlacementInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Ledge Placement") bool bDebugLedgePlacementTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Ledge Placement") float LedgePlacementTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Joint Targets") bool bDebugArmJointTargetInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Joint Targets") bool bDebugArmJointTargetTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Joint Targets") float ArmJointTargetTraceDuration = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Joint Targets") bool bDebugKneeJointTargetInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Joint Targets") bool bDebugKneeJointTargetTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Debugging|Joint Targets") float KneeJointTargetTraceDuration = 0.1;
	


	
//----------------------------------------------------------------------------------------------------------------------------------//
// Curve Values	(setting the curve values to 0 prevents (Montages/Overlays/Ik) from influencing animations							//
//----------------------------------------------------------------------------------------------------------------------------------//
#pragma region Curve Values
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
	/** Whether to prevent sprinting during certain animations. This isn't networking safe, and probably shouldn't be used */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Movement", meta=(DisplayPriority=0)) float Mask_Sprint; // 1 = Prevent sprint
	
	/** Whether to prevent leaning during certain animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Lean") float Mask_Lean; // 1 = Prevent lean
	
	/** Value to store the rotation during turn in place animations. This isn't networking safe, and probably shouldn't be used */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Movement", meta=(DisplayPriority=0)) float Turn_RotationAmount; // -1, 1 = Left, Right


	/**** Inverse Kinematic Alphas ****/
	/** Value to store whether the left foot should be stuck to the ground to create a realistic movement feel. This way the player isn't just sliding everywhere based on other player's interpretations */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float Foot_Lock_L;

	/** Value to store whether the right foot should be stuck to the ground to create a realistic movement feel. This way the player isn't just sliding everywhere based on other player's interpretations */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float Foot_Lock_R;

	/** Value to store whether to adjust the foot height to account for inclines while the player is walking */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float Foot_Incline_L;

	/** Value to store whether to adjust the foot height to account for inclines while the player is walking */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float Foot_Incline_R;
	
	/** Value to store whether to adjust inverse kinematics influence for placing the foot on the ground */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float FootPlacement_L = 1;

	/** Value to store whether to adjust inverse kinematics influence for placing the foot on the ground */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float FootPlacement_R = 1;

	/** Value to store whether to adjust inverse kinematics influence for foot movement coordination */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float FootMovement_L = 1;

	/** Value to store whether to adjust inverse kinematics influence for foot movement coordination */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float FootMovement_R = 1;

	
	/** Value to store whether to adjust inverse kinematics influence for hand placement during interactions */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmInteraction_L = 1;

	/** Value to store whether to adjust inverse kinematics influence for hand placement during interactions */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmInteraction_R = 1;
	
	/** Value to store whether to adjust inverse kinematics influence for placing hands on the wall */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmWallPlacement_L = 1;

	/** Value to store whether to adjust inverse kinematics influence for placing hands on the wall */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmWallPlacement_R = 1;
	
	/** Value to store whether to adjust inverse kinematics influence for wall running hand adjustments */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmWallRunning_L = 1;

	/** Value to store whether to adjust inverse kinematics influence for wall running hand adjustments */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmWallRunning_R = 1;
	
	/** Value to store whether to adjust inverse kinematics influence for wall climbing hand adjustments. This uses arm locking while the player is climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmWallClimbing_L;

	/** Value to store whether to adjust inverse kinematics influence for wall climbing hand adjustments. This uses arm locking while the player is climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmWallClimbing_R;
	
	/** Value to store whether the left foot should be stuck to the wall to create a realistic climbing feel. This uses foot locking while the player is climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float FootWallClimbing_L;

	/** Value to store whether the right foot should be stuck to the wall to create a realistic climbing feel. This uses foot locking while the player is climbing */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float FootWallClimbing_R;

	/** Value to store whether to adjust inverse kinematics influence for ledge placement hand adjustments */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmLedgePlacement_L = 1;

	/** Value to store whether to adjust inverse kinematics influence for ledge placement hand adjustments */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Curves|Movement") float ArmLedgePlacement_R = 1;

	
	
	
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


	/**** Curve Names ****/
	/** The name of the curve for left hand locking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Names") FName HandLockCurveName_L = Curve_Hand_Lock_L;
	
	/** The name of the curve for right hand locking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Names") FName HandLockCurveName_R = Curve_Hand_Lock_R;
	
	/** The name of the curve for left foot locking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Names") FName FootLockCurveName_L = Curve_Foot_Lock_L;
	
	/** The name of the curve for right foot locking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Names") FName FootLockCurveName_R = Curve_Foot_Lock_R;

	/** The name of the curve for left foot incline adjustments */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Names") FName FootInclineCurveName_L = Curve_Foot_Incline_L;
	
	/** The name of the curve for right foot incline adjustments */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Curves|Names") FName FootInclineCurveName_R = Curve_Foot_Incline_R;
	
#pragma endregion 
	


	
//------------------------------------------------------------------------------//
// References																	//
//------------------------------------------------------------------------------//
protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Utility") TObjectPtr<UAdvancedMovementComponent> MovementComponent;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Utility") TObjectPtr<ACharacterBase> Character;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Movement|Utility") bool bLocallyControlled;
	
	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed
	// UPROPERTY(EditDefaultsOnly, Category = "GameplayTags") FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	/** Actors that are ignored during inverse kinematic traces */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Inverse Kinematics|Utility") TArray<AActor*> IgnoredActors;


	
//------------------------------------------------------------------------------//
// Movement Calculations														//
//------------------------------------------------------------------------------//
protected:
	UAnimInstanceBase(const FObjectInitializer& ObjectInitializer);
	
	/** Calculates the character's base movement values */
	virtual void GetCharacterMovementValues(float DeltaTime);

	/** Calculates the movement direction for the character */
	virtual void CalculateMovementDirection();

	/** Finds the curve values and stores the information for bp reference */
	virtual void UpdateCurveValues();
	
	/** Calculates the yaw, lean, and aim rotations for the character */
	virtual void CalculateYawAndLean(float DeltaTime);
	
	
//------------------------------------------------------------------------------//
// Utility																		//
//------------------------------------------------------------------------------//
public:
	/** Adds an actor to ignore during inverse kinematic traces */
	UFUNCTION(BlueprintCallable) virtual void AddActorToIgnoreDuringInverseKinematics(AActor* Actor);
	
	/** Removes an ignored actor during inverse kinematic traces */
	UFUNCTION(BlueprintCallable) virtual void RemoveIgnoredActorDuringInverseKinematics(AActor* Actor);
	
	/** Clears all ignored actors during inverse kinematic traces */
	UFUNCTION(BlueprintCallable) virtual void ClearInverseKinematicIgnoredActors();

	
	UFUNCTION(BlueprintCallable) virtual bool GetCharacterInformation();
	UFUNCTION(BlueprintCallable) virtual bool GetMovementComponent();
	UFUNCTION(BlueprintCallable) virtual bool GetCharacter();

	UFUNCTION(BlueprintCallable) virtual bool IsPlayingInEditor(UObject* WorldContextObject) const;
	UFUNCTION(BlueprintCallable) virtual bool IsPlayingInGame(UObject* WorldContextObject) const;

	
};
