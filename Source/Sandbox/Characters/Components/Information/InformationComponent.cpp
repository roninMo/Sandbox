// Fill out your copyright notice in the Description page of Project Settings.


#include "InformationComponent.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Characters/Components/Periphery/PeripheryComponent.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Data/Enums/ArmorTypes.h"

DEFINE_LOG_CATEGORY(InformationLog);


UInformationComponent::UInformationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InfoRetrieval = EInfoRetrieval::Default;
	CurrentCharacter = nullptr;
	CharacterList = {};

	MovementState = FInfo_Movement();
	MovementProgress = ELoadProgress::Ready;
	
	CombatInfoState = FInfo_Combat();
	CombatInfoProgress = ELoadProgress::Ready;
	
	InventoryState = FInfo_Inventory();
	InventoryProgress = ELoadProgress::Ready;
	
	PeripheryState = FInfo_Periphery();
	PeripheryProgress = ELoadProgress::Ready;
	
	SaveState = FInfo_SaveState();
	SaveProgress = ELoadProgress::Ready;
}


void UInformationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Construct the controlled player's information widget
	ACharacterBase* Character;
	if (GetCharacter(Character))
	{
		if (Character->IsLocallyControlled() && InformationWidgetClass && !InformationWidget)
		{
			InformationWidget = CreateWidget(Character->GetPlayerController<APlayerController>(), InformationWidgetClass, FName("Client/Server Information Widget"));

			// Initialize the character's state logic
			CurrentCharacter = Character;
			GetMovementInformation(CurrentCharacter);
			GetCombatInformation(CurrentCharacter);
			GetInventoryInformation(CurrentCharacter);
			GetPeripheryInformation(CurrentCharacter);
			GetCameraInformation(CurrentCharacter);
			GetSaveStateInformation(CurrentCharacter);
		}
	}

	// Retrieve a list of the characters currently in the game!
	CharacterList.Empty();
	CharacterList = GetAllCharacters();
}


void UInformationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}




void UInformationComponent::RetrieveStateFromCharacter(EStateType StateType, ELoadProgress& LoadProgress)
{
	// We need a proper character reference to retrieve information from
	if (!CurrentCharacter)
	{
		return;
	}

	// We need to retrieve information from something first
	if (StateType == EStateType::None)
	{
		return;
	}

	// Retrieve the proper event delegate for when the information has been retrieved
	FRetrievedInformationSignature& OnRetrievedEvent = 
		StateType == EStateType::Movement ? OnRetrievedMovementInformation :
		StateType == EStateType::CombatInfo ? OnRetrievedCombatInformation :
		StateType == EStateType::Inventory ? OnRetrievedInventoryInformation :
		StateType == EStateType::Periphery ? OnRetrievedPeripheryInformation :
		StateType == EStateType::Camera ? OnRetrievedCameraInformation :
		OnRetrievedSaveStateInformation;

	ELoadProgress& Progress = 
		StateType == EStateType::Movement ? MovementProgress :
		StateType == EStateType::CombatInfo ? CombatInfoProgress :
		StateType == EStateType::Inventory ? InventoryProgress :
		StateType == EStateType::Periphery ? PeripheryProgress :
		StateType == EStateType::Camera ? CameraProgress :
		SaveProgress;
	
	// Default logic
	ENetRole Role = CurrentCharacter->GetLocalRole();
	if (InfoRetrieval == EInfoRetrieval::Default)
	{
		GetStateInformation(StateType);
		Progress = ELoadProgress::Ready;
		OnRetrievedEvent.Broadcast(this);
	}

	// Retrieving information from the client
	if (InfoRetrieval == EInfoRetrieval::Client)
	{
		// If this is already the client instance of the character
		if (Role != ROLE_Authority)
		{
			GetStateInformation(StateType);
			Progress = ELoadProgress::Ready;
			OnRetrievedEvent.Broadcast(this);
		}
		else
		{
			// Server_SendInformationToClient
			Progress = ELoadProgress::Pending;
		}
	}
		
	// Retrieving information from the server
	if (InfoRetrieval == EInfoRetrieval::Server)
	{
		// If this is already the server instance of the character
		if (Role != ROLE_Authority)
		{
			// Client_SendInformationToServer
			Progress = ELoadProgress::Pending;
		}
		else
		{
			GetStateInformation(StateType);
			Progress = ELoadProgress::Ready;
			OnRetrievedEvent.Broadcast(this);
		}
	}
}


