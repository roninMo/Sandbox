// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveComponent.h"

#include "SaveLogic.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"

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
	// InitializeSavingLogic should be called during the game mode's PostLogin() function. The player state and controller is valid then, however it won't allow for remote procedure calls yet
	//		That's okay because we just want the save information valid before/during when the player joins the game 
}


void USaveComponent::AutoSaveLogic()
{
	if (SavingLogic.IsEmpty())
	{
		return;
	}

	for (auto &[SaveState, SaveLogic] : SavingLogic)
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
	AutoSaveLogic();
}


bool USaveComponent::InitializeSavingLogic()
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
	if (!SavingLogic.IsEmpty())
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

		USaveLogic* SaveLogic = NewObject<USaveLogic>(this, Configuration.SaveLogicClass, Configuration.Name);
		check(SaveLogic);

		SavingLogic.Add(SaveState, SaveLogic);

	}
	// TODO: Handle loading information or state to prevent saving until the stored information has been properly replicated to the clients


	return true;
}

void USaveComponent::DeleteSaveStates()
{
	// TODO: Check that it's safe to save while pending construction!
	// Save and delete the save data
	for (auto &[SaveState, SaveLogic] : SavingLogic)
	{
		if (!SaveLogic) continue;
		SaveLogic->SaveData();
		SaveLogic->BeginDestroy();
	}

	SavingLogic.Empty();
}


bool USaveComponent::SaveData(const ESaveType Saving)
{
	// Only save on the server
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// If this actor has logic for saving this information
	if (!SavingLogic.Contains(Saving))
	{
		return false;
	}

	// Save the information, handle additional logic in blueprint
	bool bSuccessfullySaved = SavingLogic[Saving]->SaveData();
	BP_SaveData(Saving, bSuccessfullySaved);
	return bSuccessfullySaved;
}


bool USaveComponent::IsValidToSave(const ESaveType InformationType)
{
	if (!SavingLogic.Contains(InformationType)) return true;
	if (!SavingLogic[InformationType]->IsValidToSave()) return false;

	return true;
}


void USaveComponent::LoadPlayerInformation()
{
	// Notify components to load the player information once the player has initialized and is ready to save and load it's information

	// Default Character logic -> TODO: prevent dependency problems
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (Character)
	{
		// Attributes (if valid, send the ability system the saved stats)

		// Combat

		// CameraSettings

		// Inventory

	}

	// TODO: Add proper loading of the world / level on the server based on the owner of the lobby / game mode, and settings when the player opens / updates the settings

	BP_LoadPlayerInformation();
}


FName USaveComponent::GetSaveTypeIdReference(const ESaveType Saving)
{
	return FName();
}


FName USaveComponent::GetPlayerNetId() const
{
	// TODO: Fix net id reference retrieval to be a blueprint library function so we can safely handle juggling networking and steam / console references for retrieving the network id
	/** The platform id returns the console, and the network id is tricky and specific to the current session. We also need access to the player's account for multiplayer,
	 *		and we shouldn't really factor in the network id for saving, and still need another unique reference to each player that's on their console, and perhaps factor out the platform id
	 */
	return FName();
}
