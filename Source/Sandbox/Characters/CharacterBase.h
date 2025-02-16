// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "Sandbox/Data/Interfaces/PeripheryObject/PeripheryObjectInterface.h"
#include "Sandbox/Data/Structs/AbilityInformation.h"
#include "Sandbox/Data/Structs/CharacterMontages.h"
#include "CharacterBase.generated.h"

/*
//----------------------------------------------------------------------------------//
// Movement																			//
//----------------------------------------------------------------------------------//
	- Add a cover system for both standing and crouching for specific objects
	- Fix ledge mantling detection for proper mantle to climb up to a proper ledge / prevent clipping
	- Perhaps add logic for wall climbing on angled walls, and logic settings for either valid object based wall climbing, or wall climbing at specific angles with a valid accepted angle


//----------------------------------------------------------------------------------//
// Player interaction																//
//----------------------------------------------------------------------------------//
	- The player peripheries already gives us a construct for handling and updating information within the player's range and is probably how we'll handle the player interaction
	- Customizable Interaction Plugin handles event driven logic for players and ai to handle certain logic while interacting with the world
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


	The current anim blueprint logic handles all movement and combat animation with layered blending for both montages and custom anim curve logic that can be blended seamlessly
		and built in inverse kinematics for hands/feet movement, and customization for equipped / different character states

		


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

	- Logic for handling character settings

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
// Combat																			//
//----------------------------------------------------------------------------------//
	- Stephen Ulibarri has content on combat for fps and melee combat, and teaches you a lot of different things and how to handle multiplayer
		- Check that client side prediction helps with combat and all melee attacks except for a few edge case scenarios

	- Finally let's start learning AI in depth and how to go about this. A lot of this is already done (with some configuration that's been cleared away) this is going to be fun
		- Refactor ai combat for multiplayer for handling calculations for all types of combat, different enemies with gimmicks and let's play with attack patterns


//----------------------------------------------------------------------------------//
// Level Design																		//
//----------------------------------------------------------------------------------//

	- Landscapes, Modular kits for everything, create lighting, and learn good practices and design patterns to handle this
	- Refactor some things for handling lighting, it's just about complete but we need a good way to handle multiple changes to the weather without it causing mayhem














/*


Completed Stuff

	Ability System Component
		- Ability Retrieval and Activation
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




Current Todo


	- Add multiple different combat abilities and combo attacks
	- Add character's with different attacks and learn how to handle combat, respawning, etc.
	- Create a way of quickly creating logic and scenarios for combat instead of actually building characters. Handle this early so it's easy to build upon and add combat
	- Add actor channel logic for custom replication for different game modes. Players should always be replicated,
		- and the focus on replication should be the character and it's values for story, coop, and combat, instead of the standard replication from the client / server channel
		- Add logic / guidelines for net relevancy to get an edge on performance without any of the drawbacks / bottlenecks, however this probably isn't optimal (and rather leisure) compared to other ways of actor replication, perhaps more for the player
			

	- Fix hit stun reactions to be precise for proper combat
	- Finish combat / respawn logic



	- Respawning
		- Player respawn logic
			- Respawn logic added to the gamestate to handle events and the actual respawning specific to the gamemode
				- GameMode -> Respawn Events to character and inventory components -> Subclassed Respawn logic i.e. TDM, Singleplayer / Co op / etc.
			- Events on the player for handling respawning, saving, and any other logic before the player respawns
		- AI respawn logic
			- Respawn logic specific to the game mode, and distribution of loot / experience handled on the server


	- Combat
		- Handle sending imperative logic across the net to prevent replication problems
		- Fix hit stun to create proper hit reactions for smooth combat, and then adjust how hit stun durations are sent across the net to prevent replication causing delayed / chained lag
		- Players and enemies, We need both thrilling and gimmick combat -> try honing it towards elden ring kind of combat and play with the combat / movement mechanics for this
		- Add advanced ai combat using squads -> chained attacks, enemy behavior (phlanx, formations, reactions/rotations in enemies being hurt, ambushes, or other weird or random behaviors, and create a system to transition between each)


	- HitStun
		- Add logic to net serialize timestamps when players are attacked to avoid hit react durations to be inaccurate. This bases reaction time off of ping, and avoids replication delays for durations
			- Benefits -> delayed attacks that land don't add to hit stun duration, preventing chained lag
			- Drawbacks -> the usual lag above 400 ping will cause hit stun to not work properly.
		- So long as attack reactions aren't quick, the elapsed duration can be calculated on the client safely without relying on the server's response from the duration being finished, allowing the client to predict durations in real time


	- Saving logic
		- Infrastructure for saving during events that's easily incorporated into different game modes, or during play.
			- Net efficient for saving player state and item's added, adjusted, removed, or updated during the game


	- Advanced AI logic
		- After finishing the initial combat logic add advanced ai state trees and squad combat logic
			- Default ai behavior for different npc's and enemy types, or a transition for each individual behavior
			- Linked logic for squads, and autonomous behavior for specific interactions
			- Other logic / events for enemies / allies logic, and add a framework for idle behavior and interaction for both story and random stuff
	

*/


