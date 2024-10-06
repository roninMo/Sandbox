// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceBase.h"

#include "AbilitySystemGlobals.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Characters/Player/PlayerCharacter.h"
#include "Sandbox/Data/Enums/MovementAnimCurveValues.h"



DEFINE_LOG_CATEGORY(LogAnimationBlueprint);


void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	GetCharacterInformation();
}


void UAnimInstanceBase::InitializeAbilitySystem(UAbilitySystemComponent* Asc)
{
	if (Asc)
	{
		// check(ASC);
		GameplayTagPropertyMap.Initialize(this, Asc);
	}
}


void UAnimInstanceBase::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (!GetCharacterInformation())
	{
		return;
	}

	// Save the player's movement information
	UpdateCurveValues();
	GetCharacterMovementValues(DeltaTime);

	// Calculate movement logic
	CalculateYawAndLean(DeltaTime);
	CalculateMovementDirection();
}


void UAnimInstanceBase::GetCharacterMovementValues(float DeltaTime)
{
	// Movement component values
	MaxWalkSpeed = MovementComponent->GetMaxWalkSpeed();
	MaxCrouchSpeed = MovementComponent->MaxWalkSpeedCrouched;
	MaxRunSpeed = MovementComponent->GetMaxWalkSpeed() * MovementComponent->SprintSpeedMultiplier;
	
	// The character's velocity, speed and rotation
	Input = MovementComponent->GetPlayerInput();
	Acceleration = MovementComponent->GetCurrentAcceleration();
	Acceleration_N = Acceleration.GetSafeNormal();
	Velocity = Character->GetVelocity(); 
	Velocity_N = Velocity.GetSafeNormal();
	Speed = Velocity.Size();
	Speed_N = UKismetMathLibrary::MapRangeClamped(Speed, -MovementComponent->GetMaxSpeed(), MovementComponent->GetMaxSpeed(), -1, 1);
	Rotation = Character->GetActorRotation();
	
	// This is the movement vector based on where the player is facing
	DirectionalVelocity = UKismetMathLibrary::Quat_UnrotateVector(Rotation.Quaternion(), Velocity); // The speed of the forward vector direction
	RelativeVelocity = UKismetMathLibrary::Quat_UnrotateVector(Velocity.ToOrientationQuat(), Character->GetActorRotation().Vector()); // Movement inputs
	if (!bIsMoving) RelativeVelocity = FVector::ZeroVector;
	
	// The essentials for character movement calculations
	MovementMode = MovementComponent->MovementMode;
	CustomMovementMode = MovementComponent->CustomMovementMode;
	bIsAccelerating = Acceleration.Size() > 0 ? true : false;
	bIsMoving = !Velocity.IsNearlyZero(1);
	bSprinting = MovementComponent->IsRunning();
	bCrouching = Character->bIsCrouched;
	bWalking = !bSprinting && MovementComponent->IsWalking();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Character);
	if (PlayerCharacter)CameraStyle = PlayerCharacter->Execute_GetCameraStyle(PlayerCharacter);

	// Blendspace forwards and sideways values (converted into -1, 1) for handling multiple blendspaces
	WalkRunValues = FVector2D(
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.X, -MaxRunSpeed, MaxRunSpeed, -1, 1),
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.Y, -MaxRunSpeed, MaxRunSpeed, -1, 1)
	);

	const float MaxCrouchSprintSpeed = MaxCrouchSpeed * MovementComponent->CrouchSprintSpeedMultiplier;
	CrouchWalkValues = FVector2D(
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.X, -MaxCrouchSprintSpeed, MaxCrouchSprintSpeed, -1, 1),
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.Y, -MaxCrouchSprintSpeed, MaxCrouchSprintSpeed, -1, 1)
	);
}


