// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/CharacterBase.h"

#include "Sandbox/Data/Enums/ArmorTypes.h"
#include "Sandbox/Data/Structs/CombatInformation.h"
// #include "Sandbox/Data/Enums/SkeletonMappings.h"
#include "Sandbox/Data/Enums/HitDirection.h"
#include "Sandbox/Data/Enums/HitReacts.h"

#include "Components/AdvancedMovement/CombatMovementComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "GameFramework/PlayerState.h"
#include "Components/AnimInstance/AnimInstanceBase.h"
#include "Components/Saving/SaveComponent.h"

#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"
#include "Net/UnrealNetwork.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Game/MultiplayerGameMode.h"


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

	// Character montages
	CharacterSkeletonMapping = ECharacterSkeletonMapping::Manny;
	CharacterMontageId = FName("Manny");
	
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

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// if (HasAuthority())
	// {
	// 	UE_LOGFMT(LogTemp, Warning, "{0}::{1}() {2} PostInitializeComponents on server, ready for replication", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
	// }
}


void ACharacterBase::OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(this);
		if (!AbilitySystemComponent) return;
	}
	
	// Save component initialization
	if (SaveComponent && HasAuthority())
	{
		SaveComponent->InitializeSaveLogic();
		SaveComponent->LoadPlayerInformation();
	}

	// Add ability system state to the anim instance
	SetCharacterMontages();
	UAnimInstanceBase* AnimInstance = GetMesh() ? Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance()) : nullptr;
	if (AnimInstance)
	{
		AnimInstance->InitializeAbilitySystem(AbilitySystemComponent);
	}

	// Inventory component initialization
	if (Inventory)
	{
		Inventory->SetPlayerId();
	}

	
	// Blueprint function event
	BP_OnInitAbilityActorInfo();
	
	UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2} OnInitAbilityActorInfo", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
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


USaveComponent* ACharacterBase::GetSaveComponent() const
{
	return SaveComponent;
}


UCombatComponent* ACharacterBase::GetCombatComponent() const
{
	return CombatComponent;
}


UInventoryComponent* ACharacterBase::GetInventoryComponent() const
{
	return Inventory;
}


FM_CharacterMontages& ACharacterBase::GetCharacterMontages()
{
	return Montages;
}


UAnimMontage* ACharacterBase::GetRollMontage() const
{
	return Montages.RollMontage;
}


UAnimMontage* ACharacterBase::GetHitReactMontage() const
{
	return Montages.HitReactMontage;
}


void ACharacterBase::SetCharacterMontages()
{
	if (!CharacterMontageTable || CharacterMontageId.IsNone())
	{
		UE_LOGFMT(LogTemp, Error, "{0}::{1}() {2} Failed to retrieve montages because the id or montage db is null!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	
	const FString RowContext(TEXT("Character Montage Context"));
	if (const F_Table_CharacterMontages* Data = CharacterMontageTable->FindRow<F_Table_CharacterMontages>(CharacterMontageId, RowContext))
	{
		Montages = Data->Montages;
	}
}

FName ACharacterBase::GetHitReactSection(EHitDirection HitDirection, EHitStun HitStun) const
{
	FName MontageSection = FName();
	if (HitStun == EHitStun::VeryShort) MontageSection = Montage_Section_HitStun_VS;
	if (HitStun == EHitStun::Short) MontageSection = Montage_Section_HitStun_V;
	if (HitStun == EHitStun::Medium) MontageSection = Montage_Section_HitStun_M;
	if (HitStun == EHitStun::Long) MontageSection = Montage_Section_HitStun_L;
	if (HitStun == EHitStun::FacePlant) MontageSection = Montage_Section_HitStun_FP;

	if (!MontageSection.IsNone())
	{
		if (HitDirection == EHitDirection::Back) MontageSection = FName(MontageSection.ToString().Append(Montage_Section_HitReact_Back.ToString()));
		if (HitDirection == EHitDirection::Front) MontageSection = FName(MontageSection.ToString().Append(Montage_Section_HitReact_Front.ToString()));
		if (HitDirection == EHitDirection::Left) MontageSection = FName(MontageSection.ToString().Append(Montage_Section_HitReact_Front.ToString()));
		if (HitDirection == EHitDirection::Right) MontageSection = FName(MontageSection.ToString().Append(Montage_Section_HitReact_Front.ToString()));
	}
	
	return MontageSection;
}


void ACharacterBase::NetMulticast_PlayMontage_Implementation(UAnimMontage* Montage, FName StartSection, float PlayRate)
{
	PlayAnimMontage(Montage, PlayRate, StartSection);
}


void ACharacterBase::Client_PlayMontage_Implementation(UAnimMontage* Montage, FName StartSection, float PlayRate)
{
	PlayAnimMontage(Montage, PlayRate, StartSection);
}


FVector ACharacterBase::GetCameraLocation() const
{
	return GetActorLocation();
}

bool ACharacterBase::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}


UAdvancedMovementComponent* ACharacterBase::GetAdvancedMovementComp() const
{
	return GetMovementComp<UAdvancedMovementComponent>();
}
#pragma endregion

