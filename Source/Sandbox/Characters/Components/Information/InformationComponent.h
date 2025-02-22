// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "InformationComponent.generated.h"

enum class EHandlePeripheryLogic : uint8;
DECLARE_LOG_CATEGORY_EXTERN(InformationLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRetrievedInformationSignature, UInformationComponent*, InformationComponent);


enum class EArmamentStance : uint8;
enum class ESaveType : uint8;
enum class ECameraOrientation : uint8;
class AArmament;
class UWidgetBase;
class UAbilitySystem;
class UAttributeSet;
class ACharacterCameraLogic;
class UPlayerPeripheriesComponent;
class UCombatComponent;
class UAdvancedMovementComponent;
class UInventoryComponent;
class ACharacterBase;
class USaveComponent;


/**
 *	Where we're retrieving the character's information from. The client instance (player controlled or proxy, or the server instance actual state) 
 */
UENUM(BlueprintType)
enum class EInfoRetrieval : uint8
{
	Default							UMETA(DisplayName = "Default"),
	Client							UMETA(DisplayName = "Client"),
	Server							UMETA(DisplayName = "Server")
};


/**
 *	The loading state of client / server information. 
 */
UENUM(BlueprintType)
enum class ELoadProgress : uint8
{
	/** None */
	None							UMETA(DisplayName = "None"),
	
	/** We're still waiting on information retrieval */
	Pending							UMETA(DisplayName = "Pending"),

	/** An error occurred while retrieving the information */
	Error							UMETA(DisplayName = "Error"),
	
	/** The information has been retrieved */
	Ready							UMETA(DisplayName = "Ready")
};


/**
 *	The type of information we're dealing with. 
 */
UENUM(BlueprintType)
enum class EStateType : uint8
{
	/** None */
	None							UMETA(DisplayName = "None"),
	/** The character's Movement information */
	Movement							UMETA(DisplayName = "Movement"),
	/** The character's CombatInfo information */
	CombatInfo							UMETA(DisplayName = "CombatInfo"),
	/** The character's Inventory information */
	Inventory							UMETA(DisplayName = "Inventory"),
	/** The character's Periphery information */
	Periphery							UMETA(DisplayName = "Periphery"),
	/** The character's Camera information */
	Camera							UMETA(DisplayName = "Camera"),
	/** The character's Save information */
	Save							UMETA(DisplayName = "Save"),
};


/** Movement component object for sending widget information to the client / server */
USTRUCT(BlueprintType)
struct FInfo_Movement
{
	GENERATED_USTRUCT_BODY()
	FInfo_Movement() = default;

/**** State ****/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TEnumAsByte<EMovementMode> MovementMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bAiming;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bCrouching;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bAcceling;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D Input;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Acceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Velocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector DirectionalVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator AimRotation;
	
/**** Bhop ****/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirStrafe_MaxAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirStrafe_SpeedGainMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirStrafe_RotationRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StrafeSway_Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StrafeSway_SpeedGainMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StrafeSway_RotationRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StrafeLurch_Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StrafeLurch_FullStrengthDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StrafeLurch_Strength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StrafeLurch_Friction;
	
/**** Config ****/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseBhopping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseWallJumping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseMantleJumping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseWallClimbing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseMAntling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseLedgeClimbing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseWallRunning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseSliding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float JumpHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirBrakingDeceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WalkingBrakingDeceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Gravity;
};


/** Combat component object for sending widget information to the client / server */
USTRUCT(BlueprintType)
struct FInfo_Combat
{
	GENERATED_USTRUCT_BODY()
	FInfo_Combat() = default;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AArmament* PrimaryArmament;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AArmament* SecondaryArmament;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EArmamentStance CurrentStance;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FGameplayAbilitySpecHandle> CombatAbilityHandles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ComboIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item LeftHandEquipSlot_One;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item LeftHandEquipSlot_Two;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item LeftHandEquipSlot_Three;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item RightHandEquipSlot_One;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item RightHandEquipSlot_Two;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item RightHandEquipSlot_Three;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item Gauntlets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item Leggings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item Helm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Item Chest;
};


/** Inventory object for sending widget information to the client / server */
USTRUCT(BlueprintType)
struct FInfo_Inventory
{
	GENERATED_USTRUCT_BODY()
	FInfo_Inventory() = default;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGuid, F_Item> QuestItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGuid, F_Item> CommonItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGuid, F_Item> Weapons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGuid, F_Item> Armors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGuid, F_Item> Materials;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGuid, F_Item> Notes;
};


/** Periphery object for sending widget information to the client / server */
USTRUCT(BlueprintType)
struct FInfo_Periphery
{
	GENERATED_USTRUCT_BODY()
	FInfo_Periphery() = default;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bCone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bItemDetection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHandlePeripheryLogic ActivationPhase;
};


