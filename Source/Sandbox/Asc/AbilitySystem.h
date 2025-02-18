// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitiyUtilities.h"
#include "Sandbox/Data/Structs/AbilityInformation.h"
#include "Components/InputComponent.h"
#include "AbilitySystem.generated.h"

class UInputAction;
class AAbilitySystemController;


DECLARE_LOG_CATEGORY_EXTERN(AbilityLog, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AbilityTagLog, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AbilityPredictionKeyLog, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AscDebugInformation, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AscGameplayEffectLog, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AbilityAnimLog, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AbilityFailedLog, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AbilityTaskLog, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(AttributeLog, Verbose, All);

/* 
| Verbosity Level | Printed in Console? | Printed in Editor's Log? |                      Notes                       |
|-----------------|---------------------|--------------------------|--------------------------------------------------|
| Fatal           | Yes                 | N/A                      | Crashes the session, even if logging is disabled |
| Error           | Yes                 | Yes                      | Log text is coloured red                         |
| Warning         | Yes                 | Yes                      | Log text is coloured yellow                      |
| Display         | Yes                 | Yes                      | Log text is coloured grey                        |
| Log             | No                  | Yes                      | Log text is coloured grey                        |
| Verbose         | No                  | No                       |                                                  |
| VeryVerbose     | No                  | No                       |                                       
*/


DECLARE_MULTICAST_DELEGATE_OneParam(FOnGiveAbility, FGameplayAbilitySpec&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitAbilityActorInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityActivated, const UGameplayAbility*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCommit, UGameplayAbility*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityFailed, const UGameplayAbility*, Ability, const FGameplayTagContainer&, ReasonTags);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectAdded, FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnGameplayEffectStackChange, FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle, int32, NewStackCount, int32, OldStackCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnGameplayEffectTimeChange,  FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle, float, NewStartTime, float, NewDuration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectRemoved, FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayTagStackChange, FGameplayTag, GameplayTag, int32, NewTagCount);


/**
 * An object for the ability system to handle retrieving multiple abilities and adding/removing an ability without it affecting gameplay.  
 * TODO: There's a strange way of handling multiple abilities of the same kind that's level based and needs to be fixed specific to the current weapon
 */
USTRUCT()
struct F_MultiAbilityHandle
{
	GENERATED_USTRUCT_BODY()
	F_MultiAbilityHandle() = default;

	/** The current ability that's added to the character */
	UPROPERTY() FGameplayAbilitySpec Ability;

	/** The handle of the ability that's applied to the character */
	UPROPERTY() FGameplayAbilitySpecHandle AbilityHandle;

	/** The currently activated instance of the granted ability */
	UPROPERTY() FGuid CurrentlyActivatedAbility;
	
	/** The different instances that have granted the ability to the player */
	UPROPERTY() TMap<FGuid, FGameplayAbilityInfo> GrantedAbilities;
};


///// Ability System /////
// "showdebug abilitysystem"
// "AbilitySystem.Debug.NextCategory"
// NextDebugTarget or PgUpKey 


/** The core ActorComponent for interfacing with the GameplayAbilities System
 *	This inherits from multiple classes
 *		- GameplayTasksComponent
 *		- AbilitySystemComponent
 *		- AbilitySystemComponent_Abilities
 *
 *		TODO: Update to version 5.3 and use GameplayComponents
 */
UCLASS()
class SANDBOX_API UAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	/** Cached granted Ability Handles */
	UPROPERTY(Transient)
	TMap<TSubclassOf<UGameplayAbility>, F_MultiAbilityHandle> AddedAbilityHandles;

	/** Cached granted AttributeSets */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UAttributeSet>> AddedAttributes;

	/** Cached applied Startup Effects */
	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> AddedEffects;
	
	/** Cached granted Ability Sets */
	// UPROPERTY(Transient)
	// TArray<FCharacterAbilityDataSetHandle> AddedAbilitySets;
	
	// Cached input actions linked to abilities using Input bind handle and input id
	TArray<FInputActionBindingAbilityMap> AbilityInputActions_Old;
	FInputActionBinding LocalConfirmInput_Old;
	FInputActionBinding LocalCancelInput_Old;
	TArray<int32> RegisteredAbilityInputHandles;

	/** Array of gameplay effect handles */
	TArray<FActiveGameplayEffectHandle> GameplayEffectAddedHandles;

	
