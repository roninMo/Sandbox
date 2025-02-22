// Fill out your copyright notice in the Description page of Project Settings.


#include "InformationComponent.h"

#include "Blueprint/UserWidget.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Characters/Components/Camera/CharacterCameraLogic.h"
#include "Sandbox/Characters/Components/Periphery/PeripheryComponent.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/Combat/CombatComponent.h"

DEFINE_LOG_CATEGORY(InformationLog);


UInformationComponent::UInformationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}


void UInformationComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacterBase* Character;
	if (GetCharacter(Character))
	{
		// Construct the controlled player's information
		if (Character->IsLocallyControlled() && InformationWidgetClass && !InformationWidget)
		{
			InformationWidget = CreateWidget(Character->GetPlayerController<APlayerController>(), InformationWidgetClass, FName("Client/Server Information Widget"));
		}
	}
}


void UInformationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