/** Camera object for sending widget information to the client / server */
USTRUCT(BlueprintType)
struct FInfo_Camera
{
	GENERATED_USTRUCT_BODY()
	FInfo_Camera() = default;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName CameraStyle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ECameraOrientation CameraOrientation;
};

/** Savestate object for sending widget information to the client / server */
USTRUCT(BlueprintType)
struct FInfo_SaveState
{
	GENERATED_USTRUCT_BODY()
	FInfo_SaveState() = default;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 NetId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString PlatformId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SlotIteration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString AttributeSaveState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString CombaSaveState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString CameraSettingSaveState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString InventorSaveState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString SettingSaveState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString WorlSaveState;
};






/**
 * Component for sending / retrieving state information for both server and client. Useful for debugging network information for multiple characters on both client and server 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UInformationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInformationComponent();
	
	/** What instance of the character are we retrieving the information from */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") EInfoRetrieval InfoRetrieval;
	
	/** The current character we're retrieving the information from */
	UPROPERTY(BlueprintReadWrite, Category = "Progress") TObjectPtr<ACharacterBase> CurrentCharacter;

	/** The current list of valid characters to retrieve information from */
	UPROPERTY(BlueprintReadWrite, Category = "Progress") TArray<ACharacterBase*> CharacterList;


	/**** State Information ****/
	/** The logical state of the movement component information we're retrieving from the client / server.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Movement MovementState;
	
	/** The logical state of the combat component information we're retrieving from the client / server.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Combat CombatInfoState;
	
	/** The logical state of the inventory information we're retrieving from the client / server.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Inventory InventoryState;
	
	/** The logical state of the periphery component information we're retrieving from the client / server.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Periphery PeripheryState;

	/** The logical state of the camera component information we're retrieving from the client / server.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Camera CameraState;
	
	/** The logical state of the save component information we're retrieving from the client / server.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_SaveState SaveState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress MovementProgress;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress CombatInfoProgress;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress InventoryProgress;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress PeripheryProgress;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress CameraProgress;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress SaveProgress;

	


//----------------------------------------------------------------------//
// Information Retrieval												//
//----------------------------------------------------------------------//
protected:
	/**
	 * Begins Play for the component. 
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 * Actor BeginPlay normally happens right after PostInitializeComponents but can be delayed for networked or child actors.
	 * Requires component to be registered and initialized.
	 */
	virtual void BeginPlay() override;
	
	/** Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	/** Initializes the character's information component */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual void InitInformationComponent(ACharacterBase* Character);
	
	// TODO: widget's should automatically be deleted, check that this logic isn't storing extra references
	/** Event for when the information has been retrieved from the server/client */
	UPROPERTY(BlueprintAssignable) FRetrievedInformationSignature OnRetrievedMovementInformation;

	/** Event for when the information has been retrieved from the server/client */
	UPROPERTY(BlueprintAssignable) FRetrievedInformationSignature OnRetrievedCombatInformation;
	
	/** Event for when the information has been retrieved from the server/client */
	UPROPERTY(BlueprintAssignable) FRetrievedInformationSignature OnRetrievedInventoryInformation;
	
	/** Event for when the information has been retrieved from the server/client */
	UPROPERTY(BlueprintAssignable) FRetrievedInformationSignature OnRetrievedPeripheryInformation;
	
	/** Event for when the information has been retrieved from the server/client */
	UPROPERTY(BlueprintAssignable) FRetrievedInformationSignature OnRetrievedCameraInformation;
	
	/** Event for when the information has been retrieved from the server/client */
	UPROPERTY(BlueprintAssignable) FRetrievedInformationSignature OnRetrievedSaveStateInformation;

	
	/**
	 * Retrieve the current character's movement information. The information retrieved is based on @ref InfoRetrieval for whether it's the server, client, or controlled reference.
	 *
	 * Once the information has been retrieved, the event function will broadcast that it's state has been updated for each information state category
	 *
	 * @param StateType					The category of information we're retrieving
	 * @param LoadProgress				Whether the information is still pending, or ready to be retrieved
	 */
	UFUNCTION(BlueprintCallable, Category = "Player|State") virtual void RetrieveStateFromCharacter(EStateType StateType, ELoadProgress& LoadProgress);
	
	