public:
	/** A reference to the player's saved attribute information */
	UPROPERTY(Transient) TObjectPtr<UGameplayEffect> SavedAttributes;

	
public:
	//~ End UActorComponent interface
	virtual void BeginPlay() override;
	//~ End UActorComponent interface

	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	/**
	 * Initialized the Abilities' ActorInfo - the structure that holds information about who we are acting on and who controls us. \n\n
	 * 
	 * Invoked multiple times for both client / server, also depends on whether the Ability System Component lives on Pawns or Player States:
	 *		- Once for Server after component initialization
	 *		- Once for Server after replication of owning actor (Possessed by for Player State)
	 *		- Once for Client after component initialization
	 *		- Once for Client after replication of owning actor (Once more for Player State OnRep_PlayerState)
	 * 
	 * @param InOwnerActor			Is the actor that logically owns this component.
	 * @param InAvatarActor			Is what physical actor in the world we are acting on. Usually a Pawn but it could be a Tower, Building, Turret, etc, may be the same as Owner
	 */
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	/** Register Ability System delegates to mainly broadcast blueprint assignable event to BPs */
	void RegisterAbilitySystemDelegates();

	/** Clean up any bound delegates to Ability System delegates */
	void ShutdownAbilitySystemDelegates();

	
//------------------------------------------------------------------------------------------//
// Ability System Component Primary Functions												//
//------------------------------------------------------------------------------------------//
	/** These are just convenience functions for safely adding/removing abilities/attributes/effects during gameplay, with infrastructure for creating and adding abilities for different games. Just takes a second to construct */
