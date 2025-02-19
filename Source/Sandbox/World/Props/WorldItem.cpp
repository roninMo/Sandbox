// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/World/Props/WorldItem.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
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
	OnSpawnedInWorld();
}


void AWorldItem::OnSpawnedInWorld()
{
	// Retrieve the item's information if this was an item that was placed in the level
	if (!Item.Id.IsValid())
	{
		RetrieveItemFromDataTable(TableId, Item);
		CreateIdIfNull();
	}
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


F_LevelSaveInformation_Actor AWorldItem::SaveToLevel_Implementation()
{
	F_LevelSaveInformation_Actor SavedInformation;
	SavedInformation.Id = Item.Id;
	SavedInformation.Location = GetActorLocation();
	SavedInformation.Rotation = GetActorRotation();
	
	OnSaveToLevel(SavedInformation);
	return SavedInformation;
}


bool AWorldItem::LoadFromLevel_Implementation(const F_LevelSaveInformation_Actor& PreviousSave)
{
	bool bSuccessfullyLoaded = true;
	if (!PreviousSave.Location.IsNearlyZero())
	{
		bSuccessfullyLoaded = TeleportTo(PreviousSave.Location, PreviousSave.Rotation);
	}
	
	// Handle any other state that's specific to the character's level specific save state

	OnLoadFromLevel(PreviousSave, bSuccessfullyLoaded);
	return bSuccessfullyLoaded;
}


FGuid AWorldItem::GetActorLevelId_Implementation() const
{
	return Item.Id;
}
