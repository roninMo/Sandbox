// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SaveInformation.generated.h"

enum class ESaveType : uint8;
class USaveLogic;


/**
 * 
 */
USTRUCT(Blueprintable, BlueprintType)
struct F_SaveLogicConfiguration
{
	GENERATED_USTRUCT_BODY()

	virtual ~F_SaveLogicConfiguration() = default;
	F_SaveLogicConfiguration() = default;

	// TODO: Figure out how C++ let's you overload functions -> and prevent circular dependencies
	// explicit F_SaveLogicConfiguration(
	// 	const FGuid& Id = FGuid(),
	// 	const FName& Name = FName(""),
	// 	const bool bAutoSave = false,
	// 	const TSubclassOf<USaveLogic> SaveLogicClass = nullptr
	// ) :
	// 	Id(Id),
	// 	Name(Name),
	// 	bAutoSave(bAutoSave),
	// 	SaveState(),
	// 	SaveLogicClass(SaveLogicClass)
	// {}

	/** Just checks if the configuration has been created */
	virtual bool IsValid() const
	{
		return this->Id.IsValid() && !this->DisplayName.IsNone();
	}
	
public:
	/** The id for the specific version of save logic */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGuid Id;

	/** The display name of the save logic. I.E. (CharacterInventory_SaveLogic, EnemyInventory_SaveLogic, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName DisplayName;

	/** Whether the component should save this information during EndPlay() */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSaveDuringEndPlay = true;

	/** What this class is saving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ESaveType SaveType;
	
	/** Whether the save information should be saved during autosaving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bAutoSave = true;

	/** A reference to the save logic class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<USaveLogic> SaveLogicClass;
};




/**
 * A data table containing a list of save logic classes for certain save states categorized by the name. The Entries should use the SaveType enum name for reference
 */
USTRUCT(BlueprintType)
struct F_SaveLogicDatabase : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** The id to save state class map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, F_SaveLogicConfiguration> SaveStateClasses;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};


