// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/World/Props/Items/Item.h"

#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(ItemLog);


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
	Item.ItemType = EItemType::Inv_None;
	Item.Image = nullptr;

	Item.ActualClass = nullptr;
	Item.WorldClass = nullptr;
}


void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AItem, Item, COND_Custom, REPNOTIFY_OnChanged);
}


void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Handle this during spawn
	// if (ItemInformationTable && !Item.IsValid())
	// {
	// 	RetrieveItemFromDataTable(TableId, Item);
	// }
}


void AItem::OnRep_Item()
{
	// UE_LOGFMT(LogTemp, Log, "{0}::{1}() {2} replicated information to the client!",
	// 	*UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
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


void AItem::PrintItemInformation()
{
	UE_LOGFMT(ItemLog, Log, "Print Item Information of {0}({1}) ->  {1}({2}): ItemType: {4}, SortOrder: {5}",
		*StaticClass()->GetName(),
		GetOwner()->HasAuthority() ? *FString("Server") : *FString("Client"),
		*Item.Id.ToString(), *Item.DisplayName,
		*UEnum::GetValueAsString(GetItemType()), Item.SortOrder
	);
}
#pragma endregion 

