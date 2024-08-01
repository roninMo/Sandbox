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
	AdjustSightSense(Npc->GetSenseConfig().SightSenseConfig);
	SetDominantSense(UAISense_Sight::StaticClass());
	
	// Team
	UAISenseConfig_Team* TeamConfig = NewObject<UAISenseConfig_Team>(this, UAISenseConfig_Team::StaticClass(), TEXT("UAISenseConfig_Team"));
	ConfigureSense(*TeamConfig);

	// Hearing
	AdjustHearingSense(Npc->GetSenseConfig().HearingSenseConfig);

	// Prediction
	UAISenseConfig_Prediction* PredictionConfig = NewObject<UAISenseConfig_Prediction>(this, UAISenseConfig_Prediction::StaticClass(), TEXT("UAISenseConfig_Prediction"));
	ConfigureSense(*PredictionConfig);
	ConfigureSense(*PredictionConfig);
	
	RequestStimuliListenerUpdate();
}


void UAIPerceptionComponentBase::AdjustSightSense(const F_AISightSenseConfig& Information)
{
	UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>(this, UAISenseConfig_Sight::StaticClass(), TEXT("UAISenseConfig_Sight"));
	check(SightConfig);
	
	SightConfig->SightRadius = Information.SightRadius;
	SightConfig->LoseSightRadius = Information.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Information.PeripheralVisionAngleDegrees;
	SightConfig->DetectionByAffiliation = FAISenseAffiliationFilter();
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = Information.AutoSuccessRangeFromLastSeenLocation;
	SightConfig->PointOfViewBackwardOffset = Information.PointOfViewBackwardOffset;
	SightConfig->NearClippingRadius = Information.NearClippingRadius;
	ConfigureSense(*SightConfig);
}


void UAIPerceptionComponentBase::AdjustTeamSense()
{
	UAISenseConfig_Team* TeamConfig = NewObject<UAISenseConfig_Team>(this, UAISenseConfig_Team::StaticClass(), TEXT("UAISenseConfig_Team"));
	ConfigureSense(*TeamConfig);
}


void UAIPerceptionComponentBase::AdjustPredictionSense()
{
	UAISenseConfig_Prediction* PredictionConfig = NewObject<UAISenseConfig_Prediction>(this, UAISenseConfig_Prediction::StaticClass(), TEXT("UAISenseConfig_Prediction"));
	ConfigureSense(*PredictionConfig);
}


void UAIPerceptionComponentBase::AdjustDamageSense()
{
	UAISenseConfig_Damage* DamageConfig = NewObject<UAISenseConfig_Damage>(this, UAISenseConfig_Damage::StaticClass(), TEXT("UAISenseConfig_Damage"));
	ConfigureSense(*DamageConfig);
}


void UAIPerceptionComponentBase::AdjustHearingSense(const F_AIHearingSenseConfig& Information)
{
	UAISenseConfig_Hearing* HearingConfig = NewObject<UAISenseConfig_Hearing>(this, UAISenseConfig_Hearing::StaticClass(), TEXT("UAISenseConfig_Hearing"));
	check(HearingConfig);
	
	HearingConfig->HearingRange = Information.HearingRange;
	HearingConfig->DetectionByAffiliation = FAISenseAffiliationFilter();
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	ConfigureSense(*HearingConfig);
}




#pragma region Utility
void UAIPerceptionComponentBase::OnRegister()
{
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
