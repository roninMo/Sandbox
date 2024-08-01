// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Sandbox/Data/Structs/AISenseInformation.h"
// #include "GameplayTagContainer.h"
#include "AIControllerBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AIInformationLog, Log, All);

class ANpc;
class ACharacterBase;

class UAIPerceptionStimuliSourceComponent;
class UAttributeSet;
class UAbilitySystemComponent;
class UInventoryComponent;


/**
 * 
 */
UCLASS()
class SANDBOX_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

protected:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TObjectPtr<UAIPerceptionComponent> PerceptionComponent; // @ref AIController
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> PerceptionStimuli;

	/** The behavior tree of this character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	/** Behavior information */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<ACharacter> CurrentTarget;
	UPROPERTY(BlueprintReadWrite) FTimerHandle PlayerWithinCloseProximityHandle;
	UPROPERTY(BlueprintReadWrite) FTimerHandle PlayerWithinFarProximityHandle;
	UPROPERTY(BlueprintReadWrite) FTimerHandle PlayerExitedProximityHandle;

	/** Character information */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<ANpc> AICharacter;

	/** State information (These are tied to blackboard values) */
	UPROPERTY(BlueprintReadWrite) FName _SelfActor = FName("SelfActor");
	UPROPERTY(BlueprintReadWrite) FName _SpawnLocation = FName("SpawnLocation");
	UPROPERTY(BlueprintReadWrite) FName _SpawnRotation = FName("SpawnRotation");
	UPROPERTY(BlueprintReadWrite) FName _TargetActor = FName("TargetActor");

	// TODO: Add behavior for senses
	// TODO: Add complex ai behavior, and different ways to handle some of the behavior logic
	// TODO: Add sense buildup instead of events (or both)
	
	/** Other */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug") bool bDebugAIPerception;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug") bool bDebugSenseSight;

	
public:
	AAIControllerBase(const FObjectInitializer& ObjectInitializer);
	

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Starts another behavior tree during play, and if save values is set to true, saves the current behavior tree's values */
	UFUNCTION(BlueprintCallable, Category = "AI") virtual void StartBehaviorTree(UBehaviorTree* BTAsset, bool bSaveValues = true);

	/** Sets the behavior tree's dynamic behavior sub tree */
	// UFUNCTION(BlueprintCallable, Category = "AI") virtual void SetDynamicBehaviorSubTree(const FGameplayTag& BehaviorTreeTag, UBehaviorTree* BTAsset);

	
//--------------------------------------------------------------------------------------//
// Senses																				//
//--------------------------------------------------------------------------------------//
protected:
	/**
	 * Routes each of the senses to the perception sense functions
	 * 
	 * @note Bind this to the AIPerception's OnPerceptionUpdated in the blueprint
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Events") void OnPerceptionUpdated(UPARAM(ref) TArray<AActor*>& Actors);
	virtual void OnPerceptionUpdated_Implementation(TArray<AActor*>& Actors);
	
	/**
	 * Returns Perception information when it senses something
	 * 
	 * @note Bind this to the AIPerception's OnPerceptionInfoUpdated in the blueprint
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Events") void OnTargetPerceptionUpdated(FActorPerceptionUpdateInfo UpdateInformation);
	virtual void OnTargetPerceptionUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation);

	/**
	 * Forgets a character once it's successfully unsensed
	 * 
	 * @note this doesn't work unless OnTargetPerceptionUpdated delegate binding is linked to AIPerception's OnPerceptionInfoUpdated in the blueprint
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Events") void OnTargetPerceptionForgotten(AActor* Actor);
	virtual void OnTargetPerceptionForgotten_Implementation(AActor* Actor);

	/** Sight sense updated */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Events") void OnSenseSightUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);
	virtual void OnSenseSightUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);

	/** Team sense updated */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Events") void OnSenseTeamUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);
	virtual void OnSenseTeamUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);
	
	/** Hearing sense updated */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Events") void OnSenseHearingUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);
	virtual void OnSenseHearingUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);
	
	/** Prediction sense updated */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Events") void OnSensePredictionSenseUpdated(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);
	virtual void OnSensePredictionSenseUpdated_Implementation(FActorPerceptionUpdateInfo UpdateInformation, AActor* Target);
	
	// TODO: I don't know if keeping track of the actors during individual senses is necessary, sense each target might activate multiple senses and how that overlaps with each of these delegate bindings (learn how the perception component stores this information)
	// UAIPerceptionComponent::GetPerceivedHostileActors(TArray<AActor*>& OutActors) const;
	// UAIPerceptionComponent::GetPerceivedHostileActorsBySense(const TSubclassOf<UAISense> SenseToUse, TArray<AActor*>& OutActors) const;
	// UAIPerceptionComponent::GetCurrentlyPerceivedActors(TSubclassOf<UAISense> SenseToUse, TArray<AActor*>& OutActors) const; /** If SenseToUse is none all actors currently perceived in any way will get fetched */
	// UAIPerceptionComponent::GetKnownPerceivedActors(TSubclassOf<UAISense> SenseToUse, TArray<AActor*>& OutActors) const; /** If SenseToUse is none all actors ever perceived in any way (and not forgotten yet) will get fetched */
	// UAIPerceptionComponent::GetActorsPerception(AActor* Actor, FActorPerceptionBlueprintInfo& Info); /** Retrieves whatever has been sensed about given actor */
	// UAIPerceptionComponent::SetSenseEnabled(TSubclassOf<UAISense> SenseClass, const bool bEnable); /** Note that this works only if given sense has been already configured for this component instance */
	

//--------------------------------------------------------------------------------------//
// Get and Set functions																//
//--------------------------------------------------------------------------------------//
public:
	/** Blackboard get and set functions */
	UFUNCTION(BlueprintCallable, Category = "AI|Character") virtual ANpc* GetSelfActor() const;
	UFUNCTION(BlueprintCallable, Category = "AI|Character") virtual FVector GetSpawnLocation() const;
	UFUNCTION(BlueprintCallable, Category = "AI|Character") virtual FRotator GetSpawnRotation() const;
	
	UFUNCTION(BlueprintCallable, Category = "AI|Character") virtual void SetSelfActor(ANpc* SelfActor);
	UFUNCTION(BlueprintCallable, Category = "AI|Character") virtual void SetSpawnLocation(FVector SpawnLocation);
	UFUNCTION(BlueprintCallable, Category = "AI|Character") virtual void SetSpawnRotation(FRotator SpawnRotation);
	
	/** Retrieved owner attitude toward given Other character */
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
	/** Character information */
	UFUNCTION(BlueprintCallable) virtual bool GetCharacterInformation();
	UFUNCTION(BlueprintCallable) virtual bool GetCharacter();
	
	/** Asc */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	virtual UAttributeSet* GetAttributeSet() const;
	
	
};
