// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Components/Perception/AIPerceptionComponentBase.h"
#include "Sandbox/AI/Characters/Npc.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Team.h"
#include "Perception/AISense_Sight.h"
#include "Logging/StructuredLog.h"
#include "Perception/AISenseConfig_Blueprint.h"
#include "Perception/AISenseConfig_Damage.h"


UAIPerceptionComponentBase::UAIPerceptionComponentBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void UAIPerceptionComponentBase::InitSenses()
{
	if (!GetCharacter()) return;
	
	// Sight
	AdjustSightSense(Npc->GetSenseConfig());
	SetDominantSense(UAISense_Sight::StaticClass());
	
	// Team
	UAISenseConfig_Team* TeamConfig = NewObject<UAISenseConfig_Team>(this, UAISenseConfig_Team::StaticClass(), TEXT("UAISenseConfig_Team"));
	ConfigureSense(*TeamConfig);

	// Hearing
	AdjustHearingSense(Npc->GetSenseConfig().HearingRange);

	// Prediction
	UAISenseConfig_Prediction* PredictionConfig = NewObject<UAISenseConfig_Prediction>(this, UAISenseConfig_Prediction::StaticClass(), TEXT("UAISenseConfig_Prediction"));
	ConfigureSense(*PredictionConfig);
	ConfigureSense(*PredictionConfig);
	
	RequestStimuliListenerUpdate();
}


void UAIPerceptionComponentBase::AdjustSightSense(F_AISenseConfigurations& UpdatedSenses)
{
	UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>(this, UAISenseConfig_Sight::StaticClass(), TEXT("UAISenseConfig_Sight"));
	check(SightConfig);
	
	SightConfig->SightRadius = UpdatedSenses.SightRadius;
	SightConfig->LoseSightRadius = UpdatedSenses.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = UpdatedSenses.PeripheralVisionAngleDegrees;
	SightConfig->DetectionByAffiliation = FAISenseAffiliationFilter();
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = UpdatedSenses.AutoSuccessRangeFromLastSeenLocation;
	SightConfig->PointOfViewBackwardOffset = UpdatedSenses.PointOfViewBackwardOffset;
	SightConfig->NearClippingRadius = UpdatedSenses.NearClippingRadius;
	ConfigureSense(*SightConfig);
}


void UAIPerceptionComponentBase::AdjustTeamSense(F_AISenseConfigurations& UpdatedSenses)
{
	UAISenseConfig_Team* TeamConfig = NewObject<UAISenseConfig_Team>(this, UAISenseConfig_Team::StaticClass(), TEXT("UAISenseConfig_Team"));
	ConfigureSense(*TeamConfig);
}


void UAIPerceptionComponentBase::AdjustPredictionSense(float HearingDistance)
{
	UAISenseConfig_Prediction* PredictionConfig = NewObject<UAISenseConfig_Prediction>(this, UAISenseConfig_Prediction::StaticClass(), TEXT("UAISenseConfig_Prediction"));
	ConfigureSense(*PredictionConfig);
}


void UAIPerceptionComponentBase::AdjustDamageSense(float HearingDistance)
{
	UAISenseConfig_Damage* DamageConfig = NewObject<UAISenseConfig_Damage>(this, UAISenseConfig_Damage::StaticClass(), TEXT("UAISenseConfig_Damage"));
	ConfigureSense(*DamageConfig);
}


void UAIPerceptionComponentBase::AdjustHearingSense(float HearingDistance)
{
	UAISenseConfig_Hearing* HearingConfig = NewObject<UAISenseConfig_Hearing>(this, UAISenseConfig_Hearing::StaticClass(), TEXT("UAISenseConfig_Hearing"));
	check(HearingConfig);
	
	HearingConfig->HearingRange = HearingDistance;
	HearingConfig->DetectionByAffiliation = FAISenseAffiliationFilter();
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	ConfigureSense(*HearingConfig);
}




#pragma region Utility
void UAIPerceptionComponentBase::OnRegister()
{
	Super::Super::OnRegister();
	// Update SensesConfig
	Super::OnRegister();
}


bool UAIPerceptionComponentBase::GetCharacterInformation()
{
	if (Npc) return true;
	if (!GetCharacter()) return false;

	return true;
}


bool UAIPerceptionComponentBase::GetCharacter()
{
	if (Npc) return true;

	Npc = Cast<ANpc>(GetOwner());
	if (!Npc)
	{
		UE_LOGFMT(AIInformationLog, Error, "{0}::{1}() Failed to cast {2} to an npc character!",
			GetOwner() ? *UEnum::GetValueAsString(GetOwner()->GetLocalRole()) : *FString(""),
			*FString(__FUNCTION__), *GetNameSafe(GetOwner())
		);
		return false;
	}

	return true;
}
#pragma endregion
