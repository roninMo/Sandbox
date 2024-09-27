// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "Sandbox/Data/Structs/AbilityInformation.h"
#include "CharacterBase.generated.h"

/*
//----------------------------------------------------------------------------------//
// Combat																			//
//----------------------------------------------------------------------------------//
	- Stephen Ulibarri has content on combat for fps and melee combat, and teaches you a lot of different things and how to handle multiplayer
		- Check that client side prediction helps with combat and all melee attacks except for a few edge case scenarios

	- Finally let's start learning AI in depth and how to go about this. A lot of this is already done (with some configuration that's been cleared away) this is going to be fun
		- Refactor ai combat for multiplayer for handling calculations for all types of combat, different enemies with gimmicks and let's play with attack patterns


//----------------------------------------------------------------------------------//
// Player interaction																//
//----------------------------------------------------------------------------------//
	- The player peripheries already gives us a construct for handling and updating information within the player's range and is probably how we'll handle the player interaction
	- There needs to be some construct in place for handling player and npc interaction with precedence between different situations (whether it's during a quest or passing by)


//----------------------------------------------------------------------------------//
// Animations																		//
//----------------------------------------------------------------------------------//


		Anim Curves are an interesting thing for handling things with animation and there's a lot of functionality that branches out from this. They return a value from a frame of an animation, and that can be used for calculations
		There's using it for things like handling logic, adjusting calculations based on values, adjusting materials, using it for blending values, ik placement and logic, and the list goes on

		Values that we're using to enable certain logic during different animations to enable overlays and inverse kinematics during specific animations to prevent problems

			- Montage/Layering/Inverse Kinematics ("Montage_", "Layering_", "IK_") 
				- (Head), (Pelvis), (Spine), (Legs), (Arm_L), (Arm_R)

		Abp references
			- Primary Layers
				- Layer Blending(BasePoses, MovementLayer, WeaponLayer, OverlayLayer)
				- AO Layer

			- Additional layers
				- Weapon Layers
					- Guns, Melee Weapons, Bows 

			- Ik Layers
				- Pelvis(Layer)
				- Feet(Layer)
				- Arms(Layer)
				- Head(Layer)


	The general idea for creating animations is to have movement logic with different ways to blend certain actions into the current animations without having to create other logic
	There's also a lot of things you have to account for. The animations could be random actions, combat, or inverse kinematics for things like feet and hand placement, and you have to create another blueprint for when the character has weapons equipped

		- If you haven't created blueprints before I'd reference the documentation because there's a lot of things they've done to make things easier, I'm just combining everything here


		Slots
			- DefaultGroup
				- Overlay
				- UpperBody
				- LowerBody

			- Layering Override Group
				- Head
				- Pelvis
				- Legs
				- LeftArm
				- RightArm

				
		Blending References
			- FullBody
			- UpperBody
			- LowerBody
			- RightArm
			- LeftArm
			- RightLeg
			- LeftLeg

			- QuickFeet
			- SlowFeet
			- QuickArms
			- SlowArms


		


//----------------------------------------------------------------------------------//
// Hud																				//
//----------------------------------------------------------------------------------//
	- We need a ui to be created and constructed and each section of the hud needs an intuitive way of handling things

	- I really like how remnant 2 had made their minimap (actually their inventory system in general was kinda awesome (and the combat was very fluid) let's also create this!

	- Quest information and interaction hud

	- Settings
		- General settings
		- Controller/Keybind Input mappings
		- Graphics

	- Equipment
		- Equipment information
			- Information section for equipment interaction
			- Equipping armaments and armor
	- CharacterStats
		- Skills/Abilities perhaps
		- Information section for the character's stats and other information
	- Inventory
		- Create a subclass of the inventory container widget to handle both the player's inventory and any components or other players that they interact with

	
//----------------------------------------------------------------------------------//
// Player																			//
//----------------------------------------------------------------------------------//
	- We need skill trees, I want abilities that affect the player and their armaments tied to tags.
	-		Thinking about this is if I activate a armor or a weapon buff, I want that to add a tag through an effect that is registered by my armaments and my character individually
					(say to do more damage, or help the player block more regardless of the source) -> This makes things less complex and easier to implement


//----------------------------------------------------------------------------------//
// Game logic																		//
//----------------------------------------------------------------------------------//
	- Saving/Autosaving functions that captures the necessary information and player stats during specific events

	- Subclass the game mode and game state, prototype multiple modes -> a team based arena and a coop campaign esqe mode with objectives -> Do this after reading the game state classes
		- this is ready, but I haven't created the modes yet

	- Singleplayer / Multiplayer / Custom / Cooperative gameplay
		- Quests -> individual / specific to one player / all players


	Quest logic
		-> Is quest unlocked (requirements)?
		-> Are the conditions for the quest to be in progress cleared? (in level, etc)
		-> Objectives
			-> Save/Load current objective progress
			-> Update quest within information about current objective
			-> OnBegin / OnCompletion
		-> Quest Rewards
			- multiplayer logic for handling distribution?
			- player experience, loot, etc obtained
			

	Branching Quest
		- Logic for handling different scenarios and requirement list

	Quests
		- Information
		- Requirements
			- Valid to do quest?
			- Requirements cleared?
		- Rewards
			- Experience, loot, etc.
		- Objectives
			- Objectives completed?
		- Status
			- Current status of the quest, how it affects gameplay and other characters, etc.
		- Handle logic at the beginning of a quest, and at the ending including storing references for affecting gameplay (and account for multiplayer scenarios)


	Objective
		- Save / Restart
		- information / objective
		- Location, Combat, Retrieval, a condition that's required to be met (handle with delegates and state)
	- OnBegin / OnCompletion




//----------------------------------------------------------------------------------//
// Level Design																		//
//----------------------------------------------------------------------------------//

	- Landscapes, Modular kits for everything, create lighting, and learn good practices and design patterns to handle this
	- Refactor some things for handling lighting, it's just about complete but we need a good way to handle multiple changes to the weather without it causing mayhem














/*


	Ability System Component
		- Ability Retrieval and Activation
		- Ability delegates handling (Learn how to handle adding and removing delegate and ability system logic during play)
		- Create modular characters/attributes/abilities with game features for different characters and games
		- GSCGameFeatureAction_AddAbilities, GSCGameFeatureAction_AddAnimLayers, GSCGameFeatureAction_AddInputMappingContext
			Abilities should be retrieved for each game, and added/removed based on the player's current equipment
			Attributes should be specific to game, and then the saved information
				- Npc's retrieve saved information from databases for easy creation and reference
				- Players retrieve references to the base attributes, and then the player's saved attributes for that game


	Abilities 
		- Exec Calc Adjustments
		- Input Evoked Events
		- Combat logic
		- Utils


	Ability Tasks
		- Net safe multi task events that use targeting and are input asynchronous
		- Utils for different version of targeting


	Ability System and Attribute Information
		- Add universal information for player stats
		- Combat logic


	Blueprint function library
		- reference retrieval
		- adding abilities and attributes
		


	Combat Component
		- Handle initialization and equipping of weapons and armor 
		- Combat logic for each weapon
		- Handle Attribute's combat calculations
		- Combat Utils


	Weapons
		- Handles construction of weapon
		- Intuitive equipping and allocation of abilities and saved weapon information
		- Each weapon should have their own logic for constructing and destroying weapons 
		

	Player Abilities
		- Handle multiple instances being added/removed without it affecting gameplay
		- Have multiple objects for ability/input binding, and attribute/allocation information


		- On (Activated/Failed/Ended)
			- TArray<FGSCMappedAbility> AddedAbilityHandles;
			- TArray<TObjectPtr<UAttributeSet>> AddedAttributes;
			- TArray<FActiveGameplayEffectHandle> AddedEffects;
			- TArray<FGSCAbilitySetHandle> AddedAbilitySets;
			- TArray<FDelegateHandle> InputBindingDelegateHandles;


		Ability Handling, Attribute Handling, Input Handling




	Abilities should be retrieved for each game, and added/removed based on the player's current equipment
	Attributes should be specific to game, and then the saved information
		- Npc's retrieve saved information from databases for easy creation and reference
		- Players retrieve references to the base attributes, and then the player's saved attributes for that game


*/


