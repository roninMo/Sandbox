// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Controllers/AIControllerBase.h"

#include "Sandbox/AI/Characters/Npc.h"
#include "Logging/StructuredLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "Perception/AISenseConfig_Team.h"
#include "Perception/AISense_Sight.h"

DEFINE_LOG_CATEGORY(AIInformationLog);


AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}


void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
}


void AAIControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void AAIControllerBase::StartBehaviorTree(UBehaviorTree* BTAsset, bool bSaveValues)
{
}


// void AAIControllerBase::SetDynamicBehaviorSubTree(const FGameplayTag& BehaviorTreeTag, UBehaviorTree* BTAsset)
// {
// }




#pragma region Senses
void AAIControllerBase::AddSenses()
{
	if (!GetCharacter()) return;
	
	// Sight
	AddSightSense(AICharacter->GetSenseConfig());
	PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
	
	// Team
	UAISenseConfig_Team* TeamConfig = NewObject<UAISenseConfig_Team>(this, UAISenseConfig_Team::StaticClass(), TEXT("UAISenseConfig_Team"));
	PerceptionComponent->ConfigureSense(*TeamConfig);

	// Prediction
	UAISenseConfig_Prediction* PredictionConfig = NewObject<UAISenseConfig_Prediction>(this, UAISenseConfig_Prediction::StaticClass(), TEXT("UAISenseConfig_Prediction"));
	PerceptionComponent->ConfigureSense(*PredictionConfig);
	PerceptionComponent->ConfigureSense(*PredictionConfig);

	// Hearing
	AddHearingSense(AICharacter->GetSenseConfig().HearingRange);
	
	PerceptionComponent->RequestStimuliListenerUpdate();
}


void AAIControllerBase::AdjustSenses(F_AISenseConfigurations& UpdatedSenses)
{
}


void AAIControllerBase::AddSightSense(F_AISenseConfigurations& UpdatedSenses)
{
}


void AAIControllerBase::AddHearingSense(float HearingDistance)
{
}


void AAIControllerBase::OnPerceptionUpdated_Implementation(TArray<AActor*>& Actors)
{
}


void AAIControllerBase::OnTargetPerceptionUpdated(FActorPerceptionUpdateInfo UpdateInformation)
{
}


void AAIControllerBase::OnTargetPerceptionForgotten(AActor* Actor)
{
}


void AAIControllerBase::OnSenseSightUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
{
}


void AAIControllerBase::OnSenseTeamUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
{
}


void AAIControllerBase::OnSensePredictionSenseUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
{
}


void AAIControllerBase::OnSenseHearingUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
{
}
#pragma endregion 




#pragma region Get and Set functions
ANpc* AAIControllerBase::GetSelfActor() const { return Cast<ANpc>(Blackboard->GetValueAsObject(_SelfActor)); }
FVector AAIControllerBase::GetSpawnLocation() const { return Blackboard->GetValueAsVector(_SpawnLocation); }
FRotator AAIControllerBase::GetSpawnRotation() const { return Blackboard->GetValueAsRotator(_SpawnRotation); }
void AAIControllerBase::SetSelfActor(ANpc* SelfActor) { Blackboard->SetValueAsObject(_SpawnLocation, SelfActor); }
void AAIControllerBase::SetSpawnLocation(const FVector SpawnLocation) { Blackboard->SetValueAsVector(_SpawnLocation, SpawnLocation); }
void AAIControllerBase::SetSpawnRotation(const FRotator SpawnRotation) { Blackboard->SetValueAsRotator(_SpawnRotation, SpawnRotation); }


ETeamAttitude::Type AAIControllerBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	return Super::GetTeamAttitudeTowards(Other);
}


bool AAIControllerBase::GetCharacterInformation()
{
	if (AICharacter) return true;
	if (!GetCharacter()) return false;
	return true;
}


bool AAIControllerBase::GetCharacter()
{
	if (AICharacter) return true;

	AICharacter = Cast<ANpc>(GetPawn());
	if (!AICharacter)
	{
		UE_LOGFMT(AIInformationLog, Error, "{0}::{1}() Failed to find the character!",
			GetPawn() ? *UEnum::GetValueAsString(GetPawn()->GetLocalRole()) : *FString(""),
			*GetNameSafe(GetPawn())
		);
		return false;
	}

	return true;
}


UAbilitySystemComponent* AAIControllerBase::GetAbilitySystemComponent() const
{
	return nullptr;
}


UAttributeSet* AAIControllerBase::GetAttributeSet() const
{
	return nullptr;
}
#pragma endregion 