void UInformationComponent::GetStateInformation(EStateType StateType)
{
	if (!CurrentCharacter) return;
	if (StateType == EStateType::None) return;
	if (StateType == EStateType::Movement) MovementState = GetMovementInformation(CurrentCharacter);
	if (StateType == EStateType::CombatInfo) CombatInfoState = GetCombatInformation(CurrentCharacter);
	if (StateType == EStateType::Inventory) InventoryState = GetInventoryInformation(CurrentCharacter);
	if (StateType == EStateType::Periphery) PeripheryState = GetPeripheryInformation(CurrentCharacter);
	if (StateType == EStateType::Camera) CameraState = GetCameraInformation(CurrentCharacter);
	if (StateType == EStateType::Save) SaveState = GetSaveStateInformation(CurrentCharacter);
}


#pragma region Information Retrieval Getters
FInfo_Movement UInformationComponent::GetMovementInformation(ACharacterBase* Character)
{
	FInfo_Movement MovementInformation;
	UAdvancedMovementComponent* MovementComponent = Character ? Character->GetAdvancedMovementComp() : nullptr;
	if (!MovementComponent)
	{
		return MovementInformation;
	}

	// state
	MovementInformation.MovementMode = MovementComponent->GetMovementMode();
	MovementInformation.bAiming = MovementComponent->IsAiming();
	MovementInformation.bCrouching = MovementComponent->IsCrouching();
	MovementInformation.bAcceling = MovementComponent->Velocity.Size() > 0;
	MovementInformation.Input = MovementComponent->GetPlayerInput();
	MovementInformation.Acceleration = MovementComponent->GetCurrentAcceleration();
	MovementInformation.Velocity = MovementComponent->GetLastUpdateVelocity();
	MovementInformation.Speed = MovementComponent->Velocity.Size();
	MovementInformation.Rotation = Character->GetActorRotation();
	MovementInformation.DirectionalVelocity = UKismetMathLibrary::Quat_UnrotateVector(MovementInformation.Rotation.Quaternion(), MovementInformation.Velocity);
	MovementInformation.AimRotation = MovementInformation.DirectionalVelocity.ToOrientationRotator();;

	// bhop
	MovementInformation.AirStrafe_MaxAcceleration = MovementComponent->StrafingMaxAcceleration;
	MovementInformation.AirStrafe_SpeedGainMultiplier = MovementComponent->AirStrafeSpeedGainMultiplier;
	MovementInformation.AirStrafe_RotationRate = MovementComponent->AirStrafeRotationRate;
	MovementInformation.StrafeSway_Duration = MovementComponent->StrafeSwayDuration;
	MovementInformation.StrafeSway_SpeedGainMultiplier = MovementComponent->StrafeSwaySpeedGainMultiplier;
	MovementInformation.StrafeSway_RotationRate = MovementComponent->StrafeSwayRotationRate;
	MovementInformation.StrafeLurch_Duration = MovementComponent->StrafeLurchDuration;
	MovementInformation.StrafeLurch_FullStrengthDuration = MovementComponent->StrafeLurchFullStrengthDuration;
	MovementInformation.StrafeLurch_Strength = MovementComponent->StrafeLurchStrength;
	MovementInformation.StrafeLurch_Friction = MovementComponent->StrafeLurchFriction;

	// config
	MovementInformation.bUseBhopping = MovementComponent->bUseBhopping;
	MovementInformation.bUseWallJumping = MovementComponent->bUseWallJumping;
	MovementInformation.bUseMantleJumping = MovementComponent->bUseMantleJumping;
	MovementInformation.bUseWallClimbing = MovementComponent->bUseWallClimbing;
	MovementInformation.bUseMAntling = MovementComponent->bUseMantling;
	MovementInformation.bUseLedgeClimbing = MovementComponent->bUseLedgeClimbing;
	MovementInformation.bUseWallRunning = MovementComponent->bUseWallRunning;
	MovementInformation.bUseSliding = MovementComponent->bUseSliding;
	MovementInformation.MoveSpeed = MovementComponent->Velocity.Size();
	MovementInformation.JumpHeight = MovementComponent->JumpZVelocity;
	MovementInformation.AirBrakingDeceleration = MovementComponent->BrakingDecelerationFalling;
	MovementInformation.WalkingBrakingDeceleration = MovementComponent->BrakingDecelerationWalking;
	MovementInformation.Gravity = MovementComponent->GravityScale;

	return MovementInformation;
}


