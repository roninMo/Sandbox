// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/World/Props/Items/Item.h"

#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItem::AItem()
{
	// Network values
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	// Item information
	Item.Id = FGuid();
	Item.SortOrder = -1;
	Item.ItemName = NAME_None;
	Item.DisplayName = "Rock";

	Item.Description = "This world item's values haven't been set yet!";
	Item.InteractText = "Press E to pickup";
	Item.ItemType = EItemType::Inv_None;
	Item.Image = nullptr;

	Item.ActualClass = nullptr;
	Item.WorldClass = nullptr;
}


void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AItem, Item, COND_InitialOrOwner);
}


void AItem::InitializeItemGlobals()
{
	// Ex item globals function.
	if (!Item.GlobalInformation) return;
	F_Item GlobalItem;
	
	// const UData_DefaultItemInformation* Information = Cast<UData_CharacterGlobals>(Data);
	// ItemHighlightColor = Information->ItemHighlightColor;
}


void AItem::BeginPlay()
{
	Super::BeginPlay();
	InitializeItemGlobals();

	// Handle this during spawn
	// if (ItemInformationTable && !Item.IsValid())
	// {
	// 	RetrieveItemFromDataTable(TableId, Item);
	// }
	
	// CreateIdIfNull();
}


void AItem::CreateIdIfNull()
{
	if (Item.Id == FGuid())
	{
		Item.Id = FGuid().NewGuid();
	}
}


void AItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


#pragma region Inventory item functions
F_Item AItem::GetItem_Implementation() const				{ return Item; }
const EItemType AItem::GetItemType_Implementation() const	{ return Item.ItemType; }
const FGuid AItem::GetId_Implementation() const				{ return Item.Id; }
const FName AItem::GetItemName_Implementation() const		{ return Item.ItemName; }
void AItem::SetItem_Implementation(const F_Item Data)		{ Item = Data; }
void AItem::SetId_Implementation(const FGuid& Id)			{ Item.Id = Id; }
bool AItem::IsSafeToAdjustItem_Implementation() const { return PendingPlayer == nullptr; }
void AItem::SetPlayerPending_Implementation(ACharacter* Player) { PendingPlayer = Player; }
ACharacter* AItem::GetPlayerPending_Implementation() { return PendingPlayer; }
void AItem::SetItemInformationDatabase_Implementation(UDataTable* Database) { ItemInformationTable = Database; }


bool AItem::RetrieveItemFromDataTable(const FName Id, F_Item& ItemData)
{
	if (ItemInformationTable)
	{
		const FString RowContext(TEXT("Item Information Context"));
		if (const FInventory_ItemDatabase* Data = ItemInformationTable->FindRow<FInventory_ItemDatabase>(Id, RowContext))
		{
			ItemData = Data->ItemInformation;
			return true;
		}
		
		UE_LOGFMT(InventoryLog, Error, "{0}() {1} Did not find the item {2} to create!", *FString(__FUNCTION__), *GetName(), Id);
	}

	return false;
}

#pragma endregion 

