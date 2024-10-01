// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/Weapons/Melee/MeleeArmament.h"

#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Data/Enums/CollisionChannels.h"
#include "Sandbox/World/Props/CharacterAttachment.h"

AMeleeArmament::AMeleeArmament()
{
	ArmamentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmamentMesh"));
	SetRootComponent(ArmamentMesh);
	ArmamentMesh->SetGenerateOverlapEvents(false);
	ArmamentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmamentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArmamentMesh->SetCollisionObjectType(ECC_Armament);

	// Collision
	ArmamentCollision = CreateDefaultSubobject<UCapsuleComponent>("Armament Collision");
	ArmamentCollision->SetupAttachment(ArmamentMesh);
	ArmamentCollision->SetGenerateOverlapEvents(false);
	ArmamentCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArmamentCollision->SetCollisionObjectType(ECC_Armament);
	
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECC_Projectile, ECollisionResponse::ECR_Overlap);
	ArmamentCollision->SetCollisionResponseToChannel(ECC_Armament, ECollisionResponse::ECR_Overlap);

	// For editing
	ArmamentCollision->SetGenerateOverlapEvents(true);
	ArmamentCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
}


void AMeleeArmament::BeginPlay()
{
	Super::BeginPlay();
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
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the combat component while constructing the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return false;
	}
	
	// Create and attach the armament to the character
	bool bCreatedSuccessfully = false;
	const USkeletalMeshSocket* HolsterSocket = Character->GetMesh()->GetSocketByName(CombatComponent->GetHolsterSocketName(ArmamentInformation.Classification, EquipSlot));
	if (HolsterClass && !HolsterSocket)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} did not find the holster socket while constructing the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
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

		if (!AttachArmamentToSocket(CombatComponent->GetSheathedSocketName(ArmamentInformation.Classification, EquipSlot)))
		{
			bCreatedSuccessfully = false;
		}
	}

	if (!bCreatedSuccessfully)
	{
		DeconstructArmament();
		UE_LOGFMT(ArmamentLog, Warning, "{0}::{1} Failed to construct the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return false;
	}

	CombatComponent->OnEquippedArmament.Broadcast(this, EquipSlot);
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
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the combat component while deconstructing the armament.",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
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
	CombatComponent->OnUnequippedArmament.Broadcast(ArmamentInformation.Id, EquipSlot);
	return true;
}


bool AMeleeArmament::IsValidArmanent()
{
	// Check if montages are valid for this specific valid
	// For melee armaments, check if the overlap component is valid
	
	return true;
}


bool AMeleeArmament::SheatheArmament()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the combat component while sheathing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return false;
	}

	if (AttachArmamentToSocket(CombatComponent->GetSheathedSocketName(ArmamentInformation.Classification, EquipSlot)))
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
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the combat component while unsheathing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
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

