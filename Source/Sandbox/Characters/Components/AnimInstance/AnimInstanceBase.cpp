// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceBase.h"

#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Characters/Player/PlayerCharacter.h"

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
	
	CalculateCharacterMovementValues(DeltaTime);
	CalculateYawAndLean(DeltaTime);
	CalculateMovementDirection();
}


void UAnimInstanceBase::CalculateCharacterMovementValues(float DeltaTime)
{
	// The character's velocity, speed and rotation
	Velocity = Character->GetVelocity(); 
	Velocity_N = Velocity.GetSafeNormal();
	Speed = Velocity.Size();
	Speed_N = UKismetMathLibrary::MapRangeClamped(Speed, -MovementComponent->GetMaxSpeed(), MovementComponent->GetMaxSpeed(), -1, 1);
	PreviousRotation = Rotation;
	Rotation = Character->GetActorRotation();
	Acceleration = MovementComponent->GetCurrentAcceleration();
	Input = MovementComponent->GetPlayerInput();
	
	// This is the movement vector based on where the player is facing
	DirectionalVelocity = UKismetMathLibrary::Quat_UnrotateVector(Rotation.Quaternion(), Velocity); // The speed of the forward vector direction
	RelativeVelocity = UKismetMathLibrary::Quat_UnrotateVector(Velocity.ToOrientationQuat(), Character->GetActorRotation().Vector()); // Movement inputs
	
	// The essentials for character movement calculations
	bIsAccelerating = Acceleration.Size() > 0 ? true : false;
	bIsMoving = Velocity.IsNearlyZero(1);
	bSprinting = MovementComponent->IsRunning();
	bCrouching = Character->bIsCrouched;
	bWalking = !bSprinting && MovementComponent->IsWalking();
	MovementMode = MovementComponent->MovementMode;
	CustomMovementMode = MovementComponent->CustomMovementMode;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Character);
	if (PlayerCharacter)CameraStyle = PlayerCharacter->GetCameraStyle();

	// Blendspace forwards and sideways values (converted into -1, 1) for handling multiple blendspaces
	const float MaxWalkSpeed = MovementComponent->GetMaxWalkSpeed() * MovementComponent->SprintSpeedMultiplier;
	WalkRunValues = FVector2D(
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.X, -MaxWalkSpeed, MaxWalkSpeed, -1, 1),
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.Y, -MaxWalkSpeed, MaxWalkSpeed, -1, 1)
	);

	const float MaxCrouchSpeed = MovementComponent->MaxWalkSpeedCrouched * MovementComponent->CrouchSprintSpeedMultiplier;
	CrouchWalkValues = FVector2D(
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.X, -MaxCrouchSpeed, MaxCrouchSpeed, -1, 1),
		UKismetMathLibrary::MapRangeClamped(DirectionalVelocity.Y, -MaxCrouchSpeed, MaxCrouchSpeed, -1, 1)
	);
}


void UAnimInstanceBase::CalculateYawAndLean(float DeltaTime)
{
	// Offset yaw for strafing on the server (Looking up or down)
	AimRotation = Character->GetBaseAimRotation(); // The current direction the character is facing in the world // GetBaseAimRotation: built in function to grab the offset of where the character is aiming
	Yaw = AimRotation.Yaw; 
	const FRotator RelativeRotation = UKismetMathLibrary::NormalizedDeltaRotator(Rotation, AimRotation); // The rotation relative to the character's current rotation
	SmoothedAimRotation = FMath::RInterpTo(SmoothedAimRotation, RelativeRotation, DeltaTime, 15.f);
	Pitch = -AimRotation.Pitch;
	
	// The character lean is interp that's based on the player's relative acceleration (It's just based on the player's movement inputs, and an additive animation multiplier adjusts how much is applied for each animation with anim curves
	LeanCalculation = FVector2D(
		UKismetMathLibrary::FInterpTo(LeanCalculation.X, RelativeVelocity.X, DeltaTime, LeanInterpSpeed),
		UKismetMathLibrary::FInterpTo(LeanCalculation.Y, RelativeVelocity.Y, DeltaTime, LeanInterpSpeed)
	);
	const float SpeedFactor = FMath::Abs(Speed_N);
	LeanAmount = FVector2D(
		LeanCalculation.X * SpeedFactor,
		-LeanCalculation.Y * SpeedFactor
	);
	// UE_LOGFMT(LogTemp, Log, "LeanCalculation: {0}, LeanAmount: {1}, SpeedFactor: {2}", *LeanCalculation.ToString(), *LeanAmount.ToString(), SpeedFactor);
	
	// The spine rotation of the character (It's just a clamped value of the character's yaw rotation)
	float YawClamped = UKismetMathLibrary::FClamp(Yaw, -90, 90);
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
	BlendSpaceAimRotation = FRotator(
		FMath::Clamp(SmoothedAimRotation.Pitch, -84, 84),
		FMath::Clamp(SmoothedAimRotation.Yaw, -90, 90),
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

