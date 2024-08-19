// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceBase.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

	// Handle Inverse Kinematics
	CalculateFeetIK(DeltaTime);
	CalculateArmsIK(DeltaTime);
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




#pragma region Inverse Kinematics
#pragma region Feet Ik
void UAnimInstanceBase::CalculateFeetIK(float DeltaTime)
{
	if (!CanCalculateFeetIK())
	{
		ResetIKFeetAndPelvisOffsets(DeltaTime);
		return;
	}

	// Foot placement
	// if (ShouldCalculateFootPlacementIK())
	// {
	// 	FootPlacementInverseKinematics(DeltaTime, IKLeftFootBoneName, FootPlacementLocation_L, FootPlacementTarget_L, FootPlacementRotation_L, FootPlacementRotationTarget_L);
	// 	FootPlacementInverseKinematics(DeltaTime, IKRightFootBoneName, FootPlacementLocation_R, FootPlacementTarget_R, FootPlacementRotation_R, FootPlacementRotationTarget_R);
	// }
	// else
	// {
	// 	ResetIKFeetAndPelvisOffsets(DeltaTime);
	// }
	
	// FootLocation_L = FootPlacementLocation_L + FootLockLocation_L;
	// FootLocation_R = FootPlacementLocation_R + FootLockLocation_R;
	// FootRotation_L = FootPlacementRotation_L + (AddRotationOffset ? FootLockRotation_L + IKLeftFootRotationOffset : FootLockRotation_L);
	// FootRotation_R = FootPlacementRotation_R + (AddRotationOffset ? FootLockRotation_R + IKRightFootRotationOffset : FootLockRotation_R);

	// PelvisOffsetForFootPlacement(DeltaTime, FootLocation_L, FootLocation_R, PelvisTarget, PelvisOffset);
}


void UAnimInstanceBase::FootPlacementInverseKinematics(float DeltaTime, FName IKFootBone, FVector& CurrentOffset, FVector& TargetOffset, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset)
{;
	FVector RootLocation = GetOwningComponent()->GetSocketLocation(FName("root"));
	FVector FootLocation = GetOwningComponent()->GetSocketLocation(IKFootBone);
	
	FHitResult Hit;	
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		FootLocation + FVector(0, 0, IK_TraceDistanceAboveFoot),
		FootLocation - FVector(0, 0, IK_TraceDistanceBelowFoot),
		TraceTypeQuery1,
		false,
		{},
		bDebugIKFootPlacement ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		Hit,
		true,
		FColor::Red,
		FColor::Emerald,
		IKFootPlacementTraceDuration
	);

	// Adjust the target location and rotation
	if (MovementComponent->IsWalkable(Hit))
	{
		// Floor location + Foot dimensions 
		TargetOffset = Hit.ImpactPoint - FootLocation;
		if (TargetOffset.Z >= FootOffsetRollAdjustHeight)
		{
			float Adjusted = UKismetMathLibrary::MapRangeClamped(FMath::Abs(TargetRotationOffset.Pitch),
				UpperFootOffsetInRange.X, UpperFootOffsetInRange.Y,
				UpperFootOffsetOutRange.X, UpperFootOffsetOutRange.Y
			);
			// UE_LOGFMT(LogTemp, Log, "{0} ->  TargetOffset Z: {1}, Adjusted: -{2}", IKFootBone, TargetOffset.Z, Adjusted);
			TargetOffset.Z = TargetOffset.Z - Adjusted;
		}
		
		// Find the target rotation
		float FloorPitch = 0;
		float FloorRoll = 0;
		UKismetMathLibrary::GetSlopeDegreeAngles(Character->GetActorRightVector(), Hit.ImpactNormal, Character->GetActorUpVector(), FloorPitch, FloorRoll);
		TargetRotationOffset = FRotator(-FloorRoll, 0, -FloorPitch); // This is a fix for translating to the character's bone rotations
	}
	
	// Interp speed should only be natural if they're putting their foot down
	float InterpSpeed = IK_FootPlacementInterpSpeed;
	if (CurrentOffset.Z <= TargetOffset.Z) InterpSpeed = IK_FootInterpSpeedTransition;

	// Location and Rotation interps
	CurrentOffset = UKismetMathLibrary::VInterpTo(CurrentOffset, TargetOffset, DeltaTime, InterpSpeed);

	if (bIsMoving) CurrentRotationOffset = UKismetMathLibrary::RInterpTo(CurrentRotationOffset, FRotator(0, 0, TargetRotationOffset.Roll), DeltaTime, InterpSpeed);
	else CurrentRotationOffset = UKismetMathLibrary::RInterpTo(CurrentRotationOffset, TargetRotationOffset, DeltaTime, InterpSpeed);
	
	if (bDebugIKFootPlacement)
	{
		DrawDebugBox(
			GetWorld(),
			FootLocation + CurrentOffset + (GetOwningComponent()->GetSocketQuaternion(IKFootBone).GetRightVector() * (IKFootBone == IKLeftFootBoneName ? 7.4 : -7.4)) + FVector(0, 0, 3),
			FVector(3, 15, 5),
			GetOwningComponent()->GetSocketQuaternion(IKFootBone),
			FColor::Turquoise,
			false,
			IKFootPlacementTraceDuration
		);
	}
}