FInfo_Combat UInformationComponent::GetCombatInformation(ACharacterBase* Character)
{
	FInfo_Combat CombatInformation = FInfo_Combat();
	UCombatComponent* CombatComponent = Character ? Character->GetCombatComponent() : nullptr;
	if (CombatComponent)
	{
		return CombatInformation;
	}
	
	CombatInformation.PrimaryArmament = CombatComponent->GetArmament();
	CombatInformation.SecondaryArmament = CombatComponent->GetArmament(false);
	CombatInformation.CurrentStance = CombatComponent->GetCurrentStance();
	CombatInformation.ComboIndex = CombatComponent->GetComboIndex();
	CombatInformation.LeftHandEquipSlot_One = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotOne);
	CombatInformation.LeftHandEquipSlot_Two = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotTwo);
	CombatInformation.LeftHandEquipSlot_Three = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::LeftHandSlotThree);
	CombatInformation.RightHandEquipSlot_One = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotOne);
	CombatInformation.RightHandEquipSlot_Two = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotTwo);
	CombatInformation.RightHandEquipSlot_Three = CombatComponent->GetArmamentInventoryInformation(EEquipSlot::RightHandSlotThree);
	CombatInformation.Gauntlets = CombatComponent->GetArmorItemInformation(EArmorSlot::Gauntlets);
	CombatInformation.Leggings = CombatComponent->GetArmorItemInformation(EArmorSlot::Leggings);
	CombatInformation.Helm = CombatComponent->GetArmorItemInformation(EArmorSlot::Helm);
	CombatInformation.Chest = CombatComponent->GetArmorItemInformation(EArmorSlot::Chest);
	return CombatInformation;
}


FInfo_Inventory UInformationComponent::GetInventoryInformation(ACharacterBase* Character)
{
	FInfo_Inventory InventoryInformation = FInfo_Inventory();
	UInventoryComponent* InventoryComponent = Character ? Character->GetInventoryComponent() : nullptr;
	if (!InventoryComponent)
	{
		return InventoryInformation;
	}

	InventoryInformation.QuestItems = InventoryComponent->QuestItems;
	InventoryInformation.CommonItems = InventoryComponent->CommonItems;
	InventoryInformation.Weapons = InventoryComponent->Weapons;
	InventoryInformation.Armors = InventoryComponent->Armors;
	InventoryInformation.Materials = InventoryComponent->Materials;
	InventoryInformation.Notes = InventoryComponent->Notes;
	return InventoryInformation;
}


FInfo_Periphery UInformationComponent::GetPeripheryInformation(ACharacterBase* Character)
{
	FInfo_Periphery PeripheryInformation = FInfo_Periphery();
	UPlayerPeripheriesComponent* PeripheriesComponent = Character ? Character->GetPeripheryComponent() : nullptr;
	if (!PeripheriesComponent)
	{
		return PeripheryInformation;
	}

	PeripheriesComponent->GetPeripheryConfig(
		PeripheryInformation.bCone, 
		PeripheryInformation.bTrace,
		PeripheryInformation.bRadius,
		PeripheryInformation.bItemDetection,
		PeripheryInformation.ActivationPhase
	);

	return PeripheryInformation;
}


FInfo_Camera UInformationComponent::GetCameraInformation(ACharacterBase* Character)
{
	FInfo_Camera CameraInformation = FInfo_Camera();
	ACharacterCameraLogic* CameraCharacter = Cast<ACharacterCameraLogic>(Character);
	if (!CameraCharacter)
	{
		return CameraInformation;
	}

	CameraInformation.CameraStyle = CameraCharacter->Execute_GetCameraStyle(CameraCharacter);
	CameraInformation.CameraOrientation = CameraCharacter->Execute_GetCameraOrientation(CameraCharacter);
	return CameraInformation;
}


FInfo_SaveState UInformationComponent::GetSaveStateInformation(ACharacterBase* Character)
{
	FInfo_SaveState SaveInformation = FInfo_SaveState();
	USaveComponent* SC = Character ? Character->GetSaveComponent() : nullptr;
	if (!SC)
	{
		return SaveState;
	}

	SaveInformation.NetId = SC->GetNetId();
	SaveInformation.PlatformId = SC->GetPlatformId();
	SaveInformation.SlotIndex = SC->GetSaveSlotIndex();
	SaveInformation.SlotIteration = SC->GetSaveIteration();
	SaveInformation.CharacterName = Character->GetName();

	SaveInformation.AttributeSaveState = SC->PrintSaveState(ESaveType::Attributes, SC->ConstructSaveSlot(SC->GetNetId(), SC->GetPlatformId(), SC->GetSaveCategory(ESaveType::Attributes), SC->GetSaveSlotIndex(), SC->GetSaveIteration()));
	SaveInformation.CombaSaveState = SC->PrintSaveState(ESaveType::Combat, SC->ConstructSaveSlot(SC->GetNetId(), SC->GetPlatformId(), SC->GetSaveCategory(ESaveType::Combat), SC->GetSaveSlotIndex(), SC->GetSaveIteration()));
	SaveInformation.CameraSettingSaveState = SC->PrintSaveState(ESaveType::CameraSettings, SC->ConstructSaveSlot(SC->GetNetId(), SC->GetPlatformId(), SC->GetSaveCategory(ESaveType::CameraSettings), SC->GetSaveSlotIndex(), SC->GetSaveIteration()));
	SaveInformation.InventorSaveState = SC->PrintSaveState(ESaveType::Inventory, SC->ConstructSaveSlot(SC->GetNetId(), SC->GetPlatformId(), SC->GetSaveCategory(ESaveType::Inventory), SC->GetSaveSlotIndex(), SC->GetSaveIteration()));
	SaveInformation.SettingSaveState = SC->PrintSaveState(ESaveType::Settings, SC->ConstructSaveSlot(SC->GetNetId(), SC->GetPlatformId(), SC->GetSaveCategory(ESaveType::Settings), SC->GetSaveSlotIndex(), SC->GetSaveIteration()));
	SaveInformation.WorlSaveState = SC->PrintSaveState(ESaveType::World, SC->ConstructSaveSlot(SC->GetNetId(), SC->GetPlatformId(), SC->GetSaveCategory(ESaveType::World), SC->GetSaveSlotIndex(), SC->GetSaveIteration()));
	return SaveInformation;
}
#pragma endregion 




