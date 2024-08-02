// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UAdvancedMovementComponent>(ACharacter::CharacterMovementComponentName)
)
{
	
}




#pragma region Character Initialization
void ACharacterBase::InitCharacterGlobals(UDataAsset* Data)
{
}


void ACharacterBase::InitCharacterComponents(const bool bCalledFromPossessedBy)
{
}


void ACharacterBase::InitAbilitySystemAndAttributes(const bool bCalledFromPossessedBy)
{
}


void ACharacterBase::InitCharacterInformation()
{
}


UAISense_Sight::EVisibilityResult ACharacterBase::CanBeSeenFrom(const FCanBeSeenFromContext& Context,
	FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,
	float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	FVector TargetLocation = GetActorLocation() + AISightTraceOffset;
	FHitResult HitResult;
	const FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), true, Context.IgnoreActor);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Context.ObserverLocation, TargetLocation, ECC_Visibility, QueryParams, FCollisionResponseParams::DefaultResponseParam);

	++OutNumberOfLoSChecksPerformed;

	if (IsTraceConsideredVisible(bHit ? &HitResult : nullptr, this))
	{
		OutSeenLocation = TargetLocation;
		return UAISense_Sight::EVisibilityResult::Visible;
	}
	else
	{
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}
}


bool ACharacterBase::IsTraceConsideredVisible(const FHitResult* HitResult, const AActor* TargetActor)
{
	if (HitResult == nullptr)
	{
		return true;
	}

	const AActor* HitResultActor = HitResult->HitObjectHandle.FetchActor();
	return (HitResultActor ? HitResultActor->IsOwnedBy(TargetActor) : false);
}


UAdvancedMovementComponent* ACharacterBase::GetAdvancedCharacterMovementComponent() const
{
	return GetMovementComp<UAdvancedMovementComponent>();
}


bool ACharacterBase::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}
#pragma endregion




void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}