class UAbilitySystem;
class UInventoryComponent;
class UPlayerPeripheriesComponent;
class UAdvancedMovementComponent;


/*
 * The universal class for characters, npc's, and enemies in the game
 */
UCLASS()
class SANDBOX_API ACharacterBase : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

protected:
	/** A stored reference to the player's ability system component */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAbilitySystem> AbilitySystemComponent;
	
	/** Input bindings for the ability pressed and released events. Don't forget to also add input mappings to the player's input mapping context */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability System Component") TArray<FInputActionAbilityMap> AbilityInputActions;

	
public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);




	
//----------------------------------------------------------------------//
// Initialization functions and components								//
//----------------------------------------------------------------------//
protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;
	
	/** 
	 * Called when this Pawn is possessed. Only called on the server (or in standalone).
	 * @param NewController The controller possessing this pawn
	 */
	virtual void PossessedBy(AController* NewController) override;

	/** PlayerState Replication Notification Callback */
	virtual void OnRep_PlayerState() override;

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
	UFUNCTION(BlueprintCallable, Category = "Ability") virtual void OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "On Init Ability Actor Info") void BP_OnInitAbilityActorInfo();

	
public:
	/** Creates an InputComponent that can be used for custom input bindings. Called upon possession by a PlayerController. Return null if you don't want one. */
	virtual UInputComponent* CreatePlayerInputComponent() override;

	/** Allows a Pawn to set up custom input bindings. Called upon possession by a PlayerController, using the InputComponent created by CreatePlayerInputComponent(). */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	


	
