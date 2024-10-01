// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/World/Props/WorldItem.h"

#include "Sandbox/Data/Enums/CollisionChannels.h"

AWorldItem::AWorldItem()
{
	WorldItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	SetRootComponent(WorldItem);
	WorldItem->SetRenderCustomDepth(true);

	WorldItem->SetCollisionObjectType(ECC_Item);
	WorldItem->SetGenerateOverlapEvents(true);
	WorldItem->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WorldItem->SetCollisionResponseToAllChannels(ECR_Block);

	// Interact with the character, block the world, and ignore everything else...
	WorldItem->SetCollisionResponseToChannel(ECC_CharacterComponents, ECR_Overlap);

	WorldItem->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_Armament, ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_AOE, ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
}


void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
}


void AWorldItem::WithinPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
	
}


void AWorldItem::OutsideOfPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
	
}


void AWorldItem::WithinPlayerConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
	
}


void AWorldItem::OutsideOfConePeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
	
}