void UAnimInstanceBase::CalculateYawAndLean(float DeltaTime)
{
	// Offset yaw for strafing on the server (Looking up or down)
	AimRotation = Character->GetBaseAimRotation(); // The current direction the character is facing in the world // GetBaseAimRotation: built in function to grab the offset of where the character is aiming
	Yaw = AimRotation.Yaw; 
	Pitch = -AimRotation.Pitch;
	RelativeRotation = UKismetMathLibrary::NormalizedDeltaRotator(Rotation, AimRotation); // The rotation relative to the character's current rotation
	
	// The character lean is interp that's based on the player's relative acceleration using the player's movement inputs
	// UE_LOGFMT(LogTemp, Log, "LeanCalculation: {0}, LeanAmount: {1}, SpeedFactor: {2}", *LeanCalculation.ToString(), *LeanAmount.ToString(), SpeedFactor);
	const float SpeedFactor = FMath::Abs(Speed_N);
	LeanCalculation = FVector2D(
		UKismetMathLibrary::FInterpTo(LeanCalculation.X, RelativeVelocity.X, DeltaTime, LeanInterpSpeed),
		UKismetMathLibrary::FInterpTo(LeanCalculation.Y, RelativeVelocity.Y, DeltaTime, LeanInterpSpeed)
	);
	LeanAmount = FVector2D(
		LeanCalculation.X * SpeedFactor,
		-LeanCalculation.Y * SpeedFactor
	);

	// Wall run calculations
	if (CustomMovementMode != MOVE_Custom_WallRunning) WallRunLeanAmount = FVector2D::ZeroVector;
	else
	{
		FVector WallRunVector = RelativeVelocity - Character->GetActorForwardVector();
		WallRunLeanAmount = FVector2D(
			UKismetMathLibrary::FInterpTo(WallRunLeanAmount.X, WallRunVector.X * 1.5, DeltaTime, WallRunLeanInterpSpeed),
			UKismetMathLibrary::FInterpTo(WallRunLeanAmount.Y, WallRunVector.Y * 1.5, DeltaTime, WallRunLeanInterpSpeed)
		);
	}
	
	// The spine rotation of the character (It's just a clamped value of the character's yaw rotation)
	const float YawClamped = UKismetMathLibrary::FClamp(Yaw, -90, 90);
	NormalizedSpineRotation = FRotator(0, YawClamped / 4, 0);
	PelvisRotation = FRotator(0, YawClamped * 0.333f, 0);
	Spine01Rotation = FRotator(0, YawClamped * 0.267f, 0);
	Spine02Rotation = FRotator(0, YawClamped * 0.2f, 0);
	Spine03Rotation = FRotator(0, YawClamped * 0.133f, 0);
	Spine04Rotation = FRotator(0, YawClamped * 0.067f, 0);
	
	// The aim rotation based on character movement
	MovementAimRotation = DirectionalVelocity.ToOrientationRotator();
	MovementAimRotation.Pitch = SmoothedAimRotation.Pitch;
	
	// The aim rotation for blendspace animations
	SmoothedAimRotation = FMath::RInterpTo(SmoothedAimRotation, RelativeRotation, DeltaTime, AimRotationInterpSpeed);
	SmoothedAimRotation = FRotator(
		FMath::Clamp(SmoothedAimRotation.Pitch, -AimRotationAngles.Y, AimRotationAngles.Y),
		FMath::Clamp(SmoothedAimRotation.Yaw, -AimRotationAngles.X, AimRotationAngles.X),
		FMath::Clamp(SmoothedAimRotation.Roll, -90, 90)
	);
}


void UAnimInstanceBase::AddActorToIgnoreDuringInverseKinematics(AActor* Actor)
{
	if (!Actor) return;
	IgnoredActors.AddUnique(Actor);
}

void UAnimInstanceBase::RemoveIgnoredActorDuringInverseKinematics(AActor* Actor)
{
	if (!Actor) return;
	IgnoredActors.Remove(Actor);
}

void UAnimInstanceBase::ClearInverseKinematicIgnoredActors()
{
	IgnoredActors.Empty();
}


void UAnimInstanceBase::CalculateMovementDirection()
{
	const float Forwards = Input.X;
	const float Sideways = Input.Y;

	if (Sideways != 0 && Forwards != 0)
	{
		if (Forwards > 0) MovementDirection = Sideways > 0 ? EMD_ForwardRight : EMD_ForwardLeft;
		else MovementDirection = Sideways > 0 ? EMD_BackwardRight : EMD_BackwardLeft;
	}

	if (Sideways == 0 && Forwards == 0) MovementDirection = EMD_Neutral;
	if (Sideways == 0) MovementDirection = Forwards > 0 ? EMD_Forward : EMD_Backward;
	MovementDirection = Sideways > 0 ? EMD_Right : EMD_Left;

	Forward = UKismetMathLibrary::MapRangeClamped(Input.X, 0, 1, 0, 1);
	Backward = UKismetMathLibrary::MapRangeClamped(Input.X, 0, -1, 0, 1);
	Left = UKismetMathLibrary::MapRangeClamped(Input.Y, 0, -1, 0, 1);
	Right = UKismetMathLibrary::MapRangeClamped(Input.Y, 0, 1, 0, 1);
}