//----------------------------------------------------------------------------------//
// Movement																			//
//----------------------------------------------------------------------------------//
public:
	/** Templated convenience version for retrieving the movement component. */
	template<class T> T* GetMovementComp(void) const { return Cast<T>(GetMovementComponent()); }

	/** Retrieves the advanced movement component */
	UFUNCTION(BlueprintCallable, Category="Movement", DisplayName="Get Character Movement Component")
	virtual UAdvancedMovementComponent* GetAdvancedMovementComp() const;


protected:
	/**
	 * Customizable event to check if the character can jump in the current state.
	 * Default implementation returns true if the character is on the ground and not crouching,
	 * has a valid CharacterMovementComponent and CanEverJump() returns true.
	 * Default implementation also allows for 'hold to jump higher' functionality: 
	 * As well as returning true when on the ground, it also returns true when GetMaxJumpTime is more
	 * than zero and IsJumping returns true.
	 * 
	 *
	 * @Return Whether the character can jump in the current state. 
	 */
	virtual bool CanJumpInternal_Implementation() const override;
	


	
//-------------------------------------------------------------------------------------//
// Peripheries																		   //
//-------------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Periphery") FVector AISightTraceOffset = FVector(0, 0, 34);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UPlayerPeripheriesComponent> Peripheries;
	

protected:
	/** This calculates whether an ai character has sensed this player, and uses the default logic with an offset for accurate traces  */
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate) override;
	virtual bool IsTraceConsideredVisible(const FHitResult* HitResult, const AActor* TargetActor);
	


	
//-------------------------------------------------------------------------------------//
// Camera																			   //
//-------------------------------------------------------------------------------------//
protected:



	
//----------------------------------------------------------------------------------//
// Inventory																		//
//----------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UInventoryComponent> Inventory;

	
public:
	/** Templated convenience version for retrieving the movement component. */
	template<class T> T* GetInventory(void) const { return Cast<T>(GetInventoryComponent()); }

	/** Retrieves the advanced movement component */
	UFUNCTION(BlueprintCallable, Category="Movement", DisplayName="Get Inventory")
	virtual UInventoryComponent* GetInventoryComponent() const;


	
	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
protected:
	
	
};
