// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Animation/NotifyStates/GameplayTagState.h"

#include "Sandbox/Characters/CharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Sandbox/Asc/AbilitySystem.h"


UGameplayTagState::UGameplayTagState()
{
	bShouldFireInEditor = false;
}


void UGameplayTagState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UAbilitySystem* AbilitySystem;
	ACharacterBase* Character;
	if (!GetCharacterAndAbilitySystem(MeshComp, Character, AbilitySystem)) // TODO: Find out if this is valid (technically this should be a const function, and this might cause problems)
	{
		return;
	}

	// UE_LOGFMT(LogTemp, Warning, "{0}: {1} added to {2}_{3}", *UEnum::GetValueAsString(Character->GetLocalRole()), *TagState.ToString(), *GetNameSafe(Character), Character->CharacterId);

	// Gameplay tag state
	if (bAddGameplayTagToActor)
	{
		AbilitySystem->AddReplicatedLooseGameplayTag(GameplayTagState);
	}

	// Gameplay event state 
	if (bNotifyBeginSendGameplayEventToActor && NotifyBeginGameplayEventTag.IsValid())
	{
		FGameplayEventData EventData;
		EventData.Instigator = Character;
		
		// if (!Character->HasAuthority() && Cast<ANPC>(Character)) return; // Do not activate on npc clients
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, NotifyBeginGameplayEventTag, EventData);
	}
}


void UGameplayTagState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	UAbilitySystem* AbilitySystem;
	ACharacterBase* Character;
	if (!GetCharacterAndAbilitySystem(MeshComp, Character, AbilitySystem))
	{
		return;
	}

	// Gameplay tag state
	if (bAddGameplayTagToActor && GameplayTagState.IsValid() && AbilitySystem->HasMatchingGameplayTag(GameplayTagState))
	{
		AbilitySystem->RemoveReplicatedLooseGameplayTag(GameplayTagState);
	}
	
	// Gameplay event state 
	if (bNotifyEndSendGameplayEventToActor && NotifyEndGameplayEventTag.IsValid())
	{
		FGameplayEventData EventData;
		EventData.Instigator = Character;
		
		// if (!Character->HasAuthority() && Cast<ANPC>(Character)) return; // Do not activate on npc clients
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, NotifyEndGameplayEventTag, EventData);
	}
}


FString UGameplayTagState::GetNotifyName_Implementation() const
{
	if(TagState.IsValid())
	{
		return TagState.ToString();
	}

	return FString("Add Gameplay Tag");
}
