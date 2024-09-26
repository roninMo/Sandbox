// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Game/Quests/Quest.h"

#include "QuestObjective.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Player/PlayerCharacter.h"


UQuest::UQuest(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


bool UQuest::InitQuestInformation_Implementation(const TArray<F_QuestObjective>& QuestObjectives, const FS_QuestInformation& SavedInformation)
{
	if (!GetCharacterInformation() || QuestObjectives.IsEmpty()) return false;

	// Save information and quest initialization
	SetQuestState(SavedQuestInformation.State);
	SavedQuestInformation = SavedInformation;

	// Create the objectives and load the saved information
	Objectives.Empty();
	for (F_QuestObjective QuestObjective : QuestObjectives)
	{
		UQuestObjective* Objective = CreateObjective(QuestObjective);
		if (!Objective || !QuestObjective.IsValid())
		{
			Objective->BeginDestroy();
			return false;
		}

		// Check if there's saved information for the objective
		QuestObjective.Objective = Objective;
		if (SavedQuestInformation.SavedObjectives.Contains(QuestObjective.Name))
		{
			Objective->LoadObjectiveInformation(SavedQuestInformation.SavedObjectives[QuestObjective.Name]);
		}
		
		Objectives.Add(QuestObjective.Name, QuestObjective);
	}

	// Let level loading handle the general spawning of enemies and world information
	// The information and logic specific to objectives should be handled during objective initialization 
	// Handle spawning the player and it's information specific to the quest with the objectives (either initial logic or saved logic)
	
	return true;
}


bool UQuest::RequirementsCleared_Implementation()
{
	return false;
}


bool UQuest::QuestReady_Implementation()
{
	return false;
}


bool UQuest::BeginQuest_Implementation()
{
	if (!GetCharacterInformation()) return false;

	// Spawn the player at the checkpoint location 

	// Update the player information, equipment, and weapons
	
	OnQuestBegin.Broadcast(this);
	return true;
}


void UQuest::HandleQuest_Implementation()
{
	for (auto [ObjectiveName, QuestObjective] : Objectives)
	{
		UQuestObjective* Objective = QuestObjective.Objective;
		if (Objective->GetStatus() == EObjectiveStatus::InProgress)
		{
			HandleCurrentObjective(QuestObjective);
		}
	}
}


void UQuest::HandleCurrentObjective_Implementation(F_QuestObjective& QuestObjective)
{
	UQuestObjective* Objective = QuestObjective.Objective;
	if (!Objective) return;

	EObjectiveStatus Progress = Objective->HandleObjective();
	if (Progress == EObjectiveStatus::InProgress) return;
	if (Progress == EObjectiveStatus::Failed)
	{
		// The player failed the objective, handle the different logic for each quest
		SaveQuestInformation();
		OnObjectiveFailed.Broadcast(QuestObjective);
		HandleQuestFailed(Objective);
	}
	else if (Progress == EObjectiveStatus::Completed)
	{
		// The player finished the objective, save the information and begin the next objective
		SaveObjective(QuestObjective);
		SaveQuestInformation();
		OnObjectiveCompleted.Broadcast(QuestObjective);

		if (CompletedAllObjectives())
		{
			HandleQuestCompleted();
		}
		else
		{
			// Handle updating the objectives and being the next objective
			BeginNextObjective(QuestObjective);
		}
	}
}


bool UQuest::BeginNextObjective_Implementation(F_QuestObjective& FinishedObjective)
{
	if (!GetCharacter()) return false;

	return true;
}


bool UQuest::CompletedAllObjectives_Implementation()
{
	for (auto &[Name, QuestObjective] : Objectives)
	{
		if (QuestObjective.Objective && QuestObjective.Objective->GetStatus() != EObjectiveStatus::Completed)
		{
			return false;
		}
	}
	
	return false;
}


void UQuest::HandleQuestCompleted_Implementation()
{
	OnQuestCompleted.Broadcast(this);

	// If we're using steam subsystems we should probably create a database for handling quest information and ways to reference and save information during their specific events
	// I don't know what subsystems we should use for saving information yet, that's something that needs to be handled for multiplayer
}


void UQuest::HandleQuestFailed_Implementation(UQuestObjective* FailedObjective)
{
	OnQuestFailed.Broadcast(this, FailedObjective);
}


void UQuest::HandleQuestRewards_Implementation()
{
	// Experience, loot, etc.
}


bool UQuest::RestartAtCheckpoint_Implementation(const FName ObjectiveName)
{
	if (SavedQuestInformation.SavedObjectives.Contains(ObjectiveName))
	{
		FS_QuestObjectiveInformation SavedCheckpoint = SavedQuestInformation.SavedObjectives[ObjectiveName];
		
		// Spawn the player at the checkpoint location

		// Update the player information, equipment, and weapons using the checkpoint's saved information 

		return true;
	}

	return false;
}


bool UQuest::RestartQuest_Implementation()
{
	// Restart quest and objective information and save state

	// BeginQuest()
	
	return false;
}


void UQuest::SaveQuestInformation_Implementation()
{
	SavedQuestInformation.State = State;

	for (auto [ObjectiveName, Information] : Objectives)
	{
		if (Information.Objective && ObjectiveName.IsValid())
		{
			SaveObjective(Information);
			SavedQuestInformation.SavedObjectives.Add(Information.Name, Information.Objective->GetSaveInformation());
		}
	}
}


void UQuest::LoadQuestInformation_Implementation(FS_QuestInformation& QuestInformation)
{
	SavedQuestInformation = QuestInformation;
	SetQuestState(SavedQuestInformation.State);

	for (auto [ObjectiveName, Information] : Objectives)
	{
		if (Information.Objective && SavedQuestInformation.SavedObjectives.Contains(Information.Name))
		{
			Information.Objective->LoadObjectiveInformation(SavedQuestInformation.SavedObjectives[Information.Name]);
		}
	}
}


void UQuest::SaveObjective_Implementation(const F_QuestObjective QuestObjective)
{
	if (!QuestObjective.Objective) return;

	QuestObjective.Objective->SaveObjectiveInformation();
}


void UQuest::SetQuestState_Implementation(const EQuestState QuestState)
{
	State = QuestState;
}


UQuestObjective* UQuest::CreateObjective_Implementation(const F_QuestObjective& Information)
{
	UQuestObjective* Objective = NewObject<UQuestObjective>(Character, Information.Class);
	if (!Objective) return nullptr;
	
	if (SavedQuestInformation.SavedObjectives.Contains(Information.Name))
	{
		Objective->LoadObjectiveInformation(SavedQuestInformation.SavedObjectives[Information.Name]);
	}

	return Objective;
}


bool UQuest::GetCharacterInformation()
{
	if (Character) return true;
	if (!GetCharacter()) return false;
	return true;
}


bool UQuest::GetCharacter()
{
	if (Character) return true;

	Character = Cast<APlayerCharacter>(GetOuter());
	if (!Character)
	{
		UE_LOGFMT(LogTemp, Error, "{0} {1}() Something happened while trying to find the character!", *GetName(), *FString(__FUNCTION__));
	}

	return true;
}