class USaveComponent;
enum class EHitStun : uint8;
enum class EHitDirection : uint8;
enum class EArmorSlot : uint8;
enum class ECharacterSkeletonMapping : uint8;
class UCombatComponent;
class UAbilitySystem;
class UInventoryComponent;
class UPlayerPeripheriesComponent;
class UAdvancedMovementComponent;


/*
 * The universal class for characters, npc's, and enemies in the game
 */
UCLASS()
class SANDBOX_API ACharacterBase : public ACharacter, public IAbilitySystemInterface, public IAISightTargetInterface, public IPeripheryObjectInterface
{
	GENERATED_BODY()

protected:
	/** The combat component. Used for handling armor and equipment, with combat functionality */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UCombatComponent> CombatComponent;
	
	/** The character's inventory component. Stores their items, materials, equipment, armor, etc.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UInventoryComponent> Inventory;

	/** A stored reference to the character's ability system component */
	UPROPERTY(BlueprintReadWrite, Category = "Ability System Component") TObjectPtr<UAbilitySystem> AbilitySystemComponent;


public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	///** Make this actor tick after PrerequisiteActor. This only applies to this actor's tick function; dependencies for owned components must be set up separately if desired. */
	//UFUNCTION(BlueprintCallable, Category = "Actor|Tick", meta = (Keywords = "dependency"))
	//	virtual void AddTickPrerequisiteActor(AActor* PrerequisiteActor);

	///** Make this actor tick after PrerequisiteComponent. This only applies to this actor's tick function; dependencies for owned components must be set up separately if desired. */
	//UFUNCTION(BlueprintCallable, Category = "Actor|Tick", meta = (Keywords = "dependency"))
	//	virtual void AddTickPrerequisiteComponent(UActorComponent* PrerequisiteComponent);

	// Actor component functions for frame dependent logic -> OnEndOfFrameUpdateDuringTick() and OnPreEndOfFrameSync()

	///** Called after the actor is spawned in the world.  Responsible for setting up actor for play. */
	//void PostSpawnInitialize(FTransform const& SpawnTransform, AActor* InOwner, APawn* InInstigator, bool bRemoteOwned, bool bNoFail, bool bDeferConstruction, ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot);

	///** Called to finish the spawning process, generally in the case of deferred spawning */
	//void FinishSpawning(const FTransform& Transform, bool bIsDefaultTransform = false, const FComponentInstanceDataCache* InstanceDataCache = nullptr, ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale);

	// multiplayer notify logic that can occuring before / after spawning randomly


	
	
	/**** Initialization ****/
	///**
	// * Called after all the components in the Components array are registered, called both in editor and during gameplay.
	// * bHasRegisteredAllComponents must be set true prior to calling this function.
	// */
	//virtual void PostRegisterAllComponents();

