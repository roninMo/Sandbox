// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Controllers/AIControllerBase.h"

#include "BrainComponent.h"
#include "Sandbox/AI/Characters/Npc.h"
#include "Logging/StructuredLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Sandbox/AI/Components/Perception/AIPerceptionComponentBase.h"

DEFINE_LOG_CATEGORY(AIInformationLog);


AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("ActionsComp")))
{
	MinNetUpdateFrequency = 33;
	NetUpdateFrequency = 66;
	bReplicates = false;
	
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponentBase>(TEXT("Perception"));
	PerceptionComponent->SetIsReplicated(false);
	SetPerceptionComponent(*PerceptionComponent);
	
	PerceptionStimuli = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimuli Perception Source"));
	PerceptionStimuli->SetIsReplicated(false);
	
	// Base AI Configuration
	bStartAILogicOnPossess = 1;
	bAllowStrafe = 1;
	//bWantsPlayerState = 1; // this might be useful later for specific situations
}


void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	GetCharacterInformation();

	if (BehaviorTree) StartBehaviorTree(BehaviorTree, false);
	
	// InitSenses();
	// UAIPerceptionSystem::GetCurrent(GetWorld())->UpdateListener(*PerceptionComponent);
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
	if (!BTAsset) return;

	ANpc* SelfActor = AICharacter;
	FVector SpawnLocation = AICharacter->GetActorLocation();
	FRotator SpawnRotation = AICharacter->GetActorRotation();

	if (bSaveValues && Blackboard)
	{
		SelfActor = GetSelfActor();
		SpawnLocation = GetSpawnLocation();
		SpawnRotation = GetSpawnRotation();
	}

	// Handle if there's already another behavior tree 
	UBrainComponent* Tree = GetBrainComponent();
	if (Tree)
	{
		CleanupBrainComponent();
		Tree->DestroyComponent();
	}
	
	// Update the values of this behavior tree
	BehaviorTree = BTAsset;
	RunBehaviorTree(BTAsset);
	SetSelfActor(SelfActor);
	SetSpawnLocation(SpawnLocation);
	SetSpawnRotation(SpawnRotation);
}


// void AAIControllerBase::SetDynamicBehaviorSubTree(const FGameplayTag& BehaviorTreeTag, UBehaviorTree* BTAsset)
// {
// 	if (!BrainComponent || !BTAsset || !BehaviorTreeTag.IsValid()) return;
//
// 	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
// 	if (BehaviorTreeComponent)
// 	{
// 		BehaviorTreeComponent->SetDynamicSubtree(BehaviorTreeTag, BTAsset);
// 	}
// }




#pragma region Senses
void AAIControllerBase::OnPerceptionUpdated_Implementation(TArray<AActor*>& Actors)
{
}


void AAIControllerBase::OnTargetPerceptionUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation)
{
}


void AAIControllerBase::OnTargetPerceptionForgotten_Implementation(AActor* Actor)
{
}


void AAIControllerBase::OnSenseSightUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
{
}


void AAIControllerBase::OnSenseTeamUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
{
}


void AAIControllerBase::OnSenseHearingUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
{
}


void AAIControllerBase::OnSensePredictionSenseUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target)
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
		UE_LOGFMT(AIInformationLog, Error, "{0}::{1}() Failed to cast {2} to an npc character!",
			GetPawn() ? *UEnum::GetValueAsString(GetPawn()->GetLocalRole()) : *FString(""),
			*FString(__FUNCTION__), *GetName()
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