protected:
	/** Utility function to retrieve information based on the state we're retrieving from */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual void GetStateInformation(EStateType StateType);

	/** Utility for retrieving the state from the character's movement component */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual FInfo_Movement GetMovementInformation(ACharacterBase* Character);
	
	/** Utility for retrieving the state from the character's combat component */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual FInfo_Combat GetCombatInformation(ACharacterBase* Character);
	
	/** Utility for retrieving the state from the character's inventory component */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual FInfo_Inventory GetInventoryInformation(ACharacterBase* Character);
	
	/** Utility for retrieving the state from the character's periphery component */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual FInfo_Periphery GetPeripheryInformation(ACharacterBase* Character);
	
	/** Utility for retrieving the state from the character's camera component */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual FInfo_Camera GetCameraInformation(ACharacterBase* Character);
	
	/** Utility for retrieving the state from the character's save component */
	UFUNCTION(BlueprintCallable, Category = "Player|State|Utility") virtual FInfo_SaveState GetSaveStateInformation(ACharacterBase* Character);



	
//----------------------------------------------------------------------//
// (Client) Retrieve information from the server						//
//----------------------------------------------------------------------//
public:
	// Inventory
	// Server_SendInventoryInformationToClient();
	// Client_RetrieveInventoryInformation();
	
	// MovementComponent
	// Server_SendMovementComponentInformationToClient();
	// Client_RetrieveMovementComponentInformation();
	
	// CombatComponent
	// Server_SendCombatComponentInformationToClient();
	// Client_RetrieveCombatComponentInformation();
	
	// PeripheryComponent
	// Server_SendPeripheryComponentInformationToClient();
	// Client_RetrievePeripheryComponentInformation();
	
	// SaveComponent
	// Server_SendSaveComponentInformationToClient();
	// Client_RetrieveSaveComponentInformation();


	// Current character information
	//		-> Open information panel
	//		-> Retrieve information from component
	//		-> update display

	// Client/Server character information reference
	//		-> Open information panel
	//		-> Bind to information update delegate
	//		-> Update the progress state and route the Client/Server functions to retrieve and send back the information
	//		-> Once the information has been retrieved, broadcast the event and update the display

	
	//  - Check Info Retrieval State
	//	- Bind to delegate function
	//		- On current character, immediately broadcast and update state
	//		- On Client/Server character logic, handle remote procedure call logic to retrieve their server/client instance


	
//----------------------------------------------------------------------//
// (Server) Retrieve information from the client						//
//----------------------------------------------------------------------//
public:
	// Inventory
	// Server_SendInventoryInformationToClient();
	// Client_RetrieveInventoryInformation();
	
	// MovementComponent
	// Server_SendMovementComponentInformationToClient();
	// Client_RetrieveMovementComponentInformation();
	
	// CombatComponent
	// Server_SendCombatComponentInformationToClient();
	// Client_RetrieveCombatComponentInformation();
	
	// PeripheryComponent
	// Server_SendPeripheryComponentInformationToClient();
	// Client_RetrievePeripheryComponentInformation();
	
	// SaveComponent
	// Server_SendSaveComponentInformationToClient();
	// Client_RetrieveSaveComponentInformation();

	



//----------------------------------------------------------------------//
// Utility																//
//----------------------------------------------------------------------//
public:
	/** Retrieves all characters currently in the game */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual TArray<ACharacterBase*> GetAllCharacters();

	/** Retrieves a reference to the character */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacter(ACharacterBase*& OutCharacter) const;
	
	/** Retrieves a reference to the character and it's inventory component */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndInventory(ACharacterBase*& OutCharacter, UInventoryComponent*& OutInventoryComponent) const;
	
	/** Retrieves a reference to the character and it's combat component */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndCombatComponent(ACharacterBase*& OutCharacter, UCombatComponent*& OutCombatComponent) const;
	
	/** Retrieves a reference to the character and it's movement component */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndMovementComponent(ACharacterBase*& OutCharacter, UAdvancedMovementComponent*& OutMovementComponent) const;
	
	/** Retrieves a reference to the character and it's periphery component */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndPeripheryComponent(ACharacterBase*& OutCharacter, UPlayerPeripheriesComponent*& OutPeripheryComponent) const;
	
	/** Retrieves a reference to the character and it's camera component */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndCameraComponent(ACharacterBase*& OutCharacter, ACharacterCameraLogic*& OutCameraComponent) const;
	
	/** Retrieves a reference to the character and it's ability system component */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndAbilitySystem(ACharacterBase*& OutCharacter, UAbilitySystem*& OutAbilitySystem) const;
	
	/** Retrieves a reference to the character and it's attributes */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndAttributes(ACharacterBase*& OutCharacter, UAttributeSet*& OutAttributeSet, TSubclassOf<UAttributeSet> AttributeClass) const;

	/** Retrieves a reference to the character and it's save component */
	UFUNCTION(BlueprintCallable, Category = "Player|Utility") virtual bool GetCharacterAndSaveComponent(ACharacterBase*& OutCharacter, USaveComponent*& OutSaveComponent) const;




	
};