	///** Reset actor to initial state - used when restarting level without reloading. */
	//virtual void Reset();
	


	
	//--------------------------------------------------------------------------------------------------------------------------------------//
	// Networking																															//
	//--------------------------------------------------------------------------------------------------------------------------------------//
		/**
					Depending on the game, you might want to utilize replication for handling networking / player logic differently, and here's a breakdown of how it's handled in unreal


			Actor specific functions
				- OnSerializeNewActor
				- OnActorChannelOpen

				- PreNetReceive
				- PostNetReceive
				- PostRepNotifies
				- NetUpdateLogic

				- IsWithinNetRelevancyDistance

				- OnNetCleanUp
				- GetNetPriority

			Replication Driver / Connection / Actor Channel logic
				- Actor Channel functions for storing / saving / serializing for mapping and sending packets from client to server 
				- ServerReplicateActors
				- ReplicateActorListsForConnections_Default
				- ReplicateActorsForConnection
				- ReplicateActor

			Replication Nodes aren't used by default, and that's all good. For arena based games that aren't open world where net relevancy comes into play, and this is a priority for another time
				- Subclassing replication nodes for custom logic and handling is beneficial, since net relevancy is only used for handling replication event logic,
				- Having objects that are specific to a player need to be replication with spatialization nodes separately for performances and efficiency

			There's state for data replication to handle what objects are replicated and additional logic for handling filtering based on other conditions (NetPriority, NetRelevancy, etc.)

		*/


	/**** Actor channel sub object replication list logic ****/
	// Adding / Removing objects to the replicated subobject list queues the objects and their values to be replicated based on their configuration accordingly
	//		- Check that this happens when objects are spawned within the game
	// AbilitySystem logic -> Everytime an ability is created / invoked, an instance created and replicated for multiplayer, and the actor channel's net replication logic handles everything else
	
	// /** The main function that will actually replicate actors. Called every server tick. */
	// virtual int32 ServerReplicateActors(float DeltaSeconds) PURE_VIRTUAL(UReplicationDriver::ServerReplicateActors, return 0; );
	//		- Optionally add custom logic here for network acknowledge fixes / updates

	// /** Logic that handles replication and prioritization on what replicates during each frame budget */
	// void ReplicateActorListsForConnections_Default(UNetReplicationGraphConnection* ConnectionManager, FGatheredReplicationActorLists& GatheredReplicationListsForConnection, FNetViewerArray& Viewers);
	//		- Custom logic for what gets prioritized when (not necessary because the current system has plenty of benefits)

	// /** Actual replication logic ( Actor-> pre-replication/replication variable preparations -> ActorChannel replication logic / handling */
	//virtual void ReplicateActorsForConnection(UNetConnection* NetConnection, FPerConnectionActorInfoMap& ConnectionActorInfoMap, UNetReplicationGraphConnection* ConnectionManager, const uint32 FrameNum);
	
	// /** Replicate this channel's actor differences. Returns how many bits were replicated (does not include non-bunch packet overhead) */
	// int64 ReplicateActor();
	//		- Unreal handles the bunches / packet serialization and object mapping logic during replication, and queues for latent serialization
	//				based on the object information, what needs to be updated, and replication priority in the actor channel logic


	// NetDriver.h !!!
	// CharacterMovementComponent networking logic -> INetworkPredictionInterface.h
	// The different networking models for client / server logic
	//  - ActorChannel, DataReplication, NetDriver, ReplicationDriver, ActorReplicationBridge, etc.
	//	- FGuidReferences -> INetSerializeCB -> FRepState

	/**** NetDriver / Data Replication / ActorChannel ****/
	// void FPacketSimulationSettings::LoadConfig(const TCHAR* OptionalQualifier)


	
	
	/**** Actor.h ****/
	///** Called right before receiving a bunch */
	//virtual void PreNetReceive();

	///** Called right after receiving a bunch */
	//virtual void PostNetReceive();

	///** Called right after calling all OnRep notifies (called even when there are no notifies) */
	//virtual void PostRepNotifies() {}
	