void UAnimInstanceBase::PelvisOffsetForFootPlacement(float DeltaTime, FVector& LeftFootLocation, FVector& RightFootLocation, FVector& PelvisTargetLocation, FVector& PelvisOffsetLocation)
{
	if (LeftFootLocation.Z <= RightFootLocation.Z) PelvisTargetLocation = LeftFootLocation;
	else PelvisTargetLocation = RightFootLocation;

	float InterpSpeed = PelvisOffsetLocation.Z <= PelvisTargetLocation.Z ? IK_FootPlacementInterpSpeed : IK_FootInterpSpeedTransition;
	PelvisOffsetLocation = UKismetMathLibrary::VInterpTo(PelvisOffsetLocation, PelvisTargetLocation, DeltaTime, InterpSpeed); 
}


void UAnimInstanceBase::ResetIKFeetAndPelvisOffsets(float DeltaTime)
{
	FootPlacementLocation_L = UKismetMathLibrary::VInterpTo(FootPlacementLocation_L, FVector::ZeroVector, DeltaTime, IK_FootInterpSpeedTransition);
	FootPlacementRotation_L = UKismetMathLibrary::RInterpTo(FootPlacementRotation_L, FRotator::ZeroRotator, DeltaTime, IK_FootInterpSpeedTransition);
	FootPlacementLocation_R = UKismetMathLibrary::VInterpTo(FootPlacementLocation_R, FVector::ZeroVector, DeltaTime, IK_FootInterpSpeedTransition);
	FootPlacementRotation_R = UKismetMathLibrary::RInterpTo(FootPlacementRotation_R, FRotator::ZeroRotator, DeltaTime, IK_FootInterpSpeedTransition);
}


bool UAnimInstanceBase::CanCalculateFeetIK_Implementation()
{
	if (IK_Feet == 0) return false;
	return true;
}
#pragma endregion




#pragma region Arms IK
void UAnimInstanceBase::CalculateArmsIK(float DeltaTime)
{
	// Handle state values for handling different versions of inverse kinematics
	if (CustomMovementMode == MOVE_Custom_WallRunning)
	{
		if (!bWallRunInverseKinematics || IK_WallRunState == EInverseKinematicsState::IK_TransitionOut)
		{
			bWallRunInverseKinematics = true;
			ArmsInverseKinematicsAlpha = 1;
			IK_WallRunState = EInverseKinematicsState::IK_TransitionIn;
		}
	}
	else if (bWallRunInverseKinematics)
	{
		if (IK_WallRunState == EInverseKinematicsState::IK_Enabled) IK_WallRunState = EInverseKinematicsState::IK_TransitionOut;
		if (IK_WallRunState == EInverseKinematicsState::IK_Disabled) bWallRunInverseKinematics = false;
	}

	// Inverse kinematics during wall running
	if (bWallRunInverseKinematics)
	{
		CalculateWallRunArmsIK(DeltaTime);
	}
	else if (bWallPlacementInverseKinematics)
	{
		WallPlacementArmInverseKinematics(DeltaTime);
	}
	else
	{
		ResetArmIKOffsets(DeltaTime, ArmLocationOffset_L, ArmLocationTarget_L, ArmRotationOffset_L, ArmRotationTarget_L);
		ResetArmIKOffsets(DeltaTime, ArmLocationOffset_R, ArmLocationTarget_R, ArmRotationOffset_R, ArmRotationTarget_R);
	}
}


