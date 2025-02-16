#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sandbox/Data/Enums/QuestTypes.h"
#include "QuestInformation.generated.h"

class UQuestObjective;
class UQuest;



/**
 * Saved information pertaining to objectives that are in progress
 */
USTRUCT(Blueprintable, BlueprintType)
struct F_QuestObjective
{
	GENERATED_USTRUCT_BODY()
	F_QuestObjective(
		const int32 Index = 0,
		UQuestObjective* Objective = nullptr,
		const FName Name = FName(),
		const TSubclassOf<UQuestObjective> Class = nullptr
	) :
		Index(Index),
		Objective(Objective),
		Name(Name),
		Class(Class)
	{}

	/** Just checks if the quest objective has been created */
	virtual bool IsValid() const
	{
		return !this->Name.IsNone();
	}

	
public:
	/** The index/order of objectives. The specific quests handle routing for each of the quests, this just helps with the traditional order of operations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Index;

	/** A saved reference to the current objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UQuestObjective* Objective;

	/** The name of the current objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Name;
	
	/** A saved reference to the class of the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UQuestObjective> Class;
	
};




/**
 * Saved information pertaining to objectives that are in progress
 */
USTRUCT(Blueprintable, BlueprintType)
struct FS_QuestObjectiveInformation
{
	GENERATED_USTRUCT_BODY()
	FS_QuestObjectiveInformation(
		const EObjectiveStatus Status = EObjectiveStatus::Locked,
		const FVector& CheckpointLocation = FVector::ZeroVector,
		const FRotator& CheckpointRotation = FRotator::ZeroRotator
		// const FS_Inventory& Inventory = FS_Inventory(),
		// const FS_Equipment& Equipment = FS_Equipment(),
		// const FS_Stats& Stats = FS_Stats(),
	) : 
		Status(Status),
		CheckpointLocation(CheckpointLocation),
		CheckpointRotation(CheckpointRotation)
		// Inventory(Inventory),
		// Equipment(Equipment),
		// Stats(Stats)
	{}

public:
	/**** Begin objective ****/
	/** The current status of the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EObjectiveStatus Status;

	/**** objective checkpoint ****/
	/** The player location once they reach a checkpoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector CheckpointLocation;
	
	/** The player rotation once they reach a checkpoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator CheckpointRotation;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_Inventory Inventory;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_Equipment Equipment;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_Stats Stats;

	// Information specific to the world and objectives
	
};




/**
 * Save information pertaining to quests that are in progress
 */
USTRUCT(Blueprintable, BlueprintType)
struct FS_QuestInformation
{
	GENERATED_USTRUCT_BODY()
		FS_QuestInformation(
			const EQuestState State = EQuestState::Locked,
			const TMap<FName, FS_QuestObjectiveInformation>& SavedObjectives = {}
		) :
			State(State),
			SavedObjectives(SavedObjectives)
	{}
	
	/** Check if it's valid save information if they've made progress on the quest and saved information */
	virtual bool IsValid() const
	{
		return !this->SavedObjectives.IsEmpty();
	}
	

public:
	virtual ~FS_QuestInformation() = default;
	/** Whether the quest is in progress, completed, or locked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EQuestState State;

	/** The saved objective information of the quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, FS_QuestObjectiveInformation> SavedObjectives;

	
};




/**
 * Saved information pertaining to objectives that are in progress
 */
USTRUCT(Blueprintable, BlueprintType)
struct FS_QuestProgress
{
	GENERATED_USTRUCT_BODY()
	FS_QuestProgress(
		const TMap<FName, FS_QuestInformation>& QuestsInProgress = {},
		const TMap<FName, EQuestState>& QuestCompletionProgress = {}
	) : 
		QuestsInProgress(QuestsInProgress),
		QuestCompletionProgress(QuestCompletionProgress)
	{}

public:
	/** The saved information of the quests that the player is in progress with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, FS_QuestInformation> QuestsInProgress;

	/** A list of the statuses of the quests the player has completed, in progress, locked, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, EQuestState> QuestCompletionProgress;
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
	
	
};




/**
 * This is the data table to hold all the quests in the game. Quests are unique, and have a specific set of requirements and order, and are linked to objectives
 * TODO: Find a way to link level persistance with quests, or use gameplay tags for handling referencing things in the level for objectives
 */
USTRUCT(BlueprintType)
struct F_QuestDatabase : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<F_QuestObjective> Objectives;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UQuest> Quest;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};