public:
	/**
	 * Creates an ability, adds it to the character, and stores the reference to the player's added abilities
	 * This will be ignored if the actor is not authoritative.
	 * 
	 * @param NewAbility		Gameplay Ability Mapping containing information about the ability class, level and input ID to bind it to
	 * @returns						handle that can be used in TryActivateAbility, etc
	 */
	UFUNCTION(BlueprintCallable)
	virtual FGameplayAbilitySpecHandle AddAbility(const FGameplayAbilityInfo& NewAbility);
	
	/**
	 * Creates the abilities, adds them to the character, and stores the references to the player's added abilities
	 * This will be ignored if the actor is not authoritative.
	 * 
	 * @param NewAbilities			An array of Gameplay Ability Mappings containing information about the ability class, level and input ID to bind it to
	 * @returns						the ability handles that can be used in TryActivateAbility, etc
	 */
	UFUNCTION(BlueprintCallable)
	virtual TArray<FGameplayAbilitySpecHandle> AddAbilities(const TArray<FGameplayAbilityInfo> NewAbilities);

	/**
	 * Searches for an ability that's already been granted to the player using the class reference
	 *
	 * @param GameplayAbility		The ability we're searching for
	 * @returns A gameplay ability spec if there is one
	 */
	UFUNCTION()
	virtual FGameplayAbilitySpec GetAbilitySpec(TSubclassOf<UGameplayAbility> GameplayAbility);
	
	/** 
	 * Removes the specified ability from the character and the stored added ability references
	 * 
	 * @param Handle				Ability Spec Handle of the ability we want to remove
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RemoveGameplayAbility(const FGameplayAbilitySpecHandle& Handle);
	
	/** 
	 * Removes the specified ability from the character and the stored added ability references
	 * 
	 * @param Handles				Ability Spec Handles of the abilities we want to remove
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RemoveGameplayAbilities(const TArray<FGameplayAbilitySpecHandle> Handles);
	
	
	/**
	 * Adds a gameplay effect to the character, and stores a reference to the player's added effects
	 * 
	 * @param EffectMapping			Gameplay Effect Mapping containing the gameplay effect class and level of the effect
	 * @returns						FActiveGameplayEffectHandle of the created gameplay effect
	 */
	UFUNCTION(BlueprintCallable)
	virtual FActiveGameplayEffectHandle AddGameplayEffect(const FGameplayEffectInfo& EffectMapping);
	
	/**
	 * Adds gameplay effects to the character, and stores the references to the player's added effects
	 * 
	 * @param EffectMappings		Gameplay Effect Mappings containing the gameplay effect class and level of the effect
	 * @returns						the active effect handles of the created gameplay effects
	 */
	UFUNCTION(BlueprintCallable)
	virtual TArray<FActiveGameplayEffectHandle> AddGameplayEffects(const TArray<FGameplayEffectInfo> EffectMappings);
	
	/** 
	 * Removes the specified ability from the character and the stored added ability references
	 * 
	 * @param Handle				The gameplay effect handle that we want to remove
	 * @param StacksToRemove		The number of stacks of the gameplay effect we want to remove
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool RemoveGameplayEffect(const FActiveGameplayEffectHandle& Handle, int32 StacksToRemove = -1);
	
	
	/** 
	 * Attempts to activate a single gameplay ability that has the given tag and DoesAbilitySatisfyTagRequirements().
	 * Returns true if anything attempts to activate.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate abilities.
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities") bool TryActivateAbilityByTag(const FGameplayTag Tag, bool bAllowRemoteActivation = true);
	
	/**
	 * Bind to an input component with customized bindings
	 *
	 * @param InputComponent		The enhanced input component we're adding player input to
	 * @param AbilityInputActions	The input action information to for the different input binds 
	 */
	UFUNCTION(Blueprintable, Category = "Abilities|Input") virtual void BindAbilityActivationToEnhancedInput(UEnhancedInputComponent* InputComponent, const TArray<FInputActionAbilityMap>& AbilityInputActions);
	
	//~ Begin UAbilitySystemComponent interface
	/**
	 * Bind to an input component with customized bindings
	 *
	 * @param InputComponent		The input component we're adding player input to
	 * @param BindInfo				The input bind information, an enum used to create input actions and their index that we use to link the Ability's Input to 
	 */
	virtual void BindAbilityActivationToInputComponent(UInputComponent* InputComponent, FGameplayAbilityInputBinds BindInfo) override;
	
	/**
	 * Input binding for activating abilities and their input events using @ref BindAbilityActivationToInputComponent()
	 * It checks if there's local input pressed/released event delegates and adds them if so, then it loops through the abilities and invoke's their input events or activates their ability 
	 */
	virtual void AbilityLocalInputPressed(int32 InputID) override;

	/**
	 * Input binding for activated abilities input events using @ref BindAbilityActivationToInputComponent()
	 * Activates input released events for specific abilities
	 */
	virtual void AbilityLocalInputReleased(int32 InputID) override;
	//~ End UAbilitySystemComponent interface

	
	
	
