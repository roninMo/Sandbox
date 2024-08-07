// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "CharacterBase.generated.h"

class UInventoryComponent;
class UAdvancedMovementComponent;


/*
//----------------------------------------------------------------------------------//
// Combat																			//
//----------------------------------------------------------------------------------//
	- Stephen Ulibarri has content on combat for fps and melee combat, and teaches you a lot of different things and how to handle multiplayer
		- Check that client side prediction helps with combat and all melee attacks except for a few edge case scenarios
		- Understanding Gameplay Ability tasks helps with handling this safely and a lot of this isn't as complicated once everything settles out
		- Ai Attack reactions and behavior based on current senses 


//----------------------------------------------------------------------------------//
// CMC																				//
//----------------------------------------------------------------------------------//
	- Add Mantling -> Add valid animations for mantling and everything else that needs to be completed
	- Add break fall on high landings



//----------------------------------------------------------------------------------//
// Player interaction																//
//----------------------------------------------------------------------------------//
	- The player peripheries already gives us a construct for handling and updating information within the player's range and is probably how we'll handle the player interaction
	- There needs to be some construct in place for handling player and npc interaction with precedence between different situations (whether it's during a quest or passing by)
		- And this logic does not need to be conditional it needs to be fun
	- Should there be subsystems in place for handling objectives and other logic for this?


//----------------------------------------------------------------------------------//
// Animations																		//
//----------------------------------------------------------------------------------//



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
		- I have it so that there's an anim blueprint with logic to add actions both in blueprint and from montages, and another blueprint for handling weapons, and inverse kinematics for a bunch of things



		Slots
			- DefaultGroup
				- Overlay
				- UpperBody
				- LowerBody

			- Layering Override Group
				- Head
				- Pelvis
				- RightLeg
				- LeftLeg
				- RightArm
				- LeftArm
				

		

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
// Game configuration																//
//----------------------------------------------------------------------------------//
	- An autosave function that captures the necessary information and player stats during specific events

	- Subclass the game mode and game state, prototype multiple modes -> a team based arena and a coop campaign esqe mode with objectives -> Do this after reading the game state classes
		- this is ready, but I haven't created the modes yet

	- We need automation TDD so I feel safe building this out without breaking other things

		
//----------------------------------------------------------------------------------//
// Player																			//
//----------------------------------------------------------------------------------//
	- We need skill trees, I want abilities that affect the player and their armaments tied to tags so it's not chained together in unnecessary ways.
	-		Thinking about this is if I activate a armor or a weapon buff, I want that to add a tag through an effect that is registered by my armaments and my character individually
					(say to do more damage, or help the player block more regardless of the source) -> This makes things less complex and easier to implement


	- Finally let's start learning AI in depth and how to go about this. A lot of this is already done (with some configuration that's been cleared away) this is going to be fun
	- Refactor ai combat for multiplayer for handling calculations for all types of combat
	- I think lots of enemies with gimmicks and let's play with attack patterns


//----------------------------------------------------------------------------------//
// Level Design																		//
//----------------------------------------------------------------------------------//

	- Landscapes, Modular kits for everything, create lighting, and learn good practices and design patterns to handle this
	- Refactor some things for handling lighting, it's just about complete but we need a good way to handle multiple changes to the weather without it causing mayhem

	





*/
	

/*
 * The universal class for characters, npc's, and enemies in the game
 */
UCLASS()
class SANDBOX_API ACharacterBase : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	
//----------------------------------------------------------------------//
// Initialization functions and components								//
//----------------------------------------------------------------------//
protected:
	/** Called when play begins for this actor. */
	virtual void BeginPlay() override;
	
	
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

	
//--------------------------------------------------------------------------------------------------------------------------//
// OnRepPlayerState/PossessedBy -> Or AI PossessedBy -> To this initialization loop											//
//--------------------------------------------------------------------------------------------------------------------------//
protected:
	/** Initializes global information that's not specific to any character. This should happen before any other initialization logic as a safeguard */
	virtual void InitCharacterGlobals(UDataAsset* Data);

	/** Initialize character components -> Get access to all the pointers, nothing else */
	virtual void InitCharacterComponents(const bool bCalledFromPossessedBy);

	/** Init ability system and attributes -> These are the most important components that tie everything together */
	virtual void InitAbilitySystemAndAttributes(const bool bCalledFromPossessedBy);
	
	/** InitCharacterInformation -> Run any logic necessary for the start of any of the components */
	virtual void InitCharacterInformation();
	

//-------------------------------------------------------------------------------------//
// Peripheries																		   //
//-------------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Periphery") FVector AISightTraceOffset = FVector(0, 0, 34);


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
	template<class T> T* GetInventory(void) const { return Cast<T>(GetInventory()); }

	/** Retrieves the advanced movement component */
	UFUNCTION(BlueprintCallable, Category="Movement", DisplayName="Get Inventory")
	virtual UInventoryComponent* GetInventoryComponent() const;

	
	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
protected:
	
	
};
