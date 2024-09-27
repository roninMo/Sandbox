// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Movement/MovementAbility.h"

#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"

UAdvancedMovementComponent* UMovementAbility::GetMovementComponent(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(Actor);
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Failed to retrieve the movement component from an invalid character class! {2}",
			*UEnum::GetValueAsString(Actor->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Actor));
		return nullptr;
	}

	return Character->GetAdvancedMovementComp();
}
