// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Characters/Player/PlayerCharacter.h"

#include "Sandbox/Data/Structs/CharacterInformation.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"
#include "Logging/StructuredLog.h"

#include "Sandbox/Characters/Components/AnimInstance/AnimInstanceBase.h"

#include "Sandbox/Asc/Characters/AbilitySystemPlayerState.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Characters/Components/Periphery/PeripheryComponent.h"
#include "Sandbox/Characters/Components/Camera/TargetLockSpringArm.h"
#include "Sandbox/Characters/Components/Information/InformationComponent.h"
#include "Sandbox/Characters/Components/Saving/SaveComponents/SaveComponent_Character.h"
#include "Sandbox/Game/MultiplayerGameMode.h"
#include "Sandbox/Hud/Hud/PlayerHud.h"


APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Peripheries = CreateDefaultSubobject<UPlayerPeripheriesComponent>(TEXT("Peripheries"));
	Peripheries->GetPeripheryRadius()->SetupAttachment(RootComponent); 
	Peripheries->GetItemDetection()->SetupAttachment(RootComponent);
	Peripheries->GetPeripheryCone()->SetupAttachment(CameraArm);

	// Adjust the periphery cone logic while in editor/game
	Peripheries->SetUnusedPeripheryComponentsVisibility();
	Peripheries->AdjustPeripheryConeInEditor(CameraArm);

	// Inventory Component
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Inventory Component
	SaveComponent = CreateDefaultSubobject<USaveComponent_Character>(TEXT("Save Component"));

	// Combat Component
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	CombatComponent->SetIsReplicated(true);
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2} BeginPlay", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
}


void APlayerCharacter::OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(this);
		if (!AbilitySystemComponent) return;
	}


	/**** Character base attributes and abilities ****/
	AbilityData = GetAbilityInformationFromTable(AbilityDataId);
	AttributeData = GetAttributeInformationFromTable(AttributeInformationId);

	if (HasAuthority())
	{
		if (AbilityData)
		{
			AbilityData->AddAbilityDataToCharacter(AbilitySystemComponent, AbilityDataHandle);
		}
		if (AttributeData)
		{
			AttributeData->AddAttributesToCharacter(AbilitySystemComponent, AttributeDataHandle);
		}
	}


	/**** Character Component logic ****/
	// Inventory component initialization
	if (Inventory)
	{
		Inventory->SetPlayerId();
	}
	
	// Periphery component initialization
	if (Peripheries)
	{
		Peripheries->InitPeripheryInformation();
	}

	// Save component initialization ->  initializes save logic and loads the character's saved information for each component
	if (SaveComponent && HasAuthority())
	{
		SaveComponent->InitializeSaveLogic();
		SaveComponent->LoadPlayerInformation();
	}

	
	// Add ability system state to the anim instance
	SetCharacterMontages();
	UAnimInstanceBase* AnimInstance = GetMesh() ? Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance()) : nullptr;
	if (AnimInstance)
	{
		AnimInstance->InitializeAbilitySystem(AbilitySystemComponent);
	}
	
	// Camera
	InitCameraSettings();

	
	/**** Controller specific logic ****/
	if (IsLocallyControlled())
	{
		// Add the ability input bindings on the client (the ability system isn't ready when the input is normally configured)
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		AbilitySystemComponent->BindAbilityActivationToEnhancedInput(EnhancedInputComponent, AbilityInputActions);
		
		APlayerController* PlayerController = GetController<APlayerController>();
		if (PlayerController)
		{
			// Initialize the player hud
			APlayerHud* Hud = PlayerController->GetHUD<APlayerHud>();
			if (Hud)
			{
				AAbilitySystemPlayerState* AscPlayerState = GetPlayerState<AAbilitySystemPlayerState>();
				UMMOAttributeSet* Attributes = AscPlayerState ? AscPlayerState->GetAttributeSet<UMMOAttributeSet>() : nullptr;
				Hud->InitializeHud(this, PlayerController, AscPlayerState, AbilitySystemComponent, Attributes);
			}
		}
	}

	// Create the character's information widget
	// #if WITH_EDITOR
	if (InformationComponent)
	{
		InformationComponent->InitInformationComponent(this);
	}
	// #endif
	
	// Blueprint function event
	BP_OnInitAbilityActorInfo();
	
	UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2} OnInitAbilityActorInfo", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
}


UInputComponent* APlayerCharacter::CreatePlayerInputComponent()
{
	return Super::CreatePlayerInputComponent();
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// InputComponent = PlayerInputComponent;
	
	
	// if (!AbilitySystemComponent)
	// {
	// 	AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(this);
	// 	if (!AbilitySystemComponent) return;
	// }
	
	// Add the ability input bindings on the server (the input isn't ready on init actor info, (and it's vice versa on the client)
	if (HasAuthority())
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
		AbilitySystemComponent->BindAbilityActivationToEnhancedInput(EnhancedInputComponent, AbilityInputActions);
	}
}




UAttributeData* APlayerCharacter::GetAttributeInformationFromTable(FName AttributeId)
{
	if (!AttributeInformationTable)
	{
		UE_LOGFMT(LogTemp, Error, "{0}::{1}() {2}'s attribute data table!", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return nullptr;
	}

	const F_Table_AttributeData* Information = AttributeInformationTable->FindRow<F_Table_AttributeData>(AttributeId, TEXT("Character Attribute Data Context"));
	if (Information)
	{
		return Information->AttributeData;
	}
	
	return nullptr;
}


UAbilityData* APlayerCharacter::GetAbilityInformationFromTable(FName AbilityId)
{
	if (!AbilityInformationTable)
	{
		UE_LOGFMT(LogTemp, Error, "{0}::{1}() Set {2}'s ability data table!", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return nullptr;
	}

	const F_Table_AbilityData* Information = AbilityInformationTable->FindRow<F_Table_AbilityData>(AbilityId, TEXT("Character Ability Data Context"), false);
	if (Information)
	{
		return Information->AbilityData;
	}
	
	return nullptr;
}
