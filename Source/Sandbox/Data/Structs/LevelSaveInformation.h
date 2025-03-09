// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSaveInformation.generated.h"


// LevelSave_Actor
// LevelSave_Collectable
// LevelSave_Events
// LevelSave_Quests


enum class ESaveIdType : uint8;


/**
 * Config for what to save on the actor
 */
USTRUCT(Blueprintable, BlueprintType)
struct F_SaveActorConfig
{
	GENERATED_USTRUCT_BODY()
	
	virtual ~F_SaveActorConfig() = default;
	F_SaveActorConfig() = default;
	
	/** Whether to save the character's attributes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSaveAttributes;
	
	/** Whether to save the character's inventory data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSaveInventory;
	
	/** Whether to save the character's combat information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSaveCombat;
};


/**
 * Save information pertaining to an actor's state specific to a level. Subclassed versions cover the actor's saved information
 */
USTRUCT(BlueprintType)
struct F_LevelSaveInformation // TODO: add subclass logic
{
	GENERATED_USTRUCT_BODY()
		F_LevelSaveInformation(
			const FString& Id = FString(),
			const FVector& Location = FVector(),
			const FRotator& Rotation = FRotator(),
			const TSubclassOf<AActor> Class = AActor::StaticClass(),
			const TWeakObjectPtr<AActor> Actor = nullptr
		) :
		Id(Id),
		Location(Location),
		Rotation(Rotation),
		Class(Class),
		Actor(Actor)
	{}

	virtual ~F_LevelSaveInformation() {}

	/**
	 * The Id that's used to retrieve the actor's save information 
	 * 
	 * This is going to generic for handling multiple use cases for different scenarios, and might be adjusted later because this is kind of hacky
	 *	- Players:									The character's subsystem account / platform id
	 *	- Items Placed in Level:					The name of the object based on the level's construction
	 *	- Items Spawned in Level during Play:		The inventory's id. Uses the class reference to construct and spawn the item once the game begins
	 *	
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;

	/** The location of the actor that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Location;

	/** The rotation of the actor that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rotation;

	/** A reference to the class of the object spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> Class;

	/** A stored weak reference to the actor spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TWeakObjectPtr<AActor> Actor;
	

public:
	/** Convenience function to access the id without creating another value */
	virtual FString GetId() const
	{
		return this->Id;
	}
	
	/** Is this a valid actor? */
	virtual bool IsValid() const
	{
		return !this->Id.IsEmpty();
	}
};




/**
 * Save information pertaining to an actor's state specific to a level, and configuration for saving it's own information
 */
USTRUCT(BlueprintType)
struct F_LevelSaveInformation_Actor
{
	GENERATED_USTRUCT_BODY()
		F_LevelSaveInformation_Actor(
			const FString& Id = FString(),
			const FVector& Location = FVector(),
			const FRotator& Rotation = FRotator(),
			const F_SaveActorConfig& Config = F_SaveActorConfig(),
			const TSubclassOf<AActor> Class = AActor::StaticClass(),
			const TWeakObjectPtr<AActor> Actor = nullptr
		) :
		Id(Id),
		SaveType(),
		Location(Location),
		Rotation(Rotation),
		Config(Config),
		Class(Class),
		Actor(Actor)
	{
	}

	virtual ~F_LevelSaveInformation_Actor() {}

	/**
	 * The Id that's used to retrieve the actor's save information 
	 * 
	 * This is going to generic for handling multiple use cases for different scenarios, and might be adjusted later because this is kind of hacky
	 *	- Players:									The character's subsystem account / platform id
	 *	- Items Placed in Level:					The name of the object based on the level's construction
	 *	- Items Spawned in Level during Play:		The inventory's id. Uses the class reference to construct and spawn the item once the game begins
	 *	- TODO: We need additional naming conventions for objects that are spawned for custom levels (Classify it with tables that we use for storing things)
	 *	
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;

	/** How the save id was constructed. This is based on whether it's a player, level actor, or was spawned in the level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ESaveIdType SaveType;

	/** The location of the actor that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Location;

	/** The rotation of the actor that's spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rotation;

	/** The save config for the actor's component. Used when saving level information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_SaveActorConfig Config;

	/** A reference to the class of the object spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> Class;

	/** A stored weak reference to the actor spawned in the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TWeakObjectPtr<AActor> Actor;
	

public:
	/** Convenience function to access the id without creating another value */
	virtual FString GetId() const
	{
		return this->Id;
	}

	/** Is this a valid item? */
	virtual bool IsValid() const
	{
		return !this->Id.IsEmpty();
	}
	
	/** Is this valid information for spawning in the level? */
	virtual bool IsValidForSpawning() const
	{
		return this->Class && this->Location != FVector::ZeroVector && this->Rotation != FRotator::ZeroRotator;
	}

	/** Retrieves whether we should save the attributes */
	virtual bool ShouldSaveAttributes() const
	{
		return this->Config.bSaveAttributes;
	}
	
	/** Retrieves whether we should save the combat information  */
	virtual bool ShouldSaveCombatInformation() const
	{
		return this->Config.bSaveCombat;
	}

	/** Retrieves whether we should save the inventory  */
	virtual bool ShouldSaveInventory() const
	{
		return this->Config.bSaveInventory;
	}
	
	/** Convenience function to update the actor's save config */
	virtual void UpdateConfig(bool bAttributes = true, bool bInventory = true, bool bCombat = true)
	{
		this->Config.bSaveAttributes = bAttributes;
		this->Config.bSaveInventory = bInventory;
		this->Config.bSaveCombat = bCombat;
	}
	
	/** Print the actor's level save information */
	virtual FString Print() const
	{
		FString Result = FString::Printf(TEXT("Type: %s, Id: %s"), *UEnum::GetValueAsString(this->SaveType), *Id); // TODO: SaveIdType dependency fix potential error
		if (Class) Result.Append(FString::Printf(TEXT(", Class: '%s'"), *Class->GetName()));
		Result.Append(FString::Printf(TEXT("Location: '%s', Rotation: %s,"), *Location.ToString(), *Rotation.ToString()));
		return Result;
	}
};