	///**
	// * Function used to prioritize actors when deciding which to replicate
	// * @param ViewPos		Position of the viewer
	// * @param ViewDir		Vector direction of viewer
	// * @param Viewer		"net object" owned by the client for whom net priority is being determined (typically player controller)
	// * @param ViewTarget	The actor that is currently being viewed/controlled by Viewer, usually a pawn
	// * @param InChannel		Channel on which this actor is being replicated.
	// * @param Time			Time since actor was last replicated
	// * @param bLowBandwidth True if low bandwidth of viewer
	// * @return				Priority of this actor for replication, higher is more important
	// */
	//virtual float GetNetPriority(const FVector& ViewPos, const FVector& ViewDir, class AActor* Viewer, AActor* ViewTarget, UActorChannel* InChannel, float Time, bool bLowBandwidth);

	///**
	// * Allows for a specific response from the actor when the actor channel is opened (client side)
	// * @param InBunch Bunch received at time of open
	// * @param Connection the connection associated with this actor
	// */
	//virtual void OnActorChannelOpen(class FInBunch& InBunch, class UNetConnection* Connection) {};

	///** Force actor to be updated to clients/demo net drivers */
	//UFUNCTION(BlueprintCallable, Category = "Networking")
	//	virtual void ForceNetUpdate();
	
	///**
	// * SerializeNewActor has just been called on the actor before network replication (server side)
	// * @param OutBunch Bunch containing serialized contents of actor prior to replication
	// */
	//virtual void OnSerializeNewActor(class FOutBunch& OutBunch) {};

	///**
	// * Handles cleaning up the associated Actor when killing the connection
	// * @param Connection the connection associated with this actor
	// */
	//virtual void OnNetCleanup(class UNetConnection* Connection) {};

	///**
	// * Determines whether or not the distance between the given SrcLocation and the Actor's location
	// * is within the net relevancy distance. Actors outside relevancy distance may not be replicated.
	// *
	// * @param SrcLocation	Location to test against.
	// * @return True if the actor is within net relevancy distance, false otherwise.
	// */
	//bool IsWithinNetRelevancyDistance(const FVector& SrcLocation) const;


	/**** Debugging Information pertaining to replication stored at UNetReplicationGraphConnection ****/
	// Use Pre/Post Net receive functions for understanding how packets are sent across the net
	// Check net update logic for how it handles net updates for debugging a specific actor

	/** A map of all of our per-actor data */
	// FPerConnectionActorInfoMap ActorInfoMap;
	// FOnPostReplicatePrioritizedLists OnPostReplicatePrioritizeLists;
	// UReplicationGraph::PostServerReplicateStats(const FFrameReplicationStats& Stats)


	

	/**** Movement ****/
	///** ReplicatedMovement struct replication event */
	//UFUNCTION()
	//	virtual void OnRep_ReplicatedMovement();
	
	///** Update location and rotation from ReplicatedMovement. Not called for simulated physics! */
	//virtual void PostNetReceiveLocationAndRotation();

	///** Update velocity - typically from ReplicatedMovement, not called for simulated physics! */
	//virtual void PostNetReceiveVelocity(const FVector& NewVelocity);

	///** Update and smooth simulated physic state, replaces PostNetReceiveLocation() and PostNetReceiveVelocity() */
	//virtual void PostNetReceivePhysicState();



	
	/**** Camera ****/
	///** Called when this actor becomes the given PlayerController's ViewTarget. Triggers the Blueprint event K2_OnBecomeViewTarget. */
	//virtual void BecomeViewTarget(class APlayerController* PC);

	///** Called when this actor is no longer the given PlayerController's ViewTarget. Also triggers the Blueprint event K2_OnEndViewTarget. */
	//virtual void EndViewTarget(class APlayerController* PC);

	///**
	// * Hook to allow actors to render HUD overlays for themselves.  Called from AHUD::DrawActorOverlays().
	// * @param PC is the PlayerController on whose view this overlay is rendered
	// * @param Canvas is the Canvas on which to draw the overlay
	// * @param CameraPosition Position of Camera
	// * @param CameraDir direction camera is pointing in.
	// */
	//virtual void PostRenderFor(class APlayerController* PC, class UCanvas* Canvas, FVector CameraPosition, FVector CameraDir);



	
	/**** Utility ****/
	///** Getter for the cached world pointer, will return null if the actor is not actually spawned in a level */
	//virtual UWorld* GetWorld() const override final;

