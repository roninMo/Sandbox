// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Animation/Notifies/AddGameplayTag.h"

#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Asc/AbilitySystem.h"

UAddGameplayTag::UAddGameplayTag()
{
	bShouldFireInEditor = false;
}


void UAddGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ACharacterBase* Character;
	UAbilitySystem* AbilitySystem;
	if (!GetCharacterAndAbilitySystem(MeshComp, Character, AbilitySystem) || !TagState.IsValid())
	{
		return;
	}

	AbilitySystem->AddLooseGameplayTag(TagState);
	AbilitySystem->AddReplicatedLooseGameplayTag(TagState);
}


FString UAddGameplayTag::GetNotifyName_Implementation() const
{
	if(TagState.IsValid())
	{
		return TagState.ToString();
	}

	return FString("Add Gameplay Tag");
}

