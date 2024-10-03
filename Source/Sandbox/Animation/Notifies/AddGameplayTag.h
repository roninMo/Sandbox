// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyBase.h"
#include "GameplayTagContainer.h"
#include "AddGameplayTag.generated.h"

/**
 * Only adds loose gameplay tags to the character's state during an animation.
 * This is essential for accurately and dynamically handling things like attack frames and such, but there's some precautions and things you need to account for
 * AlwaysTickAndRefreshBones should be one if this is to also run on the server, and you need logic in place in the event that this montage is interrupted before the notify completes
 */
UCLASS(Blueprintable, meta = (DisplayName = "Add Gameplay Tag"))
class SANDBOX_API UAddGameplayTag : public UAnimNotifyBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The tag that's added for the duration of the notify state"))
	FGameplayTag TagState;


public:
	UAddGameplayTag();
	
	/** Logic that happens on the notify */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	
protected:
	/** Adjusts the name of the notify in the montage notify */
	virtual FString GetNotifyName_Implementation() const override;

	
};