	///** Get the timer instance from the actors world */
	//class FTimerManager& GetWorldTimerManager() const;

	///** Gets the GameInstance that ultimately contains this actor. */
	//class UGameInstance* GetGameInstance() const;

	// FGameplayAbilitiesModule::StartupModule() -> AHUD::OnShowDebugInfo.AddStatic(&UAbilitySystemComponent::OnShowDebugInfo);


	/**** Combat ****/
	///**
	// * Returns the optimal location to fire weapons at this actor
	// * @param RequestedBy - the Actor requesting the target location
	// */
	//virtual FVector GetTargetLocation(AActor* RequestedBy = nullptr) const;

	// FWorldAsyncTraceState



	
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
	 * @param InAvatarActor			Is what physical actor in the world we are acting on. Usually a Pawn but it could be a Tower, Building, etc, may be the same as Owner
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability") virtual void OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "On Init Ability Actor Info") void BP_OnInitAbilityActorInfo();
	


	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Peripheries")
	TObjectPtr<UPlayerPeripheriesComponent> Peripheries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Peripheries") FVector AISightTraceOffset = FVector(0, 0, 34);
	

protected:
	/** This calculates whether an ai character has sensed this player, and uses the default logic with an offset for accurate traces  */
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate) override;
	virtual bool IsTraceConsideredVisible(const FHitResult* HitResult, const AActor* TargetActor);



	
//-------------------------------------------------------------------------------------//
// Ability System Component															   //
//-------------------------------------------------------------------------------------//
public:
	/** Templated convenience version for retrieving the ability system component. */
	template<class T> T* GetAbilitySystem(void) const { return Cast<T>(GetAbilitySystemComponent()); }

	/** Returns the ability system component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	

//-------------------------------------------------------------------------------------//
// Skeleton																			   //
//-------------------------------------------------------------------------------------//
protected:
	/** The player's current helm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh") TObjectPtr<USkeletalMeshComponent> Helm;
	
	/** The player's current gauntlets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh") TObjectPtr<USkeletalMeshComponent> Gauntlets;
	
	/** The player's current leggings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh") TObjectPtr<USkeletalMeshComponent> Leggings;
	
	/** The player's current chestplate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh") TObjectPtr<USkeletalMeshComponent> Chest;

	/** The current character skeleton mapping reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh") ECharacterSkeletonMapping CharacterSkeletonMapping;

	
public:
	/** A stored reference to the armor's gauntlets mesh */
	UPROPERTY(ReplicatedUsing=OnRep_Armor_Gauntlets, BlueprintReadWrite) USkeletalMesh* Armor_Gauntlets;
	
	/** A stored reference to the armor's leggings mesh */
	UPROPERTY(ReplicatedUsing=OnRep_Armor_Leggings, BlueprintReadWrite) USkeletalMesh* Armor_Leggings;
	
	/** A stored reference to the armor's helm mesh */
	UPROPERTY(ReplicatedUsing=OnRep_Armor_Helm, BlueprintReadWrite) USkeletalMesh* Armor_Helm;
	
	/** A stored reference to the armor's chest plate mesh */
	UPROPERTY(ReplicatedUsing=OnRep_Armor_Chest, BlueprintReadWrite) USkeletalMesh* Armor_Chest;


	/** Adjust's the armor of a specific slot */
	UFUNCTION(BlueprintCallable, Category = "Character|Skeleton") virtual void SetArmorMesh(EArmorSlot ArmorSlot, USkeletalMesh* Armor);

	/** Convenience function to show/hide the character and armor for first/third person logic */
	UFUNCTION(BlueprintCallable, Category = "Character|Skeleton") virtual void SetHideCharacterAndArmor(bool bHide = true);

	/** Retrieves the armor mesh components */
	UFUNCTION(BlueprintCallable, Category = "Character|Skeleton") virtual USkeletalMeshComponent* GetLeggings() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Skeleton") virtual USkeletalMeshComponent* GetGauntlets() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Skeleton") virtual USkeletalMeshComponent* GetHelm() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Skeleton") virtual USkeletalMeshComponent* GetChest() const;
	
	/** Onrep functions handle client replicated information for scenarios when players join the game late @note these still need to be updated on the server */
	UFUNCTION() virtual void OnRep_Armor_Gauntlets();
	UFUNCTION() virtual void OnRep_Armor_Leggings();
	UFUNCTION() virtual void OnRep_Armor_Helm();
	UFUNCTION() virtual void OnRep_Armor_Chest();

	/** Constructor values for each of the armor's skeletal mesh components */
	virtual void ConstructArmorInformation(USkeletalMeshComponent* MeshComponent) const;


