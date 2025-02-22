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


FInfo_Movement UInformationComponent::GetMovementInformation(ACharacterBase* Character)
{
	FInfo_Movement Movement;
	UAdvancedMovementComponent* MovementComponent = Character->GetAdvancedMovementComp();
	if (!MovementComponent)
	{
		return Movement;
	}

	// state
	Movement.MovementMode = MovementComponent->GetMovementMode();
	Movement.bAiming = MovementComponent->IsAiming();
	Movement.bCrouching = MovementComponent->IsCrouching();
	Movement.bAcceling = MovementComponent->Velocity.Size() > 0;
	Movement.Input = MovementComponent->GetPlayerInput();
	Movement.Acceleration = MovementComponent->GetCurrentAcceleration();
	Movement.Velocity = MovementComponent->GetLastUpdateVelocity();
	Movement.Speed = MovementComponent->Velocity.Size();
	Movement.Rotation = Character->GetActorRotation();
	Movement.DirectionalVelocity = UKismetMathLibrary::Quat_UnrotateVector(Movement.Rotation.Quaternion(), Movement.Velocity);
	Movement.AimRotation = Movement.DirectionalVelocity.ToOrientationRotator();;

	// bhop
	Movement.AirStrafe_MaxAcceleration = MovementComponent->StrafingMaxAcceleration;
	Movement.AirStrafe_SpeedGainMultiplier = MovementComponent->AirStrafeSpeedGainMultiplier;
	Movement.AirStrafe_RotationRate = MovementComponent->AirStrafeRotationRate;
	Movement.StrafeSway_Duration = MovementComponent->StrafeSwayDuration;
	Movement.StrafeSway_SpeedGainMultiplier = MovementComponent->StrafeSwaySpeedGainMultiplier;
	Movement.StrafeSway_RotationRate = MovementComponent->StrafeSwayRotationRate;
	Movement.StrafeLurch_Duration = MovementComponent->StrafeLurchDuration;
	Movement.StrafeLurch_FullStrengthDuration = MovementComponent->StrafeLurchFullStrengthDuration;
	Movement.StrafeLurch_Strength = MovementComponent->StrafeLurchStrength;
	Movement.StrafeLurch_Friction = MovementComponent->StrafeLurchFriction;

	// config
	Movement.bUseBhopping = MovementComponent->bUseBhopping;
	Movement.bUseWallJumping = MovementComponent->bUseWallJumping;
	Movement.bUseMantleJumping = MovementComponent->bUseMantleJumping;
	Movement.bUseWallClimbing = MovementComponent->bUseWallClimbing;
	Movement.bUseMAntling = MovementComponent->bUseMantling;
	Movement.bUseLedgeClimbing = MovementComponent->bUseLedgeClimbing;
	Movement.bUseWallRunning = MovementComponent->bUseWallRunning;
	Movement.bUseSliding = MovementComponent->bUseSliding;
	Movement.MoveSpeed = MovementComponent->Velocity.Size();
	Movement.JumpHeight = MovementComponent->JumpZVelocity;
	Movement.AirBrakingDeceleration = MovementComponent->BrakingDecelerationFalling;
	Movement.WalkingBrakingDeceleration = MovementComponent->BrakingDecelerationWalking;
	Movement.Gravity = MovementComponent->GravityScale;

	return Movement;
}


FInfo_Combat UInformationComponent::GetCombatInformation(ACharacterBase* Character)
{
	FInfo_Combat CombatInformation = FInfo_Combat();
	UCombatComponent* CombatComponent = Character->GetCombatComponent();
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
	UInventoryComponent* InventoryComponent = Character->GetInventoryComponent();
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
	UPlayerPeripheriesComponent* PeripheriesComponent = Character->GetPeripheryComponent();
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
	FInfo_SaveState SaveState = FInfo_SaveState();
	USaveComponent* SaveComponent = Character->GetSaveComponent();
	if (!SaveComponent)
	{
		return SaveState;
	}

	return SaveState;
}




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
