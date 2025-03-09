// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"


/**
 *	The specific type of inventory item
 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Inv_Item		                    UMETA(DisplayName = "Item"),
	Inv_Armor		                    UMETA(DisplayName = "Armor"),
	Inv_Note							UMETA(DisplayName = "Note"),
	Inv_QuestItem		                UMETA(DisplayName = "Quest Item"),
	Inv_Weapon							UMETA(DisplayName = "Weapon"),
	Inv_Material						UMETA(DisplayName = "Material"),
	Inv_Custom							UMETA(DisplayName = "Custom"),
	Inv_None                            UMETA(DisplayName = "None"),
	
	// None								UMETA(DisplayName = "None"),
	// Prop								UMETA(DisplayName = "Prop"), // Building props 
	// FabricatedProp						UMETA(DisplayName = "Fabricated Prop"), // Buildings
	// Battlement							UMETA(DisplayName = "Battlement"), // anything weapon based that is a construct like a turret
	// Enemy								UMETA(DisplayName = "Enemy"),
	// Armor								UMETA(DisplayName = "Armor"),
	// Weapon								UMETA(DisplayName = "Weapon"),
	// Equipment							UMETA(DisplayName = "Equipment"),
	// Vehicle								UMETA(DisplayName = "Vehicle"),
	// GameMode							UMETA(DisplayName = "GameMode"), // GameMode / Spawn locations / etc.
	// FX									UMETA(DisplayName = "Prop"), // visual fx / post process volume / etc.
	Inv_MAX
};




/**
 *	The operation that's being done on the inventory(s)
 */
UENUM(BlueprintType)
enum class EInventoryOperation : uint8
{
	/* Player adding or editing an inventory without interacting with another inventory component */
	OP_IndividualInventory		            UMETA(DisplayName = "Same Inventory Edit"),
	
	/* Storing an item in a container */
	OP_InventoryToContainer				UMETA(DisplayName = "Storing Inventory in Container Edit"),

	/* Taking an item from a container and adding it to the player's inventory */
	OP_ContainerToInventory             UMETA(DisplayName = "Taking Container Inventory Edit"),

	/* Multiple inventories editing each other */
	OP_InventoryToInventory             UMETA(DisplayName = "Multiple Player Inventories Edit"),
	
	OP_None                             UMETA(DisplayName = "None")
};




/**
 *	The state of saving and loading character information. 
 */
UENUM(BlueprintType)
enum class ESaveState : uint8
{
	/** Everything is either saved or there's no need to save anything */
	ESave_None							UMETA(DisplayName = "None"),
	
	/** There's new save information, and the character state needs to be updated */
	ESave_NeedsSaving					UMETA(DisplayName = "NeedsSaving"),
	
	/** The save information is being sent to the client, and is pending completion */
	ESave_Pending						UMETA(DisplayName = "Pending"),

	/** The save information has been sent to the client and ready for use */
	ESave_SaveReady						UMETA(DisplayName = "SaveReady"),
	
	/** The information has been saved */
	ESave_Saved							UMETA(DisplayName = "Saved"),
	
	/** An error occurred while saving the player information! */
	ESave_Error                         UMETA(DisplayName = "Error"),
};
