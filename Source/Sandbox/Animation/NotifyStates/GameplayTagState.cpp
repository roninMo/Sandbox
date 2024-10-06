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
	if (!GetCharacterAndAbilitySystem(MeshComp, Character, AbilitySystem) || !TagState.IsValid()) // TODO: Find out if this is valid (technically this should be a const function, and this might cause problems)
	{
		return;
	}

	// UE_LOGFMT(LogTemp, Warning, "{0}: {1} added to {2}_{3}", *UEnum::GetValueAsString(Character->GetLocalRole()), *TagState.ToString(), *GetNameSafe(Character), Character->CharacterId);
	if (bAddGameplayTagToActor)
	{
		AbilitySystem->AddReplicatedLooseGameplayTag(TagState);
	}

	
	if (bSendGameplayEventToActor)
	{
		// if (!Character->HasAuthority() && Cast<ANPC>(Character)) return; // Do not activate on npc clients
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, TagState, FGameplayEventData());
	}
}


void UGameplayTagState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	UAbilitySystem* AbilitySystem;
	ACharacterBase* Character;
	if (!GetCharacterAndAbilitySystem(MeshComp, Character, AbilitySystem) || !TagState.IsValid())
	{
		return;
	}
	
	if (bAddGameplayTagToActor && AbilitySystem->HasMatchingGameplayTag(TagState))
	{
		AbilitySystem->RemoveReplicatedLooseGameplayTag(TagState);
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
