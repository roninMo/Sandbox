// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyStateBase.h"
#include "GameplayTagContainer.h"
#include "GameplayTagState.generated.h"

/**
 * Adds and removes loose gameplay tags to the character's state during an animation.
 * 
 * This is essential for accurately and dynamically handling things like attack frames and such, but there's some precautions and things you need to account for
 * AlwaysTickAndRefreshBones should be one if this is to also run on the server, and you need logic in place in the event that this montage is interrupted before the notify completes
 * @note If you're using combat animations you need to enable AlwaysTickAndRefresh the character skeleton on the character and the skeleton for animations to occur on the server for multiplayer
 */
UCLASS(Blueprintable, meta = (DisplayName = "Gameplay Tag State"))
class SANDBOX_API UGameplayTagState : public UAnimNotifyStateBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The tag that's added for the duration of the notify state"))
	FGameplayTag TagState;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Send gameplay event to player actors (and server actors)"))
	bool bSendGameplayEventToActor;


public:
	UGameplayTagState();
	
	/** Logic that happens at the beginning of the notify */
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	/** Logic that happens at the end of the notify. This occurs even if the character early outs. */
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;


protected:
	/** Adjusts the name of the notify in the montage notify */
	virtual FString GetNotifyName_Implementation() const override;

	
};