#pragma region Wall Placement
void UAnimInstanceBase::WallPlacementArmInverseKinematics(float DeltaTime)
{
	
}
#pragma endregion




#pragma region Wall Running
void UAnimInstanceBase::CalculateWallRunArmsIK(float DeltaTime)
{
	if (CustomMovementMode == MOVE_Custom_WallRunning)
	{
		float Angle = MovementComponent->GetWallRunImpactNormal().Dot(Character->GetActorRightVector());
		bRightHandWallRun = Angle <= 0 ? true : false;
	}
	
	if (!bRightHandWallRun)
	{
		WallRunArmInverseKinematics(DeltaTime, LeftArmBoneName, ArmLocationOffset_L, ArmLocationTarget_L, ArmRotationOffset_L, ArmRotationTarget_L);
	}
	else
	{
		WallRunArmInverseKinematics(DeltaTime, RightArmBoneName, ArmLocationOffset_R, ArmLocationTarget_R, ArmRotationOffset_R, ArmRotationTarget_R);
	}
}


void UAnimInstanceBase::WallRunArmInverseKinematics(float DeltaTime, FName IKHandBone, FVector& CurrentOffset, FVector& TargetOffset, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset)
{
	const FVector ArmLocation = GetOwningComponent()->GetSocketLocation(IKHandBone);
	const FVector CharacterLocation = GetOwningComponent()->GetSocketLocation("spine_05");
	const FVector RootLocation = GetOwningComponent()->GetSocketLocation("root");
	if (ArmLocation == FVector::ZeroVector || CharacterLocation == FVector::ZeroVector) return;
	
	// We're using world space for handling inverse kinematics for hands, so adjust the initial offset so we're able to use interp speeds without problems
	if (IK_WallRunState == EInverseKinematicsState::IK_TransitionIn)
	{
		IK_WallRunState = EInverseKinematicsState::IK_Enabled;
		bWallRunInverseKinematics = true;
		CurrentOffset = ArmLocation;
		TargetOffset = ArmLocation;
		ResetLocationOffset = ArmLocation - CharacterLocation;
		ResetRotationOffset = GetOwningComponent()->GetSocketRotation(IKHandBone);
	}
	
	FVector WallNormal = MovementComponent->GetWallRunImpactNormal();
	FHitResult Hit;	
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		CharacterLocation,
		CharacterLocation + -WallNormal * WallRunTraceDistance,
		TraceTypeQuery1,
		false,
		{},
		bDebugArmTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		Hit,
		true,
		FColor::Turquoise,
		FColor::Emerald,
		ArmTraceDuration
	);

	
	// Interp the location and rotation
	if (IK_WallRunState == EInverseKinematicsState::IK_Enabled)
	{
		// Adjust the interp speed based on the character's movement speed
		const float InterpSpeed = FMath::Clamp(WallRunArmsInterpSpeed * (Speed / MaxRunSpeed), 1, 100);

		// Location calculations
		const FVector ForwardVector = WallNormal.RotateAngleAxis(90 * (IKHandBone == RightArmBoneName ? 1 : -1), FVector(0, 0, 1));
		if (Hit.IsValidBlockingHit())
		{
			TargetOffset = Hit.Location + FVector(0, 0, WallRunArmHeightOffset) + (ForwardVector * WallRunArmLength) + (WallNormal * WallRunHandSpacing);
		}
		else
		{
			const FVector RightVector = WallNormal * (IKHandBone == RightArmBoneName ? 1 : -1);
			TargetOffset = CharacterLocation + (Character->GetActorForwardVector() * WallRunArmLength) + ((WallRunArmWidthOffset - WallRunHandSpacing) * -RightVector) + FVector(0, 0, WallRunArmHeightOffset);
		}
		CurrentOffset = UKismetMathLibrary::VInterpTo(CurrentOffset, TargetOffset, DeltaTime, InterpSpeed);
		
		if (bDebugArmTrace)
		{
			if (IKHandBone == LeftArmBoneName) DrawDebugBox(GetWorld(), TargetOffset, FVector(3), FColor::Red, false, ArmTraceDuration);
			if (IKHandBone == RightArmBoneName) DrawDebugBox(GetWorld(), TargetOffset, FVector(3), FColor::Emerald, false, ArmTraceDuration);
		}

		// Rotation calculations
		TargetRotationOffset = WallNormal.Rotation() + (IKHandBone == LeftArmBoneName ? WallRunLeftHandRotation : WallRunRightHandRotation);
		CurrentRotationOffset = UKismetMathLibrary::RInterpTo(CurrentRotationOffset, TargetRotationOffset, DeltaTime, InterpSpeed);
	}
	// TODO: Inverse kinematics in world space is tough during transitions if you're not using ik and referencing the actual bones, this needs to be refactored and there needs to be logic for proper rotations based on the hand
	else if (IK_WallRunState == EInverseKinematicsState::IK_TransitionOut)
	{
		// Adjust the interp speed based on the character's movement speed
		const float InterpSpeed = FMath::Clamp(WallRunArmsInterpSpeedTransition * (Speed / MaxRunSpeed), 1, 100);
		
		const FVector RightVector = Character->GetActorRightVector() * (IKHandBone == RightArmBoneName ? 1 : -1);
		TargetOffset = CharacterLocation + (Character->GetActorForwardVector() * WallRunArmLength) + (((WallRunArmWidthOffset * 0.8) - WallRunHandSpacing) * RightVector) + FVector(0, 0, WallRunArmHeightOffset / 2);
		CurrentOffset = UKismetMathLibrary::VInterpTo(CurrentOffset, TargetOffset, DeltaTime, InterpSpeed);
		
		// Adding both location and rotations when transitioning causes problems during inverse kinematics, so we handle the rotations before we disable inverse kinematics
		if (!CurrentRotationOffset.Equals(ResetRotationOffset, 1))
		{
			CurrentRotationOffset = UKismetMathLibrary::RInterpTo_Constant(CurrentRotationOffset, ResetRotationOffset, DeltaTime, InterpSpeed * 45);
		}
		else if (ArmsInverseKinematicsAlpha != 0)
		{
			ArmsInverseKinematicsAlpha = UKismetMathLibrary::FInterpTo(ArmsInverseKinematicsAlpha, 0, DeltaTime, WallRunAlphaInterpSpeed);
		}
		else
		{
			IK_WallRunState = EInverseKinematicsState::IK_Disabled;
		}
	}
	else
	{
		ResetArmIKOffsets(DeltaTime, CurrentOffset, TargetOffset, CurrentRotationOffset, TargetRotationOffset, GetOwningComponent()->GetSocketLocation(IKHandBone));
	}
}
#pragma endregion


