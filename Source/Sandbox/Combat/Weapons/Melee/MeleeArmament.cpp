// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/Weapons/Melee/MeleeArmament.h"


#include "Sandbox/Data/Enums/CollisionChannels.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "Sandbox/World/Props/CharacterAttachment.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

AMeleeArmament::AMeleeArmament()
{
	ArmamentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmamentMesh"));
	ArmamentMesh->SetGenerateOverlapEvents(false);
	ArmamentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmamentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArmamentMesh->SetCollisionObjectType(ECC_Armaments);
	SetRootComponent(ArmamentMesh);

	// Collision
	ArmamentCollision = CreateDefaultSubobject<UCapsuleComponent>("Armament Collision");
	ArmamentCollision->SetupAttachment(ArmamentMesh);
	ArmamentCollision->SetGenerateOverlapEvents(false);
	ArmamentCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArmamentCollision->SetCollisionObjectType(ECC_Armaments);
	
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECC_Projectiles, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECC_Armaments, ECollisionResponse::ECR_Overlap);

	// For editing
	ArmamentCollision->SetGenerateOverlapEvents(true);
	ArmamentCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	LeftHandSheatheSocket = Socket_Sheathe_1h_blade_l;
	RightHandSheatheSocket = Socket_Sheathe_1h_blade_r;
	LeftHandHolsterSocket = Socket_Holster_1h_blade_l;
	RightHandHolsterSocket = Socket_Holster_1h_blade_r;
}

void AMeleeArmament::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMeleeArmament, Holster, COND_InitialOnly);
}


void AMeleeArmament::BeginPlay()
{
	Super::BeginPlay();
	Hitboxes.AddUnique(ArmamentCollision);
}

void AMeleeArmament::BeginDestroy()
{
	// // Delete the holster
	// Holster->Destroy();
	//
	// // If there's a collision component, remove it's delegate bindings
	// if (ArmamentCollision)
	// {
	// 	ArmamentCollision->OnComponentBeginOverlap.RemoveAll(this);
	// 	ArmamentCollision->OnComponentEndOverlap.RemoveAll(this);
	// }
	
	Super::BeginDestroy();
}


bool AMeleeArmament::ConstructArmament()
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Super::ConstructArmament())
	{
		return false;
	}

	UCombatComponent* CombatComponent = GetCombatComponent(Character);
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while constructing the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}
	
	// Create and attach the armament to the character
	bool bCreatedSuccessfully = false;
	const USkeletalMeshSocket* HolsterSocket = Character->GetMesh()->GetSocketByName(GetHolsterSocketName());
	if (HolsterClass && !HolsterSocket)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} did not find the holster socket while constructing the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	// Spawn and attach the holster
	if (Character->HasAuthority())
	{
		if (!HolsterClass) bCreatedSuccessfully = true;
		else
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParameters.Owner = GetOwner();
			const FTransform SpawnLocation = HolsterSocket->GetSocketTransform(Character->GetMesh());

			Holster = Cast<ACharacterAttachment>(GetWorld()->SpawnActor(HolsterClass, &SpawnLocation, SpawnParameters));
			if (Holster) bCreatedSuccessfully = HolsterSocket->AttachActor(Holster, Character->GetMesh());
		}

		if (!AttachArmamentToSocket(GetSheathedSocketName()))
		{
			bCreatedSuccessfully = false;
		}
	}

	if (!bCreatedSuccessfully)
	{
		DeconstructArmament();
		UE_LOGFMT(ArmamentLog, Warning, "{0}::{1}() {2} Failed to construct the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	Hitboxes.AddUnique(ArmamentCollision);
	return true;
}


bool AMeleeArmament::DeconstructArmament()
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Super::DeconstructArmament()) return false;

	UCombatComponent* CombatComponent = GetCombatComponent(Character);
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while deconstructing the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}
	
	// Remove the hitbox references
	Hitboxes.Empty();

	// Delete the holster
	if (Holster)
	{
		Holster->Destroy();
	}

	// Remove any delegate bindings
	ArmamentCollision->OnComponentBeginOverlap.RemoveAll(this);
	return true;
}


bool AMeleeArmament::IsValidArmanent()
{
	if (!Super::IsValidArmanent())
	{
		return false;
	}

	// For melee armaments, check if the overlap component is valid
	// if (GetArmamentHitboxes().IsEmpty())
	// {
	// 	UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2}'s {3} hitboxes are invalid!",
	// 		*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), *Item.DisplayName);
	// 	return false;
	// }

	// Check if montages are valid for this specific valid
	if (MeleeMontages_OneHand.IsEmpty() && MeleeMontages_TwoHand.IsEmpty() && MeleeMontages_DualWield.IsEmpty() && Montages.IsEmpty())
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2}'s {3} montages are invalid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), *Item.DisplayName);
		return false;
	}
	
	return true;
}


bool AMeleeArmament::SheatheArmament()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while sheathing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	if (AttachArmamentToSocket(GetSheathedSocketName()))
	{
		ArmamentCollision->SetGenerateOverlapEvents(false);
		ArmamentCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return true;
	}
	
	return false;
}


bool AMeleeArmament::UnsheatheArmament()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while unsheathing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	if (AttachArmamentToSocket(CombatComponent->GetEquippedSocketName(ArmamentInformation.Classification, EquipSlot)))
	{
		ArmamentCollision->SetGenerateOverlapEvents(true);
		ArmamentCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		return true;
	}

	return false;
}

void AMeleeArmament::SetOwnerNoSee(const bool bHide)
{
	Super::SetOwnerNoSee(bHide);

	if (Holster && Holster->Object)
	{
		Holster->Object->SetOwnerNoSee(bHide);
	}
}


TArray<UPrimitiveComponent*> AMeleeArmament::GetArmamentHitboxes() const
{
	return Hitboxes;
}

void AMeleeArmament::PrintItemInformation()
{
	Super::PrintItemInformation();

	// TODO: add weapon leveling / saved information
}

