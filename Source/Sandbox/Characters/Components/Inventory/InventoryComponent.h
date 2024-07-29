// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sandbox/Data/Enums/InventoryTypes.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "Sandbox/Data/Interfaces/Inventory/InventoryInterface.h"
#include "InventoryComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(InventoryLog, Log, All);

#define OP__IndividualInventory EInventoryOperation::OP_IndividualInventory
#define OP__InventoryToContainer EInventoryOperation::OP_InventoryToContainer
#define OP__ContainerToInventory EInventoryOperation::OP_ContainerToInventory
#define OP__InventoryToInventory EInventoryOperation::OP_InventoryToInventory


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInventoryAdditionFailureDelegate, const FGuid&, Id, const FName, DatabaseId, TScriptInterface<IInventoryItemInterface>, SpawnedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryAdditionSuccessDelegate, const F_Item&, ItemData, TScriptInterface<IInventoryItemInterface>, SpawnedItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInventoryItemTransferFailureDelegate, const FGuid&, Id, const TScriptInterface<IInventoryInterface>, OtherInventory, const bool, bFromThisInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInventoryItemTransferSuccessDelegate, const FGuid&, Id, const TScriptInterface<IInventoryInterface>, OtherInventory, const bool, bFromThisInventory);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemRemovalFailureDelegate, const FGuid&, Id, UObject*, SpawnedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemRemovalSuccessDelegate, const F_Item&, ItemData, UObject*, SpawnedItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadSaveDataInventoryDelegate, bool, bSuccessfullySavedInventory);


/**
 * An inventory system for player's for storing and retrieving different inventory items in multiplayer with error handling in a safe and efficient way that even allows for customization, and works out of the box.
 * All you need to do is add the component to the character, and store and retrieve values from it. There's also logic for saving information, just search through the function list in the blueprint.
 *
 * @remarks We use remote procedure calls so replication doesn't have to try and handle everything, and only pass everything with references when we're storing information
 */