void UAnimInstanceBase::UpdateCurveValues()
{
	// Montage Overrides
	Montage_Head = GetCurveValue(Curve_Montage_Head);
	Montage_Pelvis = GetCurveValue(Curve_Montage_Pelvis);
	Montage_Spine = GetCurveValue(Curve_Montage_Spine);
	Montage_Legs = GetCurveValue(Curve_Montage_Legs);
	Montage_Arm_L = GetCurveValue(Curve_Montage_Arm_L);
	Montage_Arm_R = GetCurveValue(Curve_Montage_Arm_R);
	Montage_Hand_L = GetCurveValue(Curve_Montage_Hand_L);
	Montage_Hand_R = GetCurveValue(Curve_Montage_Hand_R);

	// Overlay Overrides
	Layering_Head = GetCurveValue(Curve_Layering_Head);
	Layering_Pelvis = GetCurveValue(Curve_Layering_Pelvis);
	Layering_Spine = GetCurveValue(Curve_Layering_Spine);
	Layering_Legs = GetCurveValue(Curve_Layering_Legs);
	Layering_Arm_L = GetCurveValue(Curve_Layering_Arm_L);
	Layering_Arm_R = GetCurveValue(Curve_Layering_Arm_R);

	// IK influence
	IK_Head = GetCurveValue(Curve_IK_Head);
	IK_Pelvis = GetCurveValue(Curve_IK_Pelvis);
	IK_Spine = GetCurveValue(Curve_IK_Spine);
	IK_Feet = GetCurveValue(Curve_IK_Feet);
	IK_Arm_L = GetCurveValue(Curve_IK_Arm_L);
	IK_Arm_R = GetCurveValue(Curve_IK_Arm_R);
	IK_Hand_L = GetCurveValue(Curve_IK_Hand_L);
	IK_Hand_R = GetCurveValue(Curve_IK_Hand_R);

	// Primary values
	Turn_RotationAmount = GetCurveValue(Curve_Turn_RotationAmount);
	Mask_Sprint = GetCurveValue(Curve_Mask_Sprint);
	Mask_Lean = GetCurveValue(Curve_Mask_Lean);

	// AO influence
	AO_Head = GetCurveValue(Curve_AO_Head);
	AO_Pelvis = GetCurveValue(Curve_AO_Pelvis);
	AO_Spine = GetCurveValue(Curve_AO_Spine);
	AO_Legs = GetCurveValue(Curve_AO_Legs);
	AO_Arm_L = GetCurveValue(Curve_AO_Arm_L);
	AO_Arm_R = GetCurveValue(Curve_AO_Arm_R);
}


UAnimInstanceBase::UAnimInstanceBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LeftArmWallPlacementState = EInverseKinematicsState::IK_Disabled;
	RightArmWallPlacementState = EInverseKinematicsState::IK_Disabled;
	LeftArmWallClimbingState = EInverseKinematicsState::IK_Disabled;
	RightArmWallClimbingState = EInverseKinematicsState::IK_Disabled;
	LeftFootWallClimbingState = EInverseKinematicsState::IK_Disabled;
	RightFootWallClimbingState = EInverseKinematicsState::IK_Disabled;
	LeftArmLedgePlacementState = EInverseKinematicsState::IK_Disabled;
	RightArmLedgePlacementState = EInverseKinematicsState::IK_Disabled;
	LeftFootLedgePlacementState = EInverseKinematicsState::IK_Disabled;
	RightFootLedgePlacementState = EInverseKinematicsState::IK_Disabled;
}




#pragma region Utility
bool UAnimInstanceBase::GetCharacterInformation()
{
	if (MovementComponent && Character) return true;
	if (!GetCharacter()) return false;
	if (!GetMovementComponent()) return false;
	return true;
}


bool UAnimInstanceBase::GetMovementComponent()
{
	if (MovementComponent) return true;
	if (!GetCharacter()) return false;

	MovementComponent = Character->GetAdvancedMovementComp();
	if (!MovementComponent)
	{
		if (IsPlayingInGame(this)) UE_LOGFMT(LogAnimationBlueprint, Error, "{0} {1}() failed to get {2}'s movement component!", *UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		return false;
	}
	
	return true;
}


bool UAnimInstanceBase::GetCharacter()
{
	if (Character) return true;

	Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!Character)
	{
		if (IsPlayingInGame(this)) UE_LOGFMT(LogAnimationBlueprint, Error, "{0} {1}() {2} failed to get the character!", *UEnum::GetValueAsString(GetOwningActor()->GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return false;
	}

	return true;
}


bool UAnimInstanceBase::IsPlayingInEditor(UObject* WorldContextObject) const
{
	if (!WorldContextObject || !WorldContextObject->GetWorld()) return false;
	return WorldContextObject->GetWorld()->IsEditorWorld();
}


bool UAnimInstanceBase::IsPlayingInGame(UObject* WorldContextObject) const
{
	if (!WorldContextObject || !WorldContextObject->GetWorld()) return false;
	return WorldContextObject->GetWorld()->IsGameWorld();
}
#pragma endregion 

