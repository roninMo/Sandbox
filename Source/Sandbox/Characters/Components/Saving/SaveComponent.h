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
	/** The player's current Net Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") int32 NetId = 0;

	/** The player's platform Id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") FString PlatformId;
	
	/** The save slot index the player is currently using */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") int32 SaveSlotIndex;

	/** The current logic for saving the actor's information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") TMap<ESaveType, TObjectPtr<USaveLogic>> SaveLogicComponents;
	
	/** The save configuration for the actor we're attached to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") TMap<ESaveType, F_SaveLogicConfiguration> SaveConfigurations;

	/** Whether to use the save configurations. Used for debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") bool bUseSaveInformation;

	/** Whether we save on EndPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") bool bSaveOnEndPlay;
	
	
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
	/** Loads the player's saved information once the client has joined the game and the player has spawned */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual void LoadPlayerInformation();

	/**
	 * Loads specific saved information for the player
	 *
	 * @param InformationType			The type of information that's being loading
	 * @returns							True if the information has been successfully loaded 
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual bool LoadData(const ESaveType InformationType);

	/** Blueprint function to load the player's saved information on the client once the client has joined the game and the player has spawned */
	UFUNCTION(BlueprintImplementableEvent, Category = "Saving and Loading", DisplayName = "On Save Data") void BP_LoadPlayerInformation();



	
//----------------------------------------------------------------------------------//
// SaveSlot Config																	//
//----------------------------------------------------------------------------------//
protected:
	/** The actual save slot iteration is based on the world's current save iteration. Everything that happens during that moment is stored here with a timestamp */
	UPROPERTY(BlueprintReadWrite) int32 CurrentSaveIteration = 0;

	/** If it's current split screen we need to retrieve the player controller's split screen reference, and use it for the user index */
	UPROPERTY(BlueprintReadWrite) int32 SplitScreenIndex = 0;


public:
	/** Initializes all the default save logic to retrieve save information for the character */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual void InitializeSaveSlotConfig();
	
	/**
	 * Retrieves the save's reference for specific save states using the save state and actor's information. This should vary between players and enemies spawned in the world \n\n 
	 * Default logic just returns FName. Subclassed versions should return the actor's unique network or npc id followed by a reference to the type of information they're saving 
	 *
	 * SaveSlot reference
	 *	- Net/Platform/Actor reference
	 *	- Save Category (SaveType)
	 *	- Slot Index
	 *	- Save Iteration (auto / manual save)
	 *
	 *	Example: CharacterId_SaveCategory_SaveSlotIndex_IterationAndAutoSaveIndex
	 *		
	 * @param Saving					The type of information we're saving
	 * @param Iteration				The save index for this specific character's individual saves. Retrieves the current iteration if one isn't provided
	 * @returns							The id used for saving information to a specific slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading") virtual FString GetCurrentSaveSlot(const ESaveType Saving, const int32 Iteration = -1) const;

	/** Utility function to construct a custom Save Slot Reference using the current functionality in place for saving */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading")
	virtual FString ConstructSaveSlot(int32 NetDriverId, FString AccountPlatformId, FString SaveCategory, int32 SlotIndex, int32 SaveIteration) const;

	// TODO: This should be a blueprint library function to prevent duplication with save state / game logic
	/** Retrieves the character's save slot id specific to their steam/console platform account for saving purposes */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual int32 GetNetId() const;
	
	/** Retrieves the character's platform id. Online games should use the subsystem account, character's should use platform id, and npc's and ai should use the name reference */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual FString GetPlatformId() const;
	
	/** Retrieves the character's save category id (the save type) */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual FString GetSaveCategory(const ESaveType SaveType) const;
	
	/** Retrieves the character's save slot index to retrieve saves from */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual int32 GetSaveSlotIndex() const;

	/** Retrieves the index of the player during split screen for handling save information */ // TODO: Should account information just be used?
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual int32 GetUserIndex() const;

	/**
	 * Retrieves the next save iteration based on the number of saves for this specific save slot.
	 * The actual save slot iteration is based on the world's current save iteration. Everything that happens during that moment is stored here with a timestamp
	 */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual int32 GetSaveIteration() const;

	// TODO: Is there a more efficient way that prevents this from not being safe?
	/** Finds the player's last save iteration, and set's the current save iteration to it. This is used for retrieving current saves and help with the list of saved information */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual int32 FindSaveIteration() const;

	/** Sets the character's Net and Platform Id based on whether it's a player, and if it's single / multiplayer games with subsystems */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual void SetNetAndPlatformId();

	/** Sets the current slot index. This should be invoked in the main menu and retrieved from the game mode / game instance */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual void SetSaveSlotIndex(const int32 Index);



	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
protected:
	/** Handle for preventing certain information from being saved during gameplay. Useful for in game debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving|Debugging") TMap<ESaveType, bool> PreventLoading;

	/** Whether this component is currently preventing loading specific information for debugging purposes */
	virtual bool PreventingLoadingFor(const ESaveType SaveType) const;

	
public:
	/** Returns whether the save component handles saving information for a specific save type */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual bool HandlesSaving(const ESaveType SaveType) const;

	/** Returns an array of the save types this component saves */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual TArray<ESaveType> GetSaveTypes() const;

	/** Utility logic for returning formatted save state for blueprint display. Helpful with debugging save information */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading|Utility") virtual FString PrintSaveState(const ESaveType SaveType, const FString Slot) const;
	
	
};
