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

	bUseSaveInformation = true;
	bSaveOnEndPlay = true;
}


void USaveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Save and delete the save data
	if (bSaveOnEndPlay)
	{
		for (auto &[SaveState, SaveLogic] : SaveLogicComponents)
		{
			if (!SaveLogic) continue;
			SaveLogic->SaveData();
		}
	}

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

	// Initialize the save config for proper saving / loading
	InitializeSaveSlotConfig();
	
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
	// delete the save components
	if (bSaveOnEndPlay)
	{
		for (auto &[SaveState, SaveLogic] : SaveLogicComponents)
		{
			if (!SaveLogic) continue;
			SaveLogic->ConditionalBeginDestroy();
		}
		
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

	// TODO: Add singleplayer / multiplayer logic for handling saving and loading information from servers/subsystems @ref USaveLogic
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (Character)
	{
		// Attributes (if valid, send the ability system the saved stats)
		LoadData(ESaveType::Attributes);

		// World information (Location and rotation, etc)
		// LoadData(ESaveType::World);
		
		// Inventory (load the inventory and send the information to the clients)
		LoadData(ESaveType::Inventory);
		
		// Combat Component (equip weapons and armor)
		LoadData(ESaveType::Combat);

		// Camera Settings (adjust camera settings on server, allow character to handle replication)
		LoadData(ESaveType::CameraSettings);
	}

	// TODO: Add proper loading of the world / level on the server based on the owner of the lobby / game mode, and settings when the player opens / updates the settings

	BP_LoadPlayerInformation();
}

bool USaveComponent::LoadData(const ESaveType InformationType)
{
	if (!SaveLogicComponents.Contains(InformationType))
	{
		return false;
	}

	if (PreventingLoadingFor(InformationType))
	{
		return false;
	}
	
	return SaveLogicComponents[InformationType]->LoadData();
}


bool USaveComponent::PreventingLoadingFor(const ESaveType SaveType) const
{
	if (!PreventLoading.Contains(SaveType)) return false;
	return PreventLoading[SaveType];
}


void USaveComponent::InitializeSaveSlotConfig()
{
	SetNetAndPlatformId();
	SetSaveSlotIndex(0);
}


FString USaveComponent::GetCurrentSaveSlot(const ESaveType Saving, const int32 SlotIndex) const
{
	// FString("CharacterId_SaveCategory_SaveSlotIndex_IterationAndAutoSaveIndex");
	return ConstructSaveSlot(GetNetId(), GetPlatformId(), GetSaveCategory(Saving), SlotIndex, GetSaveIteration());
}

FString USaveComponent::ConstructSaveSlot(int32 NetDriverId, FString AccountPlatformId, FString SaveCategory, int32 SlotIndex, int32 SaveIteration) const
{
	return AccountPlatformId.Append("_")
		.Append(SaveCategory).Append("_")
		.Append(FString::FromInt(SlotIndex)).Append("_")
		.Append(FString::FromInt(SaveIteration));
}


int32 USaveComponent::GetNetId() const
{
	return NetId;
}


FString USaveComponent::GetPlatformId() const
{
	return PlatformId;
}


FString USaveComponent::GetSaveCategory(const ESaveType SaveType) const
{
	if (SaveType == ESaveType::World) return "World";
	if (SaveType == ESaveType::Inventory) return "Inventory";
	if (SaveType == ESaveType::Combat) return "Combat";
	if (SaveType == ESaveType::Settings) return "Settings";
	if (SaveType == ESaveType::CameraSettings) return "CameraSettings";
	return "None";
}


int32 USaveComponent::GetSaveSlotIndex() const
{
	return SaveSlotIndex;
}


int32 USaveComponent::GetSaveIteration() const
{
	return 0;
}


void USaveComponent::SetNetAndPlatformId()
{
	NetId = -1;
	PlatformId = GetOwner() ? GetOwner()->GetName() : "Null";
}


void USaveComponent::SetSaveSlotIndex(const int32 Index)
{
	SaveSlotIndex = Index;
}


bool USaveComponent::HandlesSaving(const ESaveType SaveType) const
{
	return SaveConfigurations.Contains(SaveType);
}


TArray<ESaveType> USaveComponent::GetSaveTypes() const
{
	TArray<ESaveType> SaveTypes;
	for (auto &[SaveType, SaveLogic] : SaveLogicComponents)
	{
		SaveTypes.Add(SaveType);
	}

	return SaveTypes;
}


FString USaveComponent::PrintSaveState(const ESaveType SaveType, const FString Slot) const
{
	if (!SaveLogicComponents.Contains(SaveType)) return FString();
	const USaveLogic* SaveLogic = SaveLogicComponents[SaveType];

	return SaveLogic->FormattedSaveInformation(Slot);
}
