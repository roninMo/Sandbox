// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/CharacterBase.h"

#include "EnhancedInputComponent.h"
#include "Components/AdvancedMovement/CombatMovementComponent.h"
#include "Components/AnimInstance/AnimInstanceBase.h"
#include "Components/Inventory/InventoryComponent.h"
#include "GameFramework/PlayerState.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"
#include "Sandbox/Data/Enums/SkeletonMappings.h"
#include "Net/UnrealNetwork.h"
#include "Sandbox/Data/Enums/ArmorTypes.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UCombatMovementComponent>(ACharacter::CharacterMovementComponentName)
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
	
	// Character armor
	Gauntlets = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gauntlets"));
	Leggings = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Leggings"));
	Helm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helm"));
	Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));

	ACharacterBase::ConstructArmorInformation(Helm);
	ACharacterBase::ConstructArmorInformation(Gauntlets);
	ACharacterBase::ConstructArmorInformation(Leggings);
	ACharacterBase::ConstructArmorInformation(Chest);
	// TODO: there might be latency issues with pose leader component meshes that causes deformations during movement sometimes, find a fix for this
	// (however this is the standard way of handling modular characters, so it might actually be that we reimported the skeletons and retargeted the current character)
	// This isn't an error, it's because I didn't use the original mannequin - https://forums.unrealengine.com/t/set-master-pose-deforms-mesh/351660
	
	// For handling animations on the server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}


void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ACharacterBase, Armor_Leggings, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ACharacterBase, Armor_Gauntlets, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ACharacterBase, Armor_Helm, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ACharacterBase, Armor_Chest, COND_Custom, REPNOTIFY_OnChanged);
}


#pragma region Character Initialization
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (Leggings) Armor_Leggings = Leggings->GetSkeletalMeshAsset();
	if (Gauntlets) Armor_Gauntlets = Gauntlets->GetSkeletalMeshAsset();
	if (Helm) Armor_Helm = Helm->GetSkeletalMeshAsset();
	if (Chest) Armor_Chest = Chest->GetSkeletalMeshAsset();
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// For Player State ASC Pawns, initialize ASC on server in PossessedBy
	if (APlayerState* PS = GetPlayerState())
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(PS);
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
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(PS);
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
	
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(this);
		if (!AbilitySystemComponent) return;
	}

	// Add ability system state to the anim instance
	UAnimInstanceBase* AnimInstance = GetMesh() ? Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance()) : nullptr;
	if (AnimInstance)
	{
		AnimInstance->InitializeAbilitySystem(AbilitySystemComponent);
	}
	
	// Add the ability input bindings
	// UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	AbilitySystemComponent->BindAbilityActivationToEnhancedInput(EnhancedInputComponent, AbilityInputActions);

	// Inventory component initialization
	if (Inventory)
	{
		Inventory->SetPlayerId();
	}
}


UInputComponent* ACharacterBase::CreatePlayerInputComponent()
{
	return Super::CreatePlayerInputComponent();
}


void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// InputComponent = PlayerInputComponent;
	
	
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(this);
		if (!AbilitySystemComponent) return;
	}
	
	// Add the ability input bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
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


UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBase::SetArmorMesh(EArmorSlot ArmorSlot, USkeletalMesh* Armor)
{
	if (EArmorSlot::Leggings == ArmorSlot)
	{
		Armor_Leggings = Armor;
		if (Leggings) Leggings->SetSkeletalMesh(Armor_Leggings);
	}
	if (EArmorSlot::Gauntlets == ArmorSlot)
	{
		Armor_Gauntlets = Armor;
		if (Gauntlets) Gauntlets->SetSkeletalMesh(Armor_Gauntlets);
	}
	if (EArmorSlot::Helm == ArmorSlot)
	{
		Armor_Helm = Armor;
		if (Helm) Helm->SetSkeletalMesh(Armor_Helm);
	}
	if (EArmorSlot::Chest == ArmorSlot)
	{
		Armor_Chest = Armor;
		if (Chest) Chest->SetSkeletalMesh(Armor_Chest);
	}
}

void ACharacterBase::SetHideCharacterAndArmor(const bool bHide)
{
	if (GetMesh()) GetMesh()->SetOwnerNoSee(bHide);
	if (Gauntlets) Gauntlets->SetOwnerNoSee(bHide);
	if (Leggings) Leggings->SetOwnerNoSee(bHide);
	if (Helm) Helm->SetOwnerNoSee(bHide);
	if (Chest) Chest->SetOwnerNoSee(bHide);
}

ECharacterSkeletonMapping ACharacterBase::GetCharacterSkeletonMapping() const { return CharacterSkeletonMapping; }
USkeletalMeshComponent* ACharacterBase::GetLeggings() const { return Leggings; }
USkeletalMeshComponent* ACharacterBase::GetGauntlets() const { return Gauntlets; }
USkeletalMeshComponent* ACharacterBase::GetHelm() const { return Helm; }
USkeletalMeshComponent* ACharacterBase::GetChest() const { return Chest; }
void ACharacterBase::OnRep_Armor_Gauntlets() { Gauntlets->SetSkeletalMesh(Armor_Gauntlets); }
void ACharacterBase::OnRep_Armor_Leggings() { Leggings->SetSkeletalMesh(Armor_Leggings); }
void ACharacterBase::OnRep_Armor_Helm() { Helm->SetSkeletalMesh(Armor_Helm); }
void ACharacterBase::OnRep_Armor_Chest() { Chest->SetSkeletalMesh(Armor_Chest); }

void ACharacterBase::ConstructArmorInformation(USkeletalMeshComponent* MeshComponent) const
{
	if (MeshComponent)
	{
		MeshComponent->AlwaysLoadOnClient = true;
		MeshComponent->AlwaysLoadOnServer = true;
		MeshComponent->bOwnerNoSee = false;
		MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones; // @ref -> Required for client hit detection with ai
		MeshComponent->bCastDynamicShadow = true;
		MeshComponent->bAffectDynamicIndirectLighting = true;
		MeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		MeshComponent->SetupAttachment(GetMesh());
		static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
		MeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
		MeshComponent->SetGenerateOverlapEvents(false);
		MeshComponent->SetCanEverAffectNavigation(false);

		MeshComponent->SetLeaderPoseComponent(GetMesh());
	}
}


UCombatComponent* ACharacterBase::GetCombatComponent() const
{
	return CombatComponent;
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

