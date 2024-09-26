// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Enums/QuestTypes.h"
#include "Sandbox/Data/Structs/QuestInformation.h"
#include "Quest.generated.h"

class APlayerCharacter;
class UDataTable;
class UQuestObjective;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestBegin, UQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestUpdate, UQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestFailed, UQuest*, Quest, UQuestObjective*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, UQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveBegin, F_QuestObjective&, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveFailed, F_QuestObjective&, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveFinished, F_QuestObjective&, Objective);


/**
 * Class for handling the logic for a quest. Blueprint the class and objectives for handling specific logic and information to handle specific logic and player information
 * Focus on handling user logic individually, and let something else handle distributing quests and objectives to every character instead of trying to handle it all here
 */
UCLASS(Blueprintable, BlueprintType, Category = Quest)
class SANDBOX_API UQuest : public UObject
{
public:
	GENERATED_BODY()

protected:
	/**** Quest information ****/
	/** The stored name of the quest. Used to reference retrieving information from the database */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName QuestName;

	/** Whether the quest is in progress, completed, or locked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EQuestState State;

	/** The display name of the quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DisplayName;

	/** The quest's description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Description;

	/** The current quest objectives */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, F_QuestObjective> Objectives;

	/** The objective save information reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_QuestInformation SavedQuestInformation;

	
	/**** Utility ****/
	/** A reference to the character */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<APlayerCharacter> Character;
	
	
public:
	UQuest(const FObjectInitializer& ObjectInitializer);

	/** Delegate for when the player has began a quest */
	UPROPERTY(BlueprintAssignable) FOnQuestBegin OnQuestBegin;
	
	/** Delegate for when the the quest information has been updated. Use this for updating the hud */
	UPROPERTY(BlueprintAssignable) FOnQuestUpdate OnQuestUpdate;
	
	/** Delegate for when the player has began a quest */
	UPROPERTY(BlueprintAssignable) FOnObjectiveBegin OnObjectiveBegin;
	
	/** Delegate for when the player has failed an objective */
	UPROPERTY(BlueprintAssignable) FOnObjectiveFailed OnObjectiveFailed;

	/** Delegate for when the player has completed an objective */
	UPROPERTY(BlueprintAssignable) FOnObjectiveFinished OnObjectiveCompleted;
	
	/** Delegate for when the player has failed a quest */
	UPROPERTY(BlueprintAssignable) FOnQuestFailed OnQuestFailed;

	/** Delegate for when the player has completed a quest */
	UPROPERTY(BlueprintAssignable) FOnQuestCompleted OnQuestCompleted;


	// Check if quest is unlocked
	// Check if valid to begin quest
	// Begin quest
		// quest init logic
		// begin objective, and save player information
		// handle objective
		// on finish objective, begin next objective
		// -> until all objectives have been finished
	// Finish quest
	// save information to player

	// Reload from objective -> checkpoint
	// Reload quest
	
	
//----------------------------------------------------------------------------------//
// Quest logic																		//
//----------------------------------------------------------------------------------//
protected:
	/**** Ready Quest Information ****/
	/** Retrieves the quest information and saved information if the quest was already in progress  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest") bool InitQuestInformation(const TArray<F_QuestObjective>& QuestObjectives, const FS_QuestInformation& SavedInformation);
	virtual bool InitQuestInformation_Implementation(const TArray<F_QuestObjective>& QuestObjectives, const FS_QuestInformation& SavedInformation);
	
	/** Have the requirements for this quest been cleared yet? */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Validation") bool RequirementsCleared();
	virtual bool RequirementsCleared_Implementation();

	/** Whether it's valid to begin the quest (if the player's at the right location, or other logic that influences the quest that isn't specific to the requirements */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Validation") bool QuestReady();
	virtual bool QuestReady_Implementation();

	/** Begins a quest from a save or from the beginning, and handles quest and objective information */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Logic") bool BeginQuest();
	virtual bool BeginQuest_Implementation();

	
	/**** Actual Quest Logic ****/
	/** Handles checking if the quest and objectives have been finished. This is handled both during update and event logic */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Logic") void HandleQuest();
	virtual void HandleQuest_Implementation();

	/** Handles checking if the current objective has been completed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Logic") void HandleCurrentObjective(F_QuestObjective& QuestObjective);
	virtual void HandleCurrentObjective_Implementation(F_QuestObjective& QuestObjective);

	/** Checks if all objectives have been completed, and returns true if so */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Logic") bool CompletedAllObjectives();
	virtual bool CompletedAllObjectives_Implementation();
	
	/** Begins the next objective, handles objective's initialization and OnBegin logic, and sets current objective to the objective  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Logic") bool BeginNextObjective(F_QuestObjective& FinishedObjective);
	virtual bool BeginNextObjective_Implementation(F_QuestObjective& FinishedObjective);

	
	/**** Finished Quest Logic ****/
	/** Finishes a quest and handles quest completion rewards, save information, etc */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Completion") void HandleQuestCompleted();
	virtual void HandleQuestCompleted_Implementation();

	/** Handles what happens when the player fails a quest. Optionally pass in an objective if it's what caused the quest to fail */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Completion") void HandleQuestFailed(UQuestObjective* FailedObjective = nullptr);
	virtual void HandleQuestFailed_Implementation(UQuestObjective* FailedObjective = nullptr);
	
	/** Once a quest has been completed, handles the rewards for the player */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Completion") void HandleQuestRewards();
	virtual void HandleQuestRewards_Implementation();
	
	/** Restarts at the quest objective's checkpoint */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Logic") bool RestartAtCheckpoint(FName ObjectiveName);
	virtual bool RestartAtCheckpoint_Implementation(FName ObjectiveName);

	/** Restarts the quest */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Logic") bool RestartQuest();
	virtual bool RestartQuest_Implementation();
	

//----------------------------------------------------------------------------------//
// Saving																			//
//----------------------------------------------------------------------------------//
public:
	/** Saves the quest information to the quest */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Saving") void SaveQuestInformation();
	virtual void SaveQuestInformation_Implementation();

	/** Loads the quest information from the quest */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Saving") void LoadQuestInformation(FS_QuestInformation& QuestInformation);
	virtual void LoadQuestInformation_Implementation(FS_QuestInformation& QuestInformation);

	/** Saves the objective information to the quest */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Saving") void SaveObjective(F_QuestObjective QuestObjective);
	virtual void SaveObjective_Implementation(F_QuestObjective QuestObjective);

	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/** Retrieves the quest information and saved information if the quest was already in progress  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Utility") void SetQuestState(EQuestState QuestState);
	virtual void SetQuestState_Implementation(EQuestState QuestState);

	/** Creates a quest objective, and loads saved information for the objective */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Utility") UQuestObjective* CreateObjective(const F_QuestObjective& Information);
	virtual UQuestObjective* CreateObjective_Implementation(const F_QuestObjective& Information);
	
	/** Returns true if retrieved the character's information */
	UFUNCTION(BlueprintCallable, Category = "Quest|Utility") virtual bool GetCharacterInformation();

	/** Returns true if retrieved the character */
	UFUNCTION(BlueprintCallable, Category = "Quest|Utility") virtual bool GetCharacter();
	
	
};

