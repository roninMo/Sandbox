// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CharacterGameplayAbility.generated.h"

class UAbilitySystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityEnded, const UGameplayAbility*, Ability);


/**
 * Gameplay Ability
 *
 *	Abilities define custom gameplay logic that can be activated or triggered.
 *
 *	The main features provided by the AbilitySystem for GameplayAbilities are:
 *		-CanUse functionality:
 *			-Cooldowns
 *			-Costs (mana, stamina, etc)
 *			-etc
 *
 *		-Replication support
 *			-Client/Server communication for ability activation
 *			-Client prediction for ability activation
 *
 *		-Instancing support
 *			-Abilities can be non-instanced (native only)
 *			-Instanced per owner
 *			-Instanced per execution (default)
 *
 *		-Basic, extendable support for:
 *			-Input binding
 *			-'Giving' abilities (that can be used) to actors
 *
 *
 *	See GameplayAbility_Montage for an example of a non-instanced ability
 *		-Plays a montage and applies a GameplayEffect to its target while the montage is playing.
 *		-When finished, removes GameplayEffect.
 *
 *	Note on replication support:
 *		-Non instanced abilities have limited replication support.
 *			-Cannot have state (obviously) so no replicated properties
 *			-RPCs on the ability class are not possible either.
 *
 *	To support state or event replication, an ability must be instanced. This can be done with the InstancingPolicy property.
 *
 *	Everything here works however I'd still check if everything is valid
 */
UCLASS()
class SANDBOX_API UCharacterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	/* Adds gameplay effects during the ability deactivation. This helps with setting up and tearing down status tags and other effects you only want active during the ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability") TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToRemoveOnEnd;
	UPROPERTY(BlueprintReadWrite) TArray<FActiveGameplayEffectHandle> RemoveOnEndEffectHandles;

	/* Adds gameplay effects at the start of the ability activation. Use this for specific effects that are granted from this ability but not specific to the ability. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability") TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApplyOnStart;
	
	/** Does this ability need to delete its tasks at the end of the ability? If true you need to keep track of and add the tasks to the tracked tasks yourself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Tasks") bool bDeleteTasksOnEndOfAbility = false;
	UPROPERTY(BlueprintReadWrite) TArray<FName> TrackedTasks;

	/** A saved reference to the ability system component */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAbilitySystem> Asc;
	
	/** Prints debug messages about the ability. This is to just breakdown the ability to find out what's going on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Debug") bool bDebug = false;


public:
	/** Called when the ability ends. */
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FOnAbilityEnded OnAbilityEnded;
	
	
//------------------------------------------------------------------------------------------//
// Primary Functions																		//
//------------------------------------------------------------------------------------------//
public:
	UCharacterGameplayAbility(const FObjectInitializer& ObjectInitializer);
	
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/* Epic's comment: Projects should initiate passives or do other "BeginPlay" type of logic here. */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:
	/** Function for handling tags on a character at the end of an ability */
	UFUNCTION(BlueprintCallable) virtual void HandleTagsAtEndOfAbility();

	/** Ends all the tasks that are still active for this ability if bDeleteTasksOnEndOfAbility is set to true. */
	virtual void ClearTasksAtEndOfAbility();
	
	
//------------------------------------------------------------------------------------------//
// Exec Calc And Attribute Set Custom Information											//
//------------------------------------------------------------------------------------------//
protected:
	// This is the ApplyGameplayEffectSpecToTarget function but it's broken down into two functions because I want to have access to the effect before I pass it to the Asc
	// That way we're allowed to add attribute information before we do any of the calculations. This is not necessary unless it's an execution calculation effect
	// Override this function if you want to add custom attribute information to gameplay effects that you want to use during @ref PostGameplayEffectExecute() attribute calculations

	/**
	 * Non blueprintcallable, safe to call on CDO/NonInstance abilities
	 * This is the first part of ApplyGameplayEffectToTarget, and it passes the effect handle back before applying the effect to the Asc
	 * Subclass this in other abilities for calculations specific to that attack's configuration
	 * @returns FGameplayEffectSpecHandle 
	 */
	virtual FGameplayEffectSpecHandle PrepExecCalcForTarget(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		TSubclassOf<UGameplayEffect> ExecCalcClass,
		float GameplayEffectLevel = 1.f
	) const;
	
	/**
	 * Non blueprintcallable, safe to call on CDO/NonInstance abilities
	 * This is the second part of ApplyGameplayEffectToTarget, and it applies the effect to the Asc
	 * Subclass this in other abilities for calculations specific to that attack's configuration
	 * @returns TArray<FActiveGameplayEffectHandle> the list of active effect handles created from the effect handle
	 */
	virtual TArray<FActiveGameplayEffectHandle> ApplyExecCalcToTarget(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayAbilityTargetDataHandle& Target,
		const FGameplayEffectSpecHandle& ExecCalcHandle,
		int32 Stacks = 1
	) const;

	
	/**
	 * The blueprint version, safe to call on CDO/NonInstance abilities
	 * This is the first part of ApplyGameplayEffectToTarget, and it passes the effect handle back before applying the effect to the Asc
	 * Subclass this in other abilities for calculations specific to that attack's configuration
	 * @returns FGameplayEffectSpecHandle 
	 */
	UFUNCTION(BlueprintCallable) virtual FGameplayEffectSpecHandle BP_PrepExecCalcForTarget(TSubclassOf<UGameplayEffect> ExecCalcClass, float GameplayEffectLevel = 1);

	/**
	 * The blueprint version, safe to call on CDO/NonInstance abilities
	 * This is the second part of ApplyGameplayEffectToTarget, and it applies the effect to the Asc
	 * Subclass this in other abilities for calculations specific to that attack's configuration
	 * @returns TArray<FActiveGameplayEffectHandle> the list of active effect handles created from the effect handle
	 */
	UFUNCTION(BlueprintCallable) virtual TArray<FActiveGameplayEffectHandle> BP_ApplyExecCalcToTarget(const FGameplayAbilityTargetDataHandle& Target, const FGameplayEffectSpecHandle& ExecCalcHandle, int32 Stacks = 1);

	
//------------------------------------------------------------------------------------------//
// Input Evocations																			//
//------------------------------------------------------------------------------------------//
protected:
	/** Input released function that's called if the ability system input bind is released for this ability input tag. This is setup through the ability system bindings. */
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	/** Blueprint function for when the input is released for this attack. This is how to access the InputReleased function through blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "InputReleased", meta=(ScriptName = "InputReleased"))
	void K2_InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo ActivationInfo);
	

	/** Input pressed function that's called if the ability system input bind is released for this ability input tag. This is setup through the ability system bindings. */
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	/** Blueprint function for when the input is pressed for this attack. This is how to access the InputPressed function through blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "InputPressed", meta=(ScriptName = "InputPressed"))
	void K2_InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo ActivationInfo);
	
	
//------------------------------------------------------------------------------------------//
// Utility																					//
//------------------------------------------------------------------------------------------//
public:
	/** Add a loose gameplay tag to the character. This isn't safe to call in non replicated situations */
	UFUNCTION(BlueprintCallable) void AddGameplayTag(const FGameplayTag& Tag) const;
	
	/** Remove a loose gameplay tag to the character. This isn't safe to call in non replicated situations */
	UFUNCTION(BlueprintCallable) void RemoveGameplayTag(const FGameplayTag& Tag, const bool EveryTag = true) const;

	virtual void DebugActivateAbility();
	virtual void DebugEndAbility();
	
	/** Print a message and whether the character has authority or is able to do client side prediction */
	virtual void PrintInformation(const FString& Message) const;

	
};
