// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ESaveType.generated.h"


/**
 *	The different state information that is saved by the player
 */
UENUM(BlueprintType)
enum class ESaveType : uint8
{
	/** The player's health, stamina, experience, and mana save logic */
	Attributes			UMETA(DisplayName = "Attributes"),
	
	/** Combat state / armor and weapons equipped / etc. */
	Combat				UMETA(DisplayName = "Combat"),
	
	/** The player's current camera settings */
	CameraSettings		UMETA(DisplayName = "CameraSettings"),
	
	/** The current inventory state */
	Inventory			UMETA(DisplayName = "Inventory"),
	
	/** The configuration for the steam/console subsystem's player account */
	Settings			UMETA(DisplayName = "Settings"),

	/** (Enemies, Level, etc.) -> routes to the level blueprint for saving specific state of enemies state and spawning during the game */
	World				UMETA(DisplayName = "World"),

	MAX					UMETA(DisplayName = "MAX")
};




/**
 *	What kind of actor is the save information referencing?
 */
UENUM(BlueprintType)
enum class ESaveIdType : uint8
{
	/** The save id reference hasn't been created yet! */
	None				UMETA(DisplayName = "None"),

	/** An actor that isn't specific to the level and was spawned in the world. Their id is the item / inventory id reference */
	SpawnedActor		UMETA(DisplayName = "Spawned Actor"),
	
	/** A player's save information. Their id is their steam / console account id or their platform id */
	Player				UMETA(DisplayName = "Player"),
	
	/** A level actor that's spawned initially in the world, where the GetName() reference is somewhat safe to use as an id */
	LevelActor			UMETA(DisplayName = "Level Actor"),
	
	MAX					UMETA(DisplayName = "MAX")
};
