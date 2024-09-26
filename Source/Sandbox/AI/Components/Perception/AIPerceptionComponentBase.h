// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "Sandbox/AI/Controllers/AIControllerBase.h"
#include "Sandbox/Data/Structs/AISenseInformation.h"
#include "AIPerceptionComponentBase.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UAIPerceptionComponentBase : public UAIPerceptionComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<ANpc> Npc;


public:
	UAIPerceptionComponentBase(const FObjectInitializer& ObjectInitializer);
	
	
//--------------------------------------------------------------------------------------//
// Senses																				//
//--------------------------------------------------------------------------------------//
protected:
	/**
	 * Adds the character's senses. The blueprint values of this component handle what values are added and the configuration of it's senses. The reason it's handled here is to adjust this during runtime
	 *
	 * @note Having the senses configured here isn't actually required, the only configuration that's different is the GenericAgentInterface that's required on character components for reference
	 */
	UFUNCTION(BlueprintCallable) virtual void InitSenses();

	/** Adjust sight config */
	UFUNCTION(BlueprintCallable) virtual void AdjustSightSense(const F_AISightSenseConfig& Information);

	/** Adjust team config */
	UFUNCTION(BlueprintCallable) virtual void AdjustTeamSense();

	/** Adjust hearing config */
	UFUNCTION(BlueprintCallable) virtual void AdjustHearingSense(const F_AIHearingSenseConfig& Information);

	/** Adjust prediction config */
	UFUNCTION(BlueprintCallable) virtual void AdjustPredictionSense();

	/** Adjust damage sense config */
	UFUNCTION(BlueprintCallable) virtual void AdjustDamageSense();

	
//--------------------------------------------------------------------------------------//
// Utility																				//
//--------------------------------------------------------------------------------------//
protected:
	virtual void OnRegister() override;
	UFUNCTION() virtual bool GetCharacterInformation();
	UFUNCTION() virtual bool GetCharacter();
	
};