//------------------------------------------------------------------------------------------//
// Ability Events (For handling certain logic during specific events)						//
//------------------------------------------------------------------------------------------//
public:
	/**
	 * Event called just after InitAbilityActorInfo, once abilities and attributes have been granted. \n\n
	 *
	 * Invoked multiple times for both client / server, also depends on whether the Ability System Component lives on Pawns or Player States:
	 *		- Once for Server after component initialization
	 *		- Once for Server after replication of owning actor (Possessed by for Player State)
	 *		- Once for Client after component initialization
	 *		- Once for Client after replication of owning actor (Once more for Player State OnRep_PlayerState)
	 *
	 */
	UPROPERTY(BlueprintAssignable, Category="Abilities")
	FOnInitAbilityActorInfo OnInitAbilityActorInfo;

	/**
	 * Specifically set abilities to persist across deaths / respawns or possessions.
	 *
	 * When this is set to false, abilities will only be granted the first time InitAbilityActor is called. This is the default
	 * behavior for ASC living on Player States (GSCModularPlayerState specifically).
	 *
	 * Do not set it true for ASC living on Player States if you're using ability input binding. Only ASC living on Pawns supports this.
	 * 
	 * (Default is true)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	bool bResetAbilitiesOnSpawn = true;

	/** Delegate invoked OnGiveAbility (when an ability is granted and available) */
	FOnGiveAbility OnGiveAbilityDelegate;


	
	/** Triggered by ASC when GEs are added */
	virtual void OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	/** Triggered by ASC when GEs stack count changes */
	virtual void OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount);

	/** Triggered by ASC when GEs stack count changes */
	virtual void OnActiveGameplayEffectTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration);

	/** Triggered by ASC when any GEs are removed */
	virtual void OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& EffectRemoved);

	/**
	* Called when a GameplayEffect is added or removed.
	*/
	UPROPERTY(BlueprintAssignable, Category="Abilities")
	FOnGameplayEffectStackChange OnGameplayEffectStackChange;

	/**
	* Called when a GameplayEffect duration is changed (for instance when duration is refreshed)
	*/
	UPROPERTY(BlueprintAssignable, Category="Abilities")
	FOnGameplayEffectTimeChange OnGameplayEffectTimeChange;

	/**
	* Called when a GameplayEffect is added.
	*/
	UPROPERTY(BlueprintAssignable, Category="Abilities")
	FOnGameplayEffectAdded OnGameplayEffectAdded;

	/**
	* Called when a GameplayEffect is removed.
	*/
	UPROPERTY(BlueprintAssignable, Category="Abilities")
	FOnGameplayEffectRemoved OnGameplayEffectRemoved;

	
	
	
//------------------------------------------------------------------------------------------//
// Utility																					//
//------------------------------------------------------------------------------------------//
public:
	/** Adds a loose gameplay tag that isn't replicated */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Loose Gameplay Tag"))
	virtual void K2_AddLooseGameplayTag(FGameplayTag Tag);
	
	/** Adds a gameplay tag that's replicated */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Replicated Loose Gameplay Tag"))
	virtual void K2_AddReplicatedLooseGameplayTag(FGameplayTag Tag);
	
	/** Removes a loose gameplay tag */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Loose Gameplay Tag"))
	virtual void K2_RemoveLooseGameplayTag(FGameplayTag Tag);
	
	/** Removes gameplay tag and replicates it to the client */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Replicated Loose Gameplay Tag"))
	virtual void K2_RemoveReplicatedLooseGameplayTag(FGameplayTag Tag);


protected:
	/** Reinit the cached ability actor info (specifically the player controller) */
	UFUNCTION()
	void OnPawnControllerChanged(APawn* Pawn, AController* NewController);
	
	
};


