// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sandbox/Data/Structs/InventoryInformation.h"
#include "InformationComponent.generated.h"

enum class ECameraOrientation : uint8;
class AArmament;
enum class EArmamentStance : uint8;
class UAbilitySystem;
class UAttributeSet;
class ACharacterCameraLogic;
class UPlayerPeripheriesComponent;
class UCombatComponent;
class UAdvancedMovementComponent;
class UInventoryComponent;
class ACharacterBase;
class USaveComponent;
DECLARE_LOG_CATEGORY_EXTERN(InformationLog, Log, All);


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString SaveSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString CharacterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString SaveState;
};






/**
 * Component for sending / retrieving state information for both server and client. Useful for debugging network information for multiple characters on both client and server 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UInformationComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** The class of the character's widget used for handling / displaying character information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Information") TSubclassOf<UUserWidget> InformationWidgetClass;

	/** A stored reference to the character's information widget. Used for displaying / adjusting the character's component information, configuration, and other character information on both client and server */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Information") TObjectPtr<UUserWidget> InformationWidget;


public:	
	UInformationComponent();

	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
//----------------------------------------------------------------------------------------------------------//
// Client / Server Information																				//
//----------------------------------------------------------------------------------------------------------//
public:
	/** What instance of the character are we retrieving the information from */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") EInfoRetrieval InfoRetrieval;
	
	/** The current character we're retrieving the information from */
	UPROPERTY(BlueprintReadWrite, Category = "Progress") ACharacterBase* CurrentCharacter;

	/** The current list of valid characters to retrieve information from */
	UPROPERTY(BlueprintReadWrite, Category = "Progress") TArray<ACharacterBase*> CharacterList;


	/** The logical state of the movement component information we're retrieving from the client / server.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Movement MovementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress MovementProgress;
	
	/** The logical state of the combat component information we're retrieving from the client / server.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Combat CombatInfoState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress CombatInfoProgress;
	
	/** The logical state of the inventory information we're retrieving from the client / server.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Inventory InventoryState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress InventoryProgress;
	
	/** The logical state of the periphery component information we're retrieving from the client / server.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_Periphery PeripheryState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress PeripheryProgress;
	
	/** The logical state of the save component information we're retrieving from the client / server.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") FInfo_SaveState SaveState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadProgress SaveProgress;
	

//----------------------------------------------------------------------//
// Default information													//
//----------------------------------------------------------------------//
public:
	UFUNCTION(BlueprintCallable, Category = "Player|State") virtual FInfo_Movement GetMovementInformation(ACharacterBase* Character);
	UFUNCTION(BlueprintCallable, Category = "Player|State") virtual FInfo_Combat GetCombatInformation(ACharacterBase* Character);
	UFUNCTION(BlueprintCallable, Category = "Player|State") virtual FInfo_Inventory GetInventoryInformation(ACharacterBase* Character);
	UFUNCTION(BlueprintCallable, Category = "Player|State") virtual FInfo_Periphery GetPeripheryInformation(ACharacterBase* Character);
	UFUNCTION(BlueprintCallable, Category = "Player|State") virtual FInfo_Camera GetCameraInformation(ACharacterBase* Character);
	UFUNCTION(BlueprintCallable, Category = "Player|State") virtual FInfo_SaveState GetSaveStateInformation(ACharacterBase* Character);


	

	
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