void UAnimInstanceBase::ResetArmIKOffsets(float DeltaTime, FVector& CurrentOffset, FVector& TargetOffset, FRotator& CurrentRotationOffset, FRotator& TargetRotationOffset, const FVector Location)
{
	CurrentOffset = UKismetMathLibrary::VInterpTo(CurrentOffset, Location, DeltaTime, WallRunArmsInterpSpeedTransition); 
	CurrentRotationOffset = UKismetMathLibrary::RInterpTo(CurrentRotationOffset, FRotator::ZeroRotator, DeltaTime, WallRunArmsInterpSpeedTransition);
	TargetOffset = Location;
	TargetRotationOffset = FRotator::ZeroRotator;
}
#pragma endregion 
#pragma endregion 




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
	//// IK ////
	LeftFootBoneName = FName("foot_l");
	IKLeftFootBoneName = FName("ik_foot_l");
	RightFootBoneName = FName("foot_r");
	IKRightFootBoneName = FName("ik_foot_r");
	
	IK_TraceDistanceAboveFoot = 45;
	IK_TraceDistanceBelowFoot = 20;
	IK_FootPlacementInterpSpeed = 10;
	IK_FootInterpSpeedTransition = 20;
	IK_PelvisInterpSpeed = 10;
	LegLength = 64;
	
	FootPlacementLocationOffset = FVector(0, 0, 0);
	FootOffsetRollAdjustHeight = -2;
	UpperFootOffsetInRange = FVector2D(0, 15);
	UpperFootOffsetOutRange = FVector2D(2, 5);

	IK_FootLockInterpSpeed = 10;
	FootLockActivationOffset = 0.01;
	IKLeftFootRotationOffset = FRotator(90, 0, 0);
	IKRightFootRotationOffset = FRotator(-90, 180, 0);
	
	// Arms
	LeftArmBoneName = FName("hand_l");
	RightArmBoneName = FName("hand_r");
	WallRunTraceDistance = 64;
	WallRunArmsInterpSpeed = 10;
	WallRunArmsInterpSpeedTransition = 25;
	WallRunAlphaInterpSpeed = 3;
	WallRunArmLength = 54;
	WallRunArmHeightOffset = 40;
	WallRunHandSpacing = 5;
	WallRunLeftHandRotation = FRotator(45,-90, 0);
	WallRunRightHandRotation = FRotator(-135, 90, 0);
	WallRunArmWidthOffset = 30;

	
	// Animation specific values
	AlternateFootStartTime = 0.45;
	LandingFootStartTime = 0;
	
	// Rotations and aim offsets
	AimRotationAngles = FVector2D(80, 130);
	AimRotationInterpSpeed = 6.4;

	
	// Lean calculations
	LeanInterpSpeed = 3.4;
	WallRunLeanInterpSpeed = 34;

	
	// Movement logic
	MovementDirection = EMD_Neutral;
	MovementMode = MOVE_Walking;
	CustomMovementMode = 0;
	bIsAccelerating = true;
	bIsMoving = false;
	bSprinting = false;
	bCrouching = false;
	bWalking = true;
	CameraStyle = FName();

	
	//// Curve Values ////
	// Montage Overrides
	Montage_Head = 1;
	Montage_Pelvis = 1;
	Montage_Spine = 1;
	Montage_Legs = 1;
	Montage_Arm_L = 1;
	Montage_Arm_R = 1;
	Montage_Hand_L = 1;
	Montage_Hand_R = 1;

	// Overlay Overrides
	Layering_Head = 1;
	Layering_Pelvis = 1;
	Layering_Spine = 1;
	Layering_Legs = 1;
	Layering_Arm_L = 1;
	Layering_Arm_R = 1;

	// IK influence
	IK_Head = 0;
	IK_Pelvis = 0;
	IK_Spine = 0;
	IK_Feet = 1;
	IK_Arm_L = 0;
	IK_Arm_R = 0;
	IK_Hand_L = 0;
	IK_Hand_R = 0;

	// AO influence
	AO_Head = 1;
	AO_Pelvis = 1;
	AO_Spine = 1;
	AO_Legs = 1;
	AO_Arm_L = 1;
	AO_Arm_R = 1;

	// Primary Values
	FootPlacement_L = 1;
	FootPlacement_R = 1;
	Foot_Lock_L = 0;
	Foot_Lock_R = 0;
	Turn_RotationAmount = 0;
	Mask_Lean = 0;
	Mask_Sprint = 0;

	// Captured Movement Component Values
	MaxWalkSpeed = 100;
	MaxCrouchSpeed = 50;
	MaxRunSpeed = 164;
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

