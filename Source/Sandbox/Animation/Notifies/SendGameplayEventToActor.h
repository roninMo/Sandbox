// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Sandbox/Animation/Notifies/AnimNotifyBase.h"
#include "SendGameplayEventToActor.generated.h"

/**
 * Sends a gameplay event to the character. Helpful for specific state / actions during gameplay abilities
 */
UCLASS(Blueprintable, meta = (DisplayName = "Send Gameplay Event"))
class SANDBOX_API USendGameplayEventToActor : public UAnimNotifyBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The tag for the gameplay event that's sent to the character"))
	FGameplayTag EventTag;


public:
	USendGameplayEventToActor();
	
	/** Logic that happens on the notify */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	
protected:
	/** Adjusts the name of the notify in the montage notify */
	virtual FString GetNotifyName_Implementation() const override;

	
};
