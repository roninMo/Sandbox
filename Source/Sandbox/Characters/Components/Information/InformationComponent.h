// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InformationComponent.generated.h"

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
 *	The loading state of client / server information. 
 */
UENUM(BlueprintType)
enum class ELoadState : uint8
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


USTRUCT(BlueprintType)
struct FInfo_Inventory
{
	GENERATED_USTRUCT_BODY()
	FInfo_Inventory() = default;

public:
	virtual ~FInfo_Inventory() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGuid Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SortOrder;
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

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** Retrieves a reference to the character */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacter(ACharacterBase*& OutCharacter) const;
	
	/** Retrieves a reference to the character and it's inventory component */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndInventory(ACharacterBase*& OutCharacter, UInventoryComponent*& OutInventoryComponent) const;
	
	/** Retrieves a reference to the character and it's combat component */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndCombatComponent(ACharacterBase*& OutCharacter, UCombatComponent*& OutCombatComponent) const;
	
	/** Retrieves a reference to the character and it's movement component */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndMovementComponent(ACharacterBase*& OutCharacter, UAdvancedMovementComponent*& OutMovementComponent) const;
	
	/** Retrieves a reference to the character and it's periphery component */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndPeripheryComponent(ACharacterBase*& OutCharacter, UPlayerPeripheriesComponent*& OutPeripheryComponent) const;
	
	/** Retrieves a reference to the character and it's camera component */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndCameraComponent(ACharacterBase*& OutCharacter, ACharacterCameraLogic*& OutCameraComponent) const;
	
	/** Retrieves a reference to the character and it's ability system component */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndAbilitySystem(ACharacterBase*& OutCharacter, UAbilitySystem*& OutAbilitySystem) const;
	
	/** Retrieves a reference to the character and it's attributes */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndAttributes(ACharacterBase*& OutCharacter, UAttributeSet*& OutAttributeSet, TSubclassOf<UAttributeSet> AttributeClass) const;

	/** Retrieves a reference to the character and it's save component */
	UFUNCTION(BlueprintCallable, Category = "Player") virtual bool GetCharacterAndSaveComponent(ACharacterBase*& OutCharacter, USaveComponent*& OutSaveComponent) const;




	
//----------------------------------------------------------------------------------------------------------//
// Client / Server Information																				//
//----------------------------------------------------------------------------------------------------------//
public:
	/** The logical state of the inventory information we're retrieving from the client / server.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadState MovementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadState CombatInfoState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadState InventoryState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadState PeripheryState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress") ELoadState SaveState;

	

	
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

	





	
};