UCLASS( Blueprintable, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

protected:
	/**** Inventory ****/ // I've divided the inventory into maps for quick retrieval, however you're able to build with arrays and other things if you want to adjust the logic */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") TMap<FGuid, F_Item> QuestItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") TMap<FGuid, F_Item> CommonItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") TMap<FGuid, F_Item> Weapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") TMap<FGuid, F_Item> Armors;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") TMap<FGuid, F_Item> Materials;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") TMap<FGuid, F_Item> Notes;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") UDataTable* ItemDatabase;
	
	/**** References and stored information ****/
	/** The client's Net Id */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") int32 NetId;

	/** The Id of the current machine of the player  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory") FString PlatformId;

	/** A reference to the character */
	UPROPERTY(BlueprintReadWrite, Category = "Inventory") TObjectPtr<ACharacter> Character;

	/**** Other ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Debugging") bool bDebugSaveInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Debugging") bool bDebugInventory_Client;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Debugging") bool bDebugInventory_Server;

	
protected:
	UInventoryComponent();
	virtual void BeginPlay() override;
	// Loading / Saving the inventory information should be handled in the player state!
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
//----------------------------------------------------------------------------------//
// Item Addition																	//
//----------------------------------------------------------------------------------//
public:
	/**
	 * Sends the information to the server to add an item to the inventory, and handles each of the different scenarios for this action. \n\n
	 * There's multiple delegate functions in response to each scenario, and if an error occurs while updating the inventory there's safeguards in place to revert the logic \n\n
	 *
	 * Either add an item by creating a unique id and using the database id, or add an item from an already spawned item in the world
	 *
	 * Order of operations is TryAddItem:
	 *		- AddItemPendingClientLogic
	 *		- Server_TryAddItem -> HandleAddItem
	 *			- Client_AddItemResponse
	 *				- HandleItemAdditionFail
	 *				- HandleItemAdditionSuccess
	 * 
	 * @param DatabaseId								The id for this item in the inventory
	 * @param InventoryItemInterface					The reference to the actor spawned in the world, if there is one (and you want it to be deleted upon completion).
	 * @param Type										The item type (used for item allocation)
	 * @returns		True if the item was found in the database and successfully added to the inventory.
	 * 
	 * @note For handling the ui, I'd add delegates on the response functions for update notifications on the player's inventory
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual bool TryAddItem_Implementation(const FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type) override;

	
protected:
	/**
	 * What should happen assuming the item is added
	 * 
	 * @note If the item isn't successfully added then @ref HandleItemAdditionFail should be called, otherwise @ref HandleItemAdditionSuccess is called
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 * */
	virtual void AddItemPendingClientLogic_Implementation(const FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type) override;
	
	/** Handles adding the item on the server, and sends the response to the client */
	UFUNCTION(Server, Reliable) virtual void Server_TryAddItem(const FGuid& Id, const FName DatabaseId, UObject* InventoryInterface, const EItemType Type);
	/** Handles the different scenarios of an AddItem operation */
	UFUNCTION(Client, Reliable) virtual void Client_AddItemResponse(const bool bSuccess, const FGuid& Id, const FName DatabaseId, UObject* InventoryInterface, const EItemType Type);
	
	/**
	 * The actual logic that handles adding the item to an inventory component
	 * 
	 * @return The id of the newly created item
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual F_Item HandleAddItem_Implementation(const FGuid& Id, const FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type) override;
	
	/**
	 * If the item was not added to the inventory
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual void HandleItemAdditionFail_Implementation(const FGuid& Id, const FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type) override;

	/** Delegate function for when an item failed to be added to the inventory. Helpful for ui elements to keep track of inventory updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Operations") FInventoryAdditionFailureDelegate OnInventoryItemAdditionFailure;
	
	/**
	 * If the item was successfully added to the inventory
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual void HandleItemAdditionSuccess_Implementation(const FGuid& Id, const FName DatabaseId, UObject* InventoryItemInterface, const EItemType Type) override;

	/** Delegate function for when an item is successfully added to the inventory. Helpful for ui elements to keep track of inventory updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Operations") FInventoryAdditionSuccessDelegate OnInventoryItemAdditionSuccess;
	
	
//----------------------------------------------------------------------------------//
// Item Transfer																	//
//----------------------------------------------------------------------------------//
public:
	/**
	 * Sends the information to the server to transfer an item from the one inventory to another, and handles each of the different scenarios for this action. \n\n
	 * There's multiple delegate functions in response to each scenario, and if an error occurs while updating the inventory there's safeguards in place to revert the logic \n\n
	 *
	 * Order of operations is TryTransferItem ->
	 *		- TransferItemPendingClientLogic
	 *		- Server_TryTransferItem -> HandleTransferItem
	 *			- Client_TransferItemResponse
	 *				- HandleTransferItemFail
	 *				- HandleTransferItemSuccess
	 *
	 * @note For handling the ui, I'd add delegates on the response functions for update notifications on the player's inventory
	 * 
	 * @param Id										The unique id of the inventory item.
	 * @param OtherInventoryInterface					The reference to the other inventory component
	 * @param Type										The item type (used for item allocation)
	 * @returns		True if the item was successfully transferred to the other inventory.
	 * 
	 * @note For handling the ui, I'd add delegates on the response functions for update notifications on the player's inventory
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual bool TryTransferItem_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, const EItemType Type) override;

	
protected:
	/**
	 * What should happen assuming the item is transferred
	 *
	 * @note If the item isn't successfully transferred then @ref HandleTransferItemAdditionFail should be called, otherwise @ref HandleTransferItemAdditionSuccess is called
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 * */
	virtual void TransferItemPendingClientLogic_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, const EItemType Type) override;
	
	/** Handles transferring the item on the server, and sends the response to the client */
	UFUNCTION(Server, Reliable) virtual void Server_TryTransferItem(const FGuid& Id, UObject* OtherInventoryInterface, const EItemType Type);
	/** Handles the different scenarios of an TransferItem operation */
	UFUNCTION(Client, Reliable) virtual void Client_TransferItemResponse(const bool bSuccess, const FGuid& Id, const FName DatabaseId, UObject* OtherInventoryInterface, const EItemType Type, const bool bFromThisInventory);
	
	/**
	 * The actual logic that handles transferring the item to the other inventory component
	 * 
	 * @return True if it was able to transfer the item
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual bool HandleTransferItem_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, const EItemType Type, bool& bFromThisInventory) override;
	
	/**
	 * Utility function for handling updating the inventory information on other clients during an item transfer
	 * 
	 * @param Id										The unique id of the inventory item.
	 * @param DatabaseId								The id for this item in the inventory
	 * @param Type										The item type (used for item allocation)
	 * @param bAddItem									Whether the item is being added or removed from the inventory
	 * 
	 * @remarks Client logic doesn't have any problems when invoking the handle logic on the client response functions, it's just problematic when there's multiple Clients (During a transfer)
	 */
	UFUNCTION(Client, Reliable) virtual void Client_HandleTransferItemForOtherInventory(const FGuid& Id, const FName DatabaseId, const EItemType Type, const bool bAddItem);
	
	/**
	 * If the item was not transferred to the other inventory
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual void HandleTransferItemFail_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, bool bFromThisInventory) override;

	/** Delegate function for when an item failed to be added to the inventory. Helpful for ui elements to keep track of inventory updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Operations") FInventoryItemTransferFailureDelegate OnInventoryItemTransferFailure;
	
	/**
	 * If the item was successfully transferred to the other inventory
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual void HandleTransferItemSuccess_Implementation(const FGuid& Id, UObject* OtherInventoryInterface, bool bFromThisInventory) override;

	/** Delegate function for when an item is successfully added to the inventory. Helpful for ui elements to keep track of inventory updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Operations") FInventoryItemTransferSuccessDelegate OnInventoryItemTransferSuccess;


public:
	/**
	 * Utility function for handling updating the inventory information on other clients during an item transfer.
	 * This should handle the appropriate remote procedure logic so that the other inventory's client information is updated
	 * 
	 * @param Id										The unique id of the inventory item.
	 * @param DatabaseId								The id for this item in the inventory
	 * @param Type										The item type (used for item allocation)
	 * @param bAddItem									Whether the item is being added or removed from the inventory
	 * 
	 * @remarks Client logic doesn't have any problems when invoking the handle logic on the client response functions, it's just problematic when there's multiple Clients (During a transfer)
	 */
	virtual void HandleTransferItemForOtherInventoryClientLogic(const FGuid& Id, const FName DatabaseId, const EItemType Type, const bool bAddItem) override;
	
	
