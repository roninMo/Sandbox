// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/Data/Structs/QuestInformation.h"
#include "QuestObjective.generated.h"

class APlayerCharacter;
class UQuest;


/*



Objective
	- Save / Restart
	- information / objective
	- Location, Combat, Retrieval, a condition that's required to be met (handle with delegates and state)
	- OnBegin / OnCompletion



*/


/**
 * Objectives required for completing quests.
 */
UCLASS(Blueprintable, BlueprintType, Category = Quest)
class SANDBOX_API UQuestObjective : public UObject
{
	GENERATED_BODY()

protected:
	/**** The majority of the stored information and logic should be specific to each objective, and handled during gameplay ****/
	/** A reference to the current quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UQuest* Quest;

	/** The name of the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ObjectiveName;

	/** The quest's index for this objective. This isn't necessary, and could be used to handle the objective order */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Index;
	
	/** The name of the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EObjectiveStatus ObjectiveStatus;

	/** The objective's display name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DisplayName;

	/** The objective's description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Description;
	
	/** The objective save information reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_QuestObjectiveInformation SavedInformation;

	/** A reference to the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) APlayerCharacter* Character;

	
protected:
	UQuestObjective(const FObjectInitializer& ObjectInitializer);

	
//----------------------------------------------------------------------------------//
// Objective logic																	//
//----------------------------------------------------------------------------------//
public:
	/** Initialize the objective information and references from saved information. Only used if the player has already began an objective  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective") void InitInformation(const F_QuestObjective& QuestObjective, const FS_QuestObjectiveInformation& SaveData, APlayerCharacter* Player);
	virtual void InitInformation_Implementation(const F_QuestObjective& QuestObjective, const FS_QuestObjectiveInformation& SaveData, APlayerCharacter* Player);

	/** Handle beginning the objective and any logic required */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective|Logic") void BeginObjective();
	virtual void BeginObjective_Implementation();
	
	/** Handles the objective logic and returns whether the objective has been completed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective|Logic") EObjectiveStatus HandleObjective();
	virtual EObjectiveStatus HandleObjective_Implementation();
	
	/** Handle any logic that's necessary if the player has failed an objective */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective|Completion") void FailedObjective();
	virtual void FailedObjective_Implementation();
	
	/** Handle any logic that's necessary once the player has completed an objective */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective|Completion") void FinishObjective();
	virtual void FinishObjective_Implementation();

	
//----------------------------------------------------------------------------------//
// Saving																			//
//----------------------------------------------------------------------------------//
public:
	/** Returns the objective's save information */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective|Saving") FS_QuestObjectiveInformation SaveObjectiveInformation();
	virtual FS_QuestObjectiveInformation SaveObjectiveInformation_Implementation();
	
	/** Loads the information to the objective */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective|Saving") void LoadObjectiveInformation(FS_QuestObjectiveInformation SavedObjectiveInformation);
	virtual void LoadObjectiveInformation_Implementation(FS_QuestObjectiveInformation SavedObjectiveInformation);

	/** Returns the objective's save information */
	UFUNCTION(BlueprintCallable, Category = "Quest Objective|Saving") FS_QuestObjectiveInformation GetSaveInformation() const;

	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/** Retrieves the current status of the objective */
	UFUNCTION(BlueprintCallable, Category = "Quest Objective|Utility") virtual EObjectiveStatus GetStatus() const;

	/** Retrieves the current status of the objective */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest Objective|Utility") void SetObjectiveStatus(EObjectiveStatus Status);
	virtual void SetObjectiveStatus_Implementation(EObjectiveStatus Status);

	/** Retrieves the name of the objective. When the objectives are initialized using the quest's information, this is taken from the list */
	UFUNCTION(BlueprintCallable, Category = "Quest Objective|Utility") virtual FName GetObjectiveName() const;

	
};

