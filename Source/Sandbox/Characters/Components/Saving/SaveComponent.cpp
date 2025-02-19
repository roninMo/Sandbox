// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveComponent.h"

#include "SaveLogic.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Enums/ESaveType.h"

DEFINE_LOG_CATEGORY(SaveComponentLog);


USaveComponent::USaveComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(false);
	SetComponentTickInterval(10); // TODO: check that this is happening every 10 seconds, I don't know if it's time or based on the player's fps
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_EndPhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}


void USaveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeleteSaveStates();
	Super::EndPlay(EndPlayReason);
}


void USaveComponent::BeginPlay()
{
	Super::BeginPlay();
	// InitializeSaveLogic should be called during the game mode's PostLogin() function. The player state and controller is valid then, however it won't allow for remote procedure calls yet
	//		That's okay because we just want the save information valid before/during when the player joins the game 
}


void USaveComponent::AutoSaveLogic()
{
	if (SaveLogicComponents.IsEmpty())
	{
		return;
	}

	for (auto &[SaveState, SaveLogic] : SaveLogicComponents)
	{
		if (!SaveLogic) continue;

		// Autosave any of the save data that needs to be autosaved
		if (SaveLogic->IsValidAutoSave())
		{
			SaveLogic->SaveData();
		}
	}
}


void USaveComponent::TickComponent(const float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// AutoSaveLogic();
}


bool USaveComponent::InitializeSaveLogic()
{
	// Sanity checks
	if (!GetOwner())
	{
		UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() Failed to retrieve the owner while creating the save states for {2}!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(GetOwner())
		);
		return false;
	}

	// Only add save states on the server
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// Initialization has already been called
	if (!SaveLogicComponents.IsEmpty())
	{
		return true;
	}

	// Check that the save configuration is valid and create the classes for saving the actor's logic
	for (auto &[SaveState, Configuration] : SaveConfigurations)
	{
		if (!Configuration.IsValid() || !Configuration.SaveLogicClass)
		{
			UE_LOGFMT(SaveComponentLog, Error, "{0} {1}() {2} tried to create save logic for {3} with an invalid configuration!",
				*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__),
				GetNameSafe(GetOwner()), *UEnum::GetValueAsString(SaveState)
			);
			
			continue; // return false;
		}

		USaveLogic* SaveLogic = NewObject<USaveLogic>(this, Configuration.SaveLogicClass, Configuration.DisplayName);
		check(SaveLogic);
		SaveLogic->SetSaveLogicInformation(Configuration);
		SaveLogicComponents.Add(SaveState, SaveLogic);

	}
	// TODO: Handle loading information or state to prevent saving until the stored information has been properly replicated to the clients


	return true;
}


void USaveComponent::DeleteSaveStates()
{
	// TODO: Check that it's safe to save while pending construction!
	// Save and delete the save data
	for (auto &[SaveState, SaveLogic] : SaveLogicComponents)
	{
		if (!SaveLogic) continue;
		SaveLogic->SaveData();
		SaveLogic->ConditionalBeginDestroy();
	}

	SaveLogicComponents.Empty();
}


bool USaveComponent::SaveData(const ESaveType Saving)
{
	// Only save on the server
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// If this actor has logic for saving this information
	if (!SaveLogicComponents.Contains(Saving))
	{
		return false;
	}

	// Save the information, handle additional logic in blueprint
	bool bSuccessfullySaved = SaveLogicComponents[Saving]->SaveData();
	BP_SaveData(Saving, bSuccessfullySaved);
	return bSuccessfullySaved;
}


bool USaveComponent::IsValidToSave(const ESaveType InformationType)
{
	if (!SaveLogicComponents.Contains(InformationType)) return false;
	if (!SaveLogicComponents[InformationType]->IsValidToSave()) return false;

	return true;
}


void USaveComponent::LoadPlayerInformation()
{
	if (!bUseSaveInformation) return;
	// Notify components to load the player information once the player has initialized and is ready to save / load it's information

	// TODO: Add singleplayer / multiplayer logic for handling saving and loading information from servers/subsystems
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (Character)
	{
		// Attributes (if valid, send the ability system the saved stats)
		if (SaveLogicComponents.Contains(ESaveType::Attributes) && !PreventingLoadingFor(ESaveType::Attributes))
		{
			SaveLogicComponents[ESaveType::Attributes]->LoadData();
		}

		// Inventory (load the inventory and send the information to the clients)
		if (SaveLogicComponents.Contains(ESaveType::Inventory) && !PreventingLoadingFor(ESaveType::Inventory))
		{
			SaveLogicComponents[ESaveType::Inventory]->LoadData();
		}
		
		// Combat Component (equip weapons and armor)
		if (SaveLogicComponents.Contains(ESaveType::Combat) && !PreventingLoadingFor(ESaveType::Combat))
		{
			SaveLogicComponents[ESaveType::Combat]->LoadData();
		}

		// Camera Settings (adjust camera settings on server, allow character to handle replication)
		if (SaveLogicComponents.Contains(ESaveType::CameraSettings) && !PreventingLoadingFor(ESaveType::CameraSettings))
		{
			SaveLogicComponents[ESaveType::CameraSettings]->LoadData();
		}
	}

	// TODO: Add proper loading of the world / level on the server based on the owner of the lobby / game mode, and settings when the player opens / updates the settings

	BP_LoadPlayerInformation();
}


bool USaveComponent::PreventingLoadingFor(const ESaveType SaveType) const
{
	if (!PreventLoading.Contains(SaveType)) return false;
	return PreventLoading[SaveType];
}


FString USaveComponent::GetSaveSlotIdReference(const ESaveType Saving) const
{
	return FString();
}


FName USaveComponent::GetPlayerNetId() const
{
	// TODO: Fix net id reference retrieval to be a blueprint library function so we can safely handle juggling networking and steam / console references for retrieving the network id
	/** The platform id returns the console, and the network id is tricky and specific to the current session. We also need access to the player's account for multiplayer,
	 *		and we shouldn't really factor in the network id for saving, and still need another unique reference to each player that's on their console, and perhaps factor out the platform id
	 */
	return FName();
}


FString USaveComponent::GetSaveTypeName(const ESaveType SaveType) const
{
	if (SaveType == ESaveType::World) return "World";
	if (SaveType == ESaveType::Inventory) return "Inventory";
	if (SaveType == ESaveType::Combat) return "Combat";
	if (SaveType == ESaveType::Settings) return "Settings";
	if (SaveType == ESaveType::CameraSettings) return "CameraSettings";
	return "SaveType";
}
