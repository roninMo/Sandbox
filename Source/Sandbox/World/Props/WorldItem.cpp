// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/World/Props/WorldItem.h"

#include "Sandbox/Data/Enums/CollisionChannels.h"

AWorldItem::AWorldItem()
{
	WorldItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	SetRootComponent(WorldItem);
	WorldItem->SetRenderCustomDepth(true);

	WorldItem->SetCollisionObjectType(ECC_Items);
	WorldItem->SetGenerateOverlapEvents(true);
	WorldItem->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WorldItem->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Interact with the character, block the world, and ignore everything else...
	WorldItem->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WorldItem->SetCollisionResponseToChannel(ECC_CharacterComponents, ECR_Overlap);
	WorldItem->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	WorldItem->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	WorldItem->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	WorldItem->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	WorldItem->SetCollisionResponseToChannel(ECC_Items, ECR_Block);
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
