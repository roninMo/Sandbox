// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SaveLogic.generated.h"

class ACharacterBase;
class USaveComponent;
enum class ESaveType : uint8;


/**
 * The save logic specific to what and how we're saving information. This varies between different actors and gamemodes on how we want to handle the logic
 */
UCLASS( Blueprintable, ClassGroup=(Saving), meta=(BlueprintSpawnableComponent, ShortTooltip="Logic for handling Saving / Loading information for different actors in the game") )
class SANDBOX_API USaveLogic : public UObject
{
	GENERATED_BODY()

protected:
	/** The id for the specific version of save logic */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGuid Id;

	/** The display name of the save logic. I.E. (CharacterInventory_SaveLogic, EnemyInventory_SaveLogic, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName DisplayName;

	/** What this class is saving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ESaveType SaveType;

	/** Whether we should save the information during EndPlay() */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSaveDuringEndPlay;

	/** Whether the save information should be saved during autosaving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bAutoSave;
	
	
public:
	USaveLogic();

	/**
	 * Handles saving the data specific to the owning actor. \n\n
	 * Subclass this logic for saving information specific to a npc, character, and their varying game modes etc.
	 *
	 * @returns	True if it successfully saved the information for the npc / player
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") bool SaveData();
	virtual bool SaveData_Implementation();

	/**
	 * Returns whether it's valid to save specific information for a character. This allows for saving under certain conditions, and preventing saving information before it's been replicated
	 *
	 * @returns	True if it's okay to save the information
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") bool IsValidToSave();
	virtual bool IsValidToSave_Implementation();

	/**
	 * Handles loading the data specific to the owning actor. \n\n
	 * Subclass this logic for saving information specific to a npc, character, and their varying game modes etc.
	 *
	 * @returns	True if it successfully loaded the information for the npc / player
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading") bool LoadData();
	virtual bool LoadData_Implementation();


	

//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/** Retrieves a reference to the SaveComponent */
	virtual bool GetSaveComponent(USaveComponent*& OutSaveComponent);

	/** Retrieves a reference to the Save Component and the Character */
	virtual bool GetCharacterAndSaveComponent(USaveComponent*& OutSaveComponent, ACharacterBase*& OutCharacter);

	/**
	 * Called during saving to determine the load flags to save with the object.
	 * If false, this object will be discarded on clients
	 *
	 * @return	true if this object should be loaded on clients
	 */
	virtual bool NeedsLoadForClient() const override;
	
	/** Retrieves the id of this SaveLogic class */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading|Utility") FGuid GetId() const;
	virtual FGuid GetId_Implementation() const;

	/** Retrieves the name */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading|Utility") FName GetDisplayName() const;
	virtual FName GetDisplayName_Implementation() const;

	/** Whether this information should be saved while auto saving */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading|Utility")bool IsValidAutoSave() const;
	virtual bool IsValidAutoSave_Implementation() const;

	/** Retrieves a reference to the type of information this class saves */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saving and Loading|Utility") ESaveType GetSaveType() const;
	virtual ESaveType GetSaveType_Implementation() const;
	
	
};