#pragma region Utility
TArray<ACharacterBase*> UInformationComponent::GetAllCharacters()
{
	TArray<ACharacterBase*> Characters;

	// Retrieve a list of the current characters in the game
	if (GetWorld())
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), Actors);
		CharacterList.Empty();
		for (AActor* Actor : Actors) Characters.AddUnique(Cast<ACharacterBase>(Actor));
	}

	return Characters;
}

bool UInformationComponent::GetCharacter(ACharacterBase*& OutCharacter) const
{
	OutCharacter = Cast<ACharacterBase>(GetOwner());
	if (!OutCharacter)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the character!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
 }


 bool UInformationComponent::GetCharacterAndInventory(ACharacterBase*& OutCharacter, UInventoryComponent*& OutInventoryComponent) const
{
	if (!GetCharacter(OutCharacter))
	{
		return false;
	}

	OutInventoryComponent = OutCharacter->GetInventoryComponent();
	if (!OutInventoryComponent)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the inventory component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
 }


 bool UInformationComponent::GetCharacterAndMovementComponent(ACharacterBase*& OutCharacter, UAdvancedMovementComponent*& OutMovementComponent) const
{
	if (!GetCharacter(OutCharacter))
	{
		return false;
	}

	OutMovementComponent = OutCharacter->GetAdvancedMovementComp();
	if (!OutMovementComponent)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the movement component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
 }

 
 bool UInformationComponent::GetCharacterAndCombatComponent(ACharacterBase*& OutCharacter, UCombatComponent*& OutCombatComponent) const
{
	if (!GetCharacter(OutCharacter))
	{
		return false;
	}

	OutCombatComponent = OutCharacter->GetCombatComponent();
	if (!OutCombatComponent)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
 }

 
 bool UInformationComponent::GetCharacterAndPeripheryComponent(ACharacterBase*& OutCharacter, UPlayerPeripheriesComponent*& OutPeripheryComponent) const
{
	if (!GetCharacter(OutCharacter))
	{
		return false;
	}

	OutPeripheryComponent = OutCharacter->GetPeripheryComponent();
	if (!OutPeripheryComponent)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the periphery component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
 }

bool UInformationComponent::GetCharacterAndCameraComponent(ACharacterBase*& OutCharacter, ACharacterCameraLogic*& OutCameraComponent) const
{
	if (!GetCharacter(OutCharacter))
	{
		return false;
	}
	
	OutCameraComponent = Cast<ACharacterCameraLogic>(OutCharacter);
	if (!OutCameraComponent)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the camera component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
}


bool UInformationComponent::GetCharacterAndAbilitySystem(ACharacterBase*& OutCharacter, UAbilitySystem*& OutAbilitySystem) const
{
	if (!GetCharacter(OutCharacter))
	{
		return false;
	}
	
	OutAbilitySystem = OutCharacter->GetAbilitySystem<UAbilitySystem>();
	if (!OutAbilitySystem)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
}


bool UInformationComponent::GetCharacterAndAttributes(ACharacterBase*& OutCharacter, UAttributeSet*& OutAttributeSet, TSubclassOf<UAttributeSet> AttributeClass) const
{
	UAbilitySystem* AbilitySystemComponent;
	if (!GetCharacterAndAbilitySystem(OutCharacter, AbilitySystemComponent))
	{
		return false;
	}
	
	OutAttributeSet = const_cast<UAttributeSet*>(AbilitySystemComponent->GetAttributeSet(AttributeClass));
	if (!OutAttributeSet)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the attribute set!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
}


bool UInformationComponent::GetCharacterAndSaveComponent(ACharacterBase*& OutCharacter, USaveComponent*& OutSaveComponent) const
{
	if (!GetCharacter(OutCharacter))
	{
		return false;
	}

	OutSaveComponent = Cast<USaveComponent>(OutCharacter->GetSaveComponent());
	if (!OutSaveComponent)
	{
		UE_LOGFMT(InformationLog, Error, "{0}::{1}() {2} Failed to retrieve the save component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	return true;
 }
#pragma endregion