//----------------------------------------------------------------------------------//
// Remove Item																		//
//----------------------------------------------------------------------------------//
public:
	/**
	 * Sends the information to the server to remove an item from the inventory, and handles each of the different scenarios for this action (including whether to drop the item). \n\n
	 * There's multiple delegate functions in response to each scenario, and if an error occurs while updating the inventory there's safeguards in place to revert the logic \n\n
	 *
	 * Order of operations is TryRemoveItem ->
	 *		- RemoveItemPendingClientLogic
	 *		- Server_TryRemoveItem -> HandleRemoveItem
	 *			- Client_RemoveItemResponse
	 *				- HandleRemoveItemFail
	 *				- HandleRemoveItemSuccess
	 * 
	 * @param Id					The unique id of the inventory item.
	 * @param Type					The item type (used for item allocation)
	 * @param bDropItem				Whether the item should be spawned in the world when removed
	 * @returns		True if the item was found in the database and successfully added to the inventory.
	 * 
	 * @note For handling the ui, I'd add delegates on the response functions for update notifications on the player's inventory
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual bool TryRemoveItem_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem) override;
	
	
protected:
	/**
	 * What should happen assuming the item is removed
	 * 
	 * @note If the item isn't successfully removed then @ref HandleRemoveItemAdditionFail should be called, otherwise @ref HandleRemoveItemAdditionSuccess is called
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 * */
	virtual void RemoveItemPendingClientLogic_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem) override;
	
	/** Handles removing the item on the server, and sends the response to the client */
	UFUNCTION(Server, Reliable) void Server_TryRemoveItem(const FGuid& Id, const EItemType Type, bool bDropItem);
	/** Handles the different scenarios of an RemoveItem operation */
	UFUNCTION(Client, Reliable) void Client_RemoveItemResponse(const bool bSuccess, const FGuid& Id, const FName DatabaseId, const EItemType Type, bool bDropItem, UObject* SpawnedItem);
	
	/**
	 * The actual logic that handles removing the item from the inventory component
	 *
	 * @note If the item isn't successfully removed then @ref HandleRemoveItemAdditionFail should be called, otherwise @ref HandleRemoveItemAdditionSuccess is called
	 * @remark Blueprints do not need to handle this logic unless they want to override the logic already in place
	 * */
	virtual bool HandleRemoveItem_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem, UObject*& SpawnedItem) override;
	
	/**
	 * If the item was not removed from the inventory
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual void HandleRemoveItemFail_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem, UObject* SpawnedItem) override;

	/** Delegate function for when an item failed to be added to the inventory. Helpful for ui elements to keep track of inventory updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Operations") FInventoryItemRemovalFailureDelegate OnInventoryItemRemovalFailure;
	
	/**
	 * If the item was successfully removed from the inventory
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual void HandleRemoveItemSuccess_Implementation(const FGuid& Id, const EItemType Type, bool bDropItem, UObject* SpawnedItem) override;
	
	/** Delegate function for when an item is successfully added to the inventory. Helpful for ui elements to keep track of inventory updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Operations") FInventoryItemRemovalSuccessDelegate OnInventoryItemRemovalSuccess;
	
//----------------------------------------------------------------------------------//
// Saving																			//
//----------------------------------------------------------------------------------//
protected:
	/** The save state of the inventory. Used when the player is saving information for communication between the server and client to determine when the client has retrieved it's save information  */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "Inventory|Saving") ESaveState SaveState;

	/** The current inventory save data. This isn't updated until the server has sent all it's inventory information */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "Inventory|Saving") F_InventorySaveInformation CurrentInventorySaveData;

	/** This is a value to store the information that's sent to the client. Once everything has been sent to the client, the current inventory save data is updated with this information */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "Inventory|Saving") F_InventorySaveInformation ClientInventorySaveData;
	
	
