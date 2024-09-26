// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/AI/Characters/Npc.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API AEnemy : public ANpc
{
	GENERATED_BODY()

protected:
	

public:
	AEnemy(const FObjectInitializer& ObjectInitializer);
	

protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;
	
	/** 
	 * Called when this Pawn is possessed. Only called on the server (or in standalone).
	 * @param NewController The controller possessing this pawn
	 */
	virtual void PossessedBy(AController* NewController) override;

	
//--------------------------------------------------------------------------------------------------------------------------//
// OnRepPlayerState/PossessedBy -> Or AI PossessedBy -> To this initialization loop											//
//--------------------------------------------------------------------------------------------------------------------------//
	/** Initialize character components -> Get access to all the pointers, nothing else */
	virtual void InitCharacterComponents(const bool bCalledFromPossessedBy) override;

	/** Init ability system and attributes -> These are the most important components that tie everything together */
	virtual void InitAbilitySystem(const bool bCalledFromPossessedBy) override;
	
	/** InitCharacterInformation -> Run any logic necessary for the start of any of the components */
	virtual void InitCharacterInformation() override;

	/** Blueprint logic for character information initialization logic */
	// UFUNCTION(BlueprintImplementableEvent, DisplayName = "Init Character Information (Blueprint)") void BP_InitCharacterInformation();

	
};
