// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Game/Quests/QuestObjective.h"

#include "Quest.h"
#include "Sandbox/Characters/Player/PlayerCharacter.h"

UQuestObjective::UQuestObjective(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void UQuestObjective::InitInformation_Implementation(const F_QuestObjective& QuestObjective, const FS_QuestObjectiveInformation& SaveData, APlayerCharacter* Player)
{
	ObjectiveName = QuestObjective.Name;
	Character = Player;
	Quest = Cast<UQuest>(GetOuter());
	Index = QuestObjective.Index;

	// Save information
	LoadObjectiveInformation(SaveData);
}


void UQuestObjective::BeginObjective_Implementation()
{
	if (!Character) return;

	SetObjectiveStatus(EObjectiveStatus::InProgress);
	SaveObjectiveInformation();
}


EObjectiveStatus UQuestObjective::HandleObjective_Implementation()
{
	return EObjectiveStatus::InProgress;
}


void UQuestObjective::FailedObjective_Implementation()
{
	SetObjectiveStatus(EObjectiveStatus::Failed);
}


void UQuestObjective::FinishObjective_Implementation()
{
	SetObjectiveStatus(EObjectiveStatus::Completed);
}


FS_QuestObjectiveInformation UQuestObjective::SaveObjectiveInformation_Implementation()
{
	if (!Character) return FS_QuestObjectiveInformation();

	SavedInformation.Status = ObjectiveStatus;
	
	// Save the player location
	SavedInformation.CheckpointLocation = Character->GetActorLocation();
	SavedInformation.CheckpointRotation = Character->GetActorRotation();
	
	// Save the player's information
	
	return SavedInformation;
}


void UQuestObjective::LoadObjectiveInformation_Implementation(FS_QuestObjectiveInformation SavedObjectiveInformation)
{
	SavedInformation = SavedObjectiveInformation;
	SetObjectiveStatus(SavedInformation.Status);
}


FS_QuestObjectiveInformation UQuestObjective::GetSaveInformation() const
{
	return SavedInformation;
}


EObjectiveStatus UQuestObjective::GetStatus() const
{
	return ObjectiveStatus;
}


void UQuestObjective::SetObjectiveStatus_Implementation(const EObjectiveStatus Status)
{
	ObjectiveStatus = Status;
}


FName UQuestObjective::GetObjectiveName() const
{
	return ObjectiveName;
}
