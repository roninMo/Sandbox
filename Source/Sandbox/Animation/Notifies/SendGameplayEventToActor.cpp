// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Animation/Notifies/SendGameplayEventToActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Sandbox/Asc/AbilitySystem.h"

USendGameplayEventToActor::USendGameplayEventToActor()
{
}

void USendGameplayEventToActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}


FString USendGameplayEventToActor::GetNotifyName_Implementation() const
{
	if(EventTag.IsValid())
	{
		return FString("SGE: ").Append(EventTag.ToString());
	}

	return FString("Send A Gameplay Event!");
}