public:
	/** Returns the player's inventory information for saving */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Saving and Loading") virtual F_InventorySaveInformation GetInventorySaveInformation();

	/**
	 * Loads the player's inventory on both the server and client from a list of saved inventory items. 
	 *
	 * @param SaveInformation			The save information object containing the player's inventory information
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Saving and Loading") virtual void LoadInventoryInformation(const F_InventorySaveInformation& SaveInformation);

	/** Delegate function for when they've loaded to the client's save information */
	UPROPERTY(BlueprintAssignable) FOnLoadSaveDataInventoryDelegate OnLoadSaveData;

	
protected:
	/** Start sending saved inventory information to the client */
	UFUNCTION(Client, Reliable) virtual void Client_BeginLoadingInventoryData();
	
	/** Sends some of the current save's inventory information to the client
	 * @note this is done in multiple calls to avoid network issues (RPC's default data limits are 64kb)
	 */
	UFUNCTION(Client, Reliable) virtual void Client_LoadSomeInventoryData(const TArray<FS_Item>& Items);

	/** Sets the current save data to the captured client information. Called once all the client information has been sent */
	UFUNCTION(Client, Reliable) virtual void Client_LoadSaveDataCompleted();

	/**
	 * Updates the inventory information with the player state's current save data. This is called in UpdateInventoryAfterRetrievingSaveInformation() during play based on the SaveState of the inventory
	 * 
	 * @param SaveInformation			The save information object containing the player's inventory information
	 * 
	 * @returns true if every inventory item is successfully added
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Saving and Loading") virtual bool UpdateInventoryInformation(const F_InventorySaveInformation& SaveInformation);
	
	/** Function for handling the save state information once a player loads the inventory information. Updates the inventory if they retrieved new save information */
	UFUNCTION(Category = "Inventory|Saving and Loading") virtual void UpdateInventoryAfterRetrievingSaveInformation();
	
	/** Create a save item from an inventory item */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Saving and Loading") virtual FS_Item CreateSavedItem(const F_Item& Item) const;

	
	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/**
	 * Returns a copy of an item from the player's inventory. This function shouldn't be called directly, and should only be called on the server.
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place (Server side logic between two inventory component interfaces)
	 */
	virtual F_Item InternalGetInventoryItem_Implementation(const FGuid& Id, EItemType InventorySectionToSearch = EItemType::Inv_None) override;
	
	/**
	 * Adds an item from the player's inventory. This function shouldn't be called directly, and should only be called on the server.
	 *
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place (Server side logic between two inventory component interfaces)
	 */
	virtual void InternalAddInventoryItem_Implementation(const F_Item& Item) override;
		
	/**
	 * Removes an item from the player's inventory. This function shouldn't be called directly, and should only be called on the server.
	 * 
	 * @remark Blueprints do not need to handle this logic unless they want to override the logic already in place
	 * 
	 * @remark these are only used for specific cases where there isn't a traditional way of editing the inventory (Server side logic between two inventory component interfaces)
	 */
	virtual void InternalRemoveInventoryItem_Implementation(const FGuid& Id, EItemType InventorySectionToSearch = EItemType::Inv_None) override;
	
	