public:
	/** Retrieves the character to montage mapping. Used for retrieving the proper animations for different character skeletons */
	UFUNCTION(BlueprintCallable, Category = "Animation|Utilities") virtual ECharacterSkeletonMapping GetCharacterSkeletonMapping() const;


	
	
//-------------------------------------------------------------------------------------//
// Combat																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Templated convenience version for retrieving the combat component. */
	template<class T> T* GetCombatComponent(void) const { return Cast<T>(GetCombatComponent()); }

	/** Retrieves the combat component */
	UFUNCTION(BlueprintCallable, Category = "Combat", DisplayName = "Get Combat Component")
	virtual UCombatComponent* GetCombatComponent() const;

	
//----------------------------------------------------------------------------------//
// Inventory																		//
//----------------------------------------------------------------------------------//
public:
	/** Templated convenience version for retrieving the inventory component. */
	template<class T> T* GetInventory(void) const { return Cast<T>(GetInventoryComponent()); }

	/** Retrieves the inventory component */
	UFUNCTION(BlueprintCallable, Category="Inventory", DisplayName="Get Inventory Component")
	virtual UInventoryComponent* GetInventoryComponent() const;



	
//----------------------------------------------------------------------------------//
// Movement																			//
//----------------------------------------------------------------------------------//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving") TObjectPtr<USaveComponent> SaveComponent;


	
//-------------------------------------------------------------------------------------//
// Montages																			   //
//-------------------------------------------------------------------------------------//
protected:
	/** The data table for character montages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Montage") UDataTable* CharacterMontageTable;

	/** The character to montage id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Montage") FName CharacterMontageId;
	
	/** The data table for character montages */
	UPROPERTY(BlueprintReadWrite, Category = "Animation|Montage") FM_CharacterMontages Montages;


public:
	/** Retrieves the list of the character's montages */
	UFUNCTION(BlueprintCallable, Category = "Animation|Montage") virtual FM_CharacterMontages& GetCharacterMontages();

	/** Retrieves the character's roll montage */
	UFUNCTION(BlueprintCallable, Category = "Animation|Montage") virtual UAnimMontage* GetRollMontage() const;

	/** Retrieves the character's hit react montage */
	UFUNCTION(BlueprintCallable, Category = "Animation|Montage") virtual UAnimMontage* GetHitReactMontage() const;

	/** Retrieves the character montages from the database */
	virtual void SetCharacterMontages();

	/** Plays a montage on all clients */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Animation|Montage")
	virtual void NetMulticast_PlayMontage(UAnimMontage* Montage, FName StartSection = NAME_None, float PlayRate = 1);

	/** Plays a montage on a specific client */
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Animation|Montage")
	virtual void Client_PlayMontage(UAnimMontage* Montage, FName StartSection = NAME_None, float PlayRate = 1);
	
	/** Returns the hit react direction montage section based on the location of the player and the weapon's impact location */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual FName GetHitReactSection(EHitDirection HitDirection, EHitStun HitStun) const;

	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Returns player's camera location */
	UFUNCTION(BlueprintCallable, Category = "Camera|Utilities") virtual FVector GetCameraLocation() const;
	
	
};
