// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/CharacterBase.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerState.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UAdvancedMovementComponent>(ACharacter::CharacterMovementComponentName)
)
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetReplicates(true);
	
	//////////////////////// Replication stuff (Server/Client rendering) 
	NetUpdateFrequency = 66.f; // default update character on other machines 66 times a second (general fps defaults)
	MinNetUpdateFrequency = 33.f; // To help with bandwidth and lagginess, allow a minNetUpdateFrequency, which is generally 33 in fps games
	// The other value is the server config tick rate, which is in the project defaultEngine.ini -> [/Script/OnlineSubsystemUtils.IpNetDriver] NetServerMaxTickRate = 60
	// also this which is especially crucial for implementing the gameplay ability system defaultEngine.ini -> [SystemSettings] net.UseAdaptiveNetUpdateFrequency = 1

	
}




#pragma region Character Initialization
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// For Player State ASC Pawns, initialize ASC on server in PossessedBy
	if (APlayerState* PS = GetPlayerState())
	{
		AbilitySystemComponent = UGameplayAbilitiyUtilities::GetAbilitySystem(PS);
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
			OnInitAbilityActorInfo(PS, this);
		}
	}
}


void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// For Player State ASC Pawns, initialize ASC on clients in OnRep_PlayerState
	if (APlayerState* PS = GetPlayerState())
	{
		AbilitySystemComponent = UGameplayAbilitiyUtilities::GetAbilitySystem(PS);
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
			OnInitAbilityActorInfo(PS, this);
		}
	}
}


void ACharacterBase::OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// Blueprint function event
	BP_OnInitAbilityActorInfo();
}


UInputComponent* ACharacterBase::CreatePlayerInputComponent()
{
	return Super::CreatePlayerInputComponent();
}


void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Add the ability input bindings
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	AbilitySystemComponent->BindAbilityActivationToEnhancedInput(EnhancedInputComponent, AbilityInputActions);
}
#pragma endregion 




#pragma region Utility
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

UInventoryComponent* ACharacterBase::GetInventoryComponent() const
{
	return Inventory;
}


UAdvancedMovementComponent* ACharacterBase::GetAdvancedMovementComp() const
{
	return GetMovementComp<UAdvancedMovementComponent>();
}


bool ACharacterBase::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}
#pragma endregion