public:
	/** Returns the Item Id from a specific item in the inventory  */
	UFUNCTION() virtual FName GetItemId(const FGuid& Id, EItemType Type, UObject* OtherInventory = nullptr);
	
	/** Returns the character's id (a combination of both the NetId and the PlatformId) */
	virtual FString GetPlayerId_Implementation() const override;

	/** Finds the NetId and Platform Id of the player, and sets those values */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utilities") virtual void SetPlayerId();
	
	
protected:
	/**
	 * Returns the inventory list specific to the item's type
	 * @returns One of the inventory lists from this component
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory") virtual TMap<FGuid, F_Item>& GetInventoryList(EItemType InventorySectionToSearch);

	/**
	 * Returns an item from one of the lists in this component.
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual bool GetItem_Implementation(F_Item& ReturnedItem, FGuid Id, EItemType InventorySectionToSearch = EItemType::Inv_None) override;
	
	/**
	 *	Returns an item from the database
	 *  @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual bool GetDataBaseItem_Implementation(FName Id, F_Item& Item) override;
	
	/**
	 * Creates the inventory item object for adding things to the inventory.
	 * If you want to subclass the inventory object, use this function
	 * 
	 * @remarks If you want to subclass the Item object, use this function. And if you create any Items, do it with this function
	 */
	virtual F_Item* CreateInventoryObject() const override;
	
	/** Access the save state on the client to know when to update the character information */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Saving and Loading") virtual ESaveState GetSaveState();
	
	/** Access the save state on the client to know when to update the character information */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Saving and Loading") virtual void SetSaveState(ESaveState State);

	/**
	 * Checks if the character is valid and if not, gets the character component and returns true
	 * @returns true whether the character is valid
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utilities") virtual bool GetCharacter();
	
	/**
	 * Spawns an inventory item in the world
	 * 
	 * @remarks Blueprints do not need to handle this logic unless they want to override the logic already in place
	 */
	virtual TScriptInterface<IInventoryItemInterface> SpawnWorldItem_Implementation(const F_Item& Item, const FTransform& Location) override;
	
	
protected:
	/** Listing inventory information -> @ref ListInventory, ListSavedCharacterInformation  */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utilities|Listing") virtual void ListInventory();
	UFUNCTION(Server, Reliable, Category = "Inventory|Utilities|Listing") virtual void Server_ListInventory(const TArray<FS_Item>& ClientItemList, bool bCalledFromServer);
	UFUNCTION(Category = "Inventory|Utilities|Listing") virtual void ListInventoryMap(const TMap<FGuid, F_Item>& Map, FString ListName);
	UFUNCTION(Category = "Inventory|Utilities|Listing") virtual void ListInventoryItem(const F_Item& Item);

	UFUNCTION(Category = "Inventory|Utilities|Listing") virtual void ListSavedItem(const FS_Item& SavedItem);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utilities|Listing") virtual void ListSavedInventory(const F_InventorySaveInformation& Data);

		
};
