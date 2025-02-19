// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sandbox/Data/Enums/ESaveType.h"
#include "Sandbox/Data/Structs/SaveInformation.h"
#include "SaveComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(SaveComponentLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSaveData, const ESaveType, Saving);

class USaveLogic;
enum class ESaveType : uint8;
enum class ESaveState : uint8;




/**
 * 
 */
UCLASS( Blueprintable, ClassGroup=(Saving), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API USaveComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** Whether to use the save configurations. Used for debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") bool bUseSaveInformation;
	
	/** The save configuration for the actor we're attached to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") TMap<ESaveType, F_SaveLogicConfiguration> SaveConfigurations;

	/** The current logic for saving the actor's information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") TMap<ESaveType, TObjectPtr<USaveLogic>> SaveLogicComponents;
	
	
public:	
	USaveComponent(const FObjectInitializer& ObjectInitializer);

	
protected:
	/**
	 * Ends gameplay for this component.
	 * Called from AActor::EndPlay only if bHasBegunPlay is true
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
	/**
	 * Begins Play for the component. 
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 * Actor BeginPlay normally happens right after PostInitializeComponents but can be delayed for networked or child actors.
	 * Requires component to be registered and initialized.
	 */
	virtual void BeginPlay() override;

	/**
	 * Handles the logic for autosaving information \n\n
	 * Any of the save information classes that are marked for auto saving will be saved here.
	 * 
	 * @note There still needs to be a duration between autosaving, not to be mistaken with events that automatically cause the game to save
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual void AutoSaveLogic();
	
	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 * Only executes if the component is registered, and also PrimaryComponentTick.bCanEverTick must be set to true.
	 *	
	 * @param DeltaTime - The time since the last tick.
	 * @param TickType - The kind of tick this is, for example, are we paused, or 'simulating' in the editor
	 * @param ThisTickFunction - Internal tick function struct that caused this to run
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
//----------------------------------------------------------------------------------//
// Saving																			//
//----------------------------------------------------------------------------------//
public:
	/** Delegate event for when information begins saving */
	UPROPERTY(BlueprintAssignable) FSaveData OnSavingInformation;
	
	/** Delegate event for when information has been saved */
	UPROPERTY(BlueprintAssignable) FSaveData OnSavedInformation;

	/**
	 * Initializes the save logic for saving the actors information during play. \n\n
	 * Retrieves and initializes each of the classes for saving information on the server once the player has been spawned
	 * 
	 * @returns		true if all of the specified classes have been successfully created.
	 * @note		Called when the match has began for replication during BeginPlay. Can also be called on client join with Online subsystems, and once the information has been replicated to AbilitySystems OnInitActorInfo()
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Initialization") virtual bool InitializeSaveLogic();

	/**
	 * Handles the logic for tearing down the save states before deleting this component
	 *
	 * @note TODO: Check that EndPlay works when this component's actor is destroyed 
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Initialization") virtual void DeleteSaveStates();

	/**
	 * Saves the actor's information using it's save configuration to capture relevant data, storing it specific to each actor's required way of saving information
	 *
	 * @param Saving					The type of information that's being saved		"the vibes you're killing todays"
	 * @returns							True if the information has been successfully saved 
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual bool SaveData(const ESaveType Saving);

	/**
	 * Blueprint function for adding save functionality for specific events when saving information.
	 *
	 * @param SaveType					The type of information that's being saved
	 * @param bSuccessfullySaved		True if the save function successfully saved the information
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Saving and Loading", DisplayName = "On Save Data") void BP_SaveData(const ESaveType SaveType, bool bSuccessfullySaved);
	
	/**
	 * Returns whether it's valid to save specific information for a character. This prevents saving logic before it's been replicated
	 *
	 * @param InformationType			The type of information that we want to save
	 * @returns							True if it's okay to save the information
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual bool IsValidToSave(const ESaveType InformationType);



	
//----------------------------------------------------------------------------------//
// Loading																			//
//----------------------------------------------------------------------------------//
	/** Loads the player's saved information on the client once the client has joined the game and the player has spawned */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual void LoadPlayerInformation();

	/** Blueprint function to load the player's saved information on the client once the client has joined the game and the player has spawned */
	UFUNCTION(BlueprintImplementableEvent, Category = "Saving and Loading", DisplayName = "On Save Data") void BP_LoadPlayerInformation();



	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
protected:
	/** Handle for preventing certain information from being saved during gameplay. Useful for in game debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving|Debugging") TMap<ESaveType, bool> PreventLoading;

	/** Whether this component is currently preventing loading specific information for debugging purposes */
	virtual bool PreventingLoadingFor(const ESaveType SaveType) const;

	
public:
	/**
	 * Retrieves the save's reference for specific save states using the save state and actor's information. This should vary between players and enemies spawned in the world \n\n 
	 * Default logic just returns FName. Subclassed versions should return the actor's unique network or npc id followed by a reference to the type of information they're saving \n
	 *		Something like this:
	 *			- Enemies: SpawnedEnemy_0_Inventory
	 *			- Players: Player_0_Inventory
	 *
	 * @param Saving					The type of information we're saving
	 * @returns							The id used for saving information to a specific slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual FString GetSaveSlotIdReference(const ESaveType Saving) const;

	/** Returns whether the save component handles saving information for a specific save type */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual bool HandlesSaving(const ESaveType SaveType) const;

protected:
	/** Retrieves the player's save slot id specific to their steam/console platform account for saving purposes */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual FString GetPlayerNetId() const;

	/** Returns the save section for the player's save information */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual FString GetSaveTypeName(const ESaveType SaveType) const;
	
		
};