/**
	Ability System Component Event Functions  @ref GSCCoreComponent / AbilitySystemComponent
		- OnInitAbilityActorInfo()

		// Gameplay Abilities
		- AbilityActivatedCallbacks(FGenericAbilityDelegate, UGameplayAbility*)
		- AbilityCommittedCallbacks(FGenericAbilityDelegate, UGameplayAbility*)
		- AbilityEndedCallbacks(FAbilityEnded, UGameplayAbility*)
		- AbilityFailedCallbacks(FAbilityFailedDelegate, const UGameplayAbility*, const FGameplayTagContainer&)

		// Gameplay Effects
		- OnActiveGameplayEffectAddedDelegateToSelf(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, const FActiveGameplayEffectHandle ActiveHandle)
		- OnAnyGameplayEffectRemovedDelegate(const FActiveGameplayEffect& EffectRemoved)
		
		// Returns delegate structure that allows binding to several gameplay effect changes
		FActiveGameplayEffectEvents* GetActiveEffectEventSet(FActiveGameplayEffectHandle Handle);
		FOnActiveGameplayEffectRemoved_Info* OnGameplayEffectRemoved_InfoDelegate(FActiveGameplayEffectHandle Handle);
		FOnActiveGameplayEffectStackChange* OnGameplayEffectStackChangeDelegate(FActiveGameplayEffectHandle Handle);
		FOnActiveGameplayEffectTimeChange* OnGameplayEffectTimeChangeDelegate(FActiveGameplayEffectHandle Handle);
		// Retrieving the delegate during activation and binding to one of these events for updates, or during creation

		// Other
		- RegisterGenericGameplayTagEvent(const FGameplayTag GameplayTag, const int32 NewCount)
		- OnGiveAbilityDelegate(FGameplayAbilitySpec&, UGSCAbilityInputBindingComponent*, UInputAction*, EGSCAbilityTriggerEvent, FGameplayAbilitySpec)
*/


// Ability System Component
	// Test if abilities support multiple montages being played through client prediction
		// - Already done, works with ai, you just need to invoke the montage again (they're very time sensitive, still need to test in combination with asynchronous input events)

	// Test the optional objects passed through gameplay events

	// Subclass the gameplay cue manager and learn gameplay cues
		// - InitGameplayCueParameters
		// - UGameplayCueManager::CheckForTooManyRPCs
	
		
	// Start using tags to classify specific abilities, for both evocation and invocation
	/** 
	 * Gets all Activatable Gameplay Abilities that match all tags in GameplayTagContainer AND for which
	 * DoesAbilitySatisfyTagRequirements() is true.  The latter requirement allows this function to find the correct
	 * ability without requiring advanced knowledge.  For example, if there are two "Melee" abilities, one of which
	 * requires a weapon and one of which requires being unarmed, then those abilities can use Blocking and Required
	 * tags to determine when they can fire.  Using the Satisfying Tags requirements simplifies a lot of usage cases.
	 * For example, Behavior Trees can use various decorators to test an ability fetched using this mechanism as well
	 * as the Task to execute the ability without needing to know that there even is more than one such ability.
	 */
	// void GetActivatableGameplayAbilitySpecsByAllMatchingTags(const FGameplayTagContainer& GameplayTagContainer, TArray < struct FGameplayAbilitySpec* >& MatchingGameplayAbilities, bool bOnlyAbilitiesThatSatisfyTagRequirements = true) const;

	
	/** 
	 * Attempts to activate every gameplay ability that matches the given tag and DoesAbilitySatisfyTagRequirements().
	 * Returns true if anything attempts to activate. Can activate more than one ability and the ability may fail later.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate abilities.
	 */
	// UFUNCTION(BlueprintCallable, Category = "Abilities")
	// bool TryActivateAbilitiesByTag(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation = true);
	

	/** Returns an ability spec corresponding to given ability class. If modifying call MarkAbilitySpecDirty */
	// FGameplayAbilitySpec* FindAbilitySpecFromClass(TSubclassOf<UGameplayAbility> InAbilityClass);

	
	// Learn how to pass an ability through a gameplay event and activate it through this function, and check if this replicates event information
	/*
	 * Grants an ability and attempts to activate it exactly one time, which will cause it to be removed.
	 * Only valid on the server, and the ability's Net Execution Policy cannot be set to Local or Local Predicted
	 * 
	 * @param AbilitySpec FGameplayAbilitySpec containing information about the ability class, level and input ID to bind it to.
	 * @param GameplayEventData Optional activation event data. If provided, Activate Ability From Event will be called instead of ActivateAbility, passing the Event Data
	 */
	// FGameplayAbilitySpecHandle GiveAbilityAndActivateOnce(FGameplayAbilitySpec& AbilitySpec, const FGameplayEventData* GameplayEventData = nullptr);
	
	