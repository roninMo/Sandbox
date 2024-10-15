// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sandbox/AI/Controllers/AIControllerBase.h"
#include "EnemyController.generated.h"

class UAbilitySystem;
class UAttributeLogic;
/**
 * 
 */
UCLASS()
class SANDBOX_API AEnemyController : public AAIControllerBase
{
	GENERATED_BODY()

protected:
	/** A stored reference to the character's ability system component */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAbilitySystem> AbilitySystemComponent;

	/** A stored reference to the character's attributes */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAttributeLogic> AttributeSet;


public:
	AEnemyController(const FObjectInitializer& ObjectInitializer);

	/** Retrieves the ability system component */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	/** Retrieves the character's attributes */
	virtual UAttributeSet* GetAttributeSet() const override;
	
	/** Templated convenience version for retrieving the ability system component. */
	template<class T> T* GetAbilitySystem(void) const { return Cast<T>(GetAbilitySystemComponent()); }

	/** Templated convenience version for getting the attribute set. */
	template<class T> T* GetAttributeSet(void) const { return Cast<T>(AttributeSet); }
	

protected:
	/**
	 * Routes each of the senses to the perception sense functions
	 * 
	 * @note Bind this to the AIPerception's OnPerceptionUpdated in the blueprint
	 */
	virtual void OnPerceptionUpdated_Implementation(TArray<AActor*>& Actors);
	
	/**
	 * Returns Perception information when it senses something
	 * 
	 * @note Bind this to the AIPerception's OnPerceptionInfoUpdated in the blueprint
	 */
	virtual void OnTargetPerceptionUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation);

	/**
	 * Forgets a character once it's successfully unsensed
	 * 
	 * @note this doesn't work unless OnTargetPerceptionUpdated delegate binding is linked to AIPerception's OnPerceptionInfoUpdated in the blueprint
	 */
	virtual void OnTargetPerceptionForgotten_Implementation(AActor* Actor);

	
};
