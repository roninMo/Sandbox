// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

#include "GameModeLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/Components/Saving/Level/LevelSaveComponent.h"
#include "Sandbox/Characters/Player/BasePlayerState.h"
#include "Sandbox/Data/Enums/GameModeTypes.h"
#include "Sandbox/Data/Save/Save.h"
#include "Sandbox/World/Props/WorldItem.h"

DEFINE_LOG_CATEGORY(GameModeLog);



#pragma region Constructors
AMultiplayerGameMode::AMultiplayerGameMode(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	LevelSaveComponentClass = ULevelSaveComponent::StaticClass();

	// The game mode's classification
	GameModeType = EGameModeType::None;

	// Save information pertaining to each Game Mode
	CurrentSave = nullptr;
	SaveIndex = 0;

	// Levels
	LobbyLevel = "/Game/Maps/Lobby/Lobby";
	SingleplayerLevel = "/Game/Maps/BhopMap/Maps/UEDPIE_1_Demo";
}


void AMultiplayerGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	
	// Spawn the Level's save component
	if (LevelSaveComponentClass)
	{
		LevelSaveComponent = NewObject<ULevelSaveComponent>(this, LevelSaveComponentClass);
	}
	PrintMessage("PreInitializeComponents");
}


void AMultiplayerGameMode::Reset()
{
	Super::Reset();
	// InitGameState();
	PrintMessage("Reset");
}
#pragma endregion




#pragma region Match State Functions
void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PrintMessage("PostLogin");
	
	// TODO: Once the level / GameMode has been initialized, search for the owner, and retrieve the save information
	RetrieveSavePlatformId();
	
	// Init the player's save information (Save components won't save / retrieve save information until the save references are ready
	ABasePlayerState* PlayerState = NewPlayer->GetPlayerState<ABasePlayerState>(); // TODO: Dry
	if (PlayerState)
	{
		PlayerState->SetSaveUrl(GetCurrentSaveGameUrl());
		PlayerState->SetSaveIndex(SaveIndex);
		if (CurrentSave) PlayerState->SetSaveSlot(CurrentSave->SaveSlot);
	}
}


void AMultiplayerGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	PrintMessage("Handling MatchIsWaitingToStart");
}


void AMultiplayerGameMode::HandleMatchHasStarted()
{
	// BeginPlay ->	(RestartPlayers, BuildLevel, NotifyBeginPlay)  -> -> InitAbilityActorInfo
	Super::HandleMatchHasStarted();
	

	// Load the game's current save data. Level, Characters, and custom actor save data for client replication
	PrintActorsInLevel(false);

	// Custom level save logic
	ULevel* Level = GetLevel();
	if (Level)
	{
		
	}
	


	// TODO: Figure out what to do for clients that are joining games during play
	// TODO: race condition with OnInitAbilityActorInfo and both adjust the settings / saved information of the character, we need to use ClientJoinSession for initializing server information at the beginning of games
	//			- until then just use OnInitAbilityActorInfo()
	PrintMessage("Handling MatchHasStarted");
}


void AMultiplayerGameMode::HandleMatchHasEnded()
{
	
	// Save level and character information
	if (CurrentSave)
	{
		SaveGame(GetCurrentSaveGameUrl(), CurrentSave->SaveIndex);
	}

	PrintMessage("Handling MatchHasEnded");

	Super::HandleMatchHasEnded();
}


void AMultiplayerGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
	PrintMessage("Handling LeavingMap");
}


void AMultiplayerGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();
	
	// Save level and character information
	if (LevelSaveComponent)
	{
		
	}

	PrintMessage("Handling MatchAborted");
}

bool AMultiplayerGameMode::Travel(FString Map)
{
	if (!GetWorld()) return false;
	return GetWorld()->ServerTravel(Map);
}
#pragma endregion




#pragma region Save State Functions
bool AMultiplayerGameMode::SaveGame(const FString& SaveUrl, const int32 Index)
{
	// TODO: Check if we're saving to the proper level

	// Handle save information specific to multiplayer game state here (Quests, objectives, etc.)
	//	- Games with save information that persists across multiple games, or from singleplayer / multiplayer should have custom save logic for save / retrieving that information 

	
	// Level save logic
	if (LevelSaveComponent)
	{
		// TODO: Update save state to be two separate things based on whether the information is persistent
		//			- Actor->SaveToLevel() saves level information, and optionally additionally save character specific information
		//			- Actor->SaveActorData() saves character specific information
		//			- Having a function that binds to the level save component to update latent information (like inventory updates, weapon equips, etc. should be handled at all times)
		LevelSaveComponent->SaveLevel(SaveUrl, true);
	}

	// Character save logic (Multiplayer logic that isn't specific to the world)
	for (APlayerController* Player : GetPlayers())
	{
		if (!Player->GetPawn()) continue;

		// SaveLevel handles saving actor data for singleplayer / multiplayer already. Additional save state can be handled here
		
	}

	
	return true;
}


bool AMultiplayerGameMode::LoadSave(const FString& SaveUrl, const int32 Index)
{
	// Safety precautions
	if (!GetWorld()) return false;

	// Check if it's a valid save
	USave* Save = Cast<USave>(UGameplayStatics::LoadGameFromSlot(SaveUrl, 0));
	if (!Save)
	{
		UE_LOGFMT(GameModeLog, Error, "{0}() SaveUrl {1} is invalid, GameMode: {2}", *FString(__FUNCTION__), *SaveUrl, *GetName());
		return false;
	}

	// Travel to the level
	if (!GetWorld()->ServerTravel(Save->LevelUrl))
	{
		UE_LOGFMT(GameModeLog, Error, "{0}() Failed to travel to the world while trying to load a game save! {2}", *FString(__FUNCTION__), *SaveUrl, *GetName());

		// TODO: Add logic for handling server travel errors
		return false;
	}


	// Handle save information specific to multiplayer game state here (Quests, objectives, etc.)
	//	- Games with save information that persists across multiple games, or from singleplayer / multiplayer should have custom save logic for save / retrieving that information


	// Level save logic
	if (LevelSaveComponent) // TODO: Check if this is valid immediately after ServerTravel, or if there's a function we're able to bind to for loading a save before it's ready
	{
		// TODO: Update save state to be two separate things based on whether the information is persistent
		//			- Actor->SaveToLevel() saves level information, and optionally additionally save character specific information
		//			- Actor->SaveActorData() saves character specific information
		//			- Having a function that binds to the level save component to update latent information (like inventory updates, weapon equips, etc. should be handled at all times)
		LevelSaveComponent->LoadLevelSave(SaveUrl, true);
	}

	// Character save logic (Multiplayer logic that isn't specific to the world)
	for (APlayerController* Player : GetPlayers())
	{
		if (!Player->GetPawn()) continue;

		// SaveLevel handles saving actor data for singleplayer / multiplayer already. Additional save state can be handled here
		
	}

	
	return true;
}


bool AMultiplayerGameMode::SetCurrentSave(USave* Save)
{
	if (!Save)
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to update the game's save state, the save game provided was null", *FString(__FUNCTION__));
		return false;
	}
	
	CurrentSave = Save;
	SaveIndex = CurrentSave->SaveIndex;
	
	// Player States
	for (APlayerController* PlayerController : GetPlayers())
	{
		ABasePlayerState* PlayerState = PlayerController->GetPlayerState<ABasePlayerState>();
		if (PlayerState)
		{
			PlayerState->SetSaveUrl(GetCurrentSaveGameUrl());
			PlayerState->SetSaveIndex(SaveIndex);
			PlayerState->SetSaveSlot(CurrentSave->SaveSlot);
		}
	}

	return true;
}


bool AMultiplayerGameMode::CreateNextSave()
{
	if (!CurrentSave) return false;
	int32 CurrentIndex = CurrentSave->SaveIndex;

	// Create a new save, and update the game state
	USave* Save = NewObject<USave>(this, USave::StaticClass());
	if (!Save)
	{
		return false;
	}
	
	Save->SaveInformation(
		CurrentSave->SaveName,
		CurrentSave->Description,
		CurrentSave->NetId,
		CurrentSave->PlatformId,
		CurrentSave->SaveSlot,
		CurrentIndex++
	);

	
	// Additional save state here
	

	
	// Update the game mode and player state to notify the clients to start saving to the next instance
	if (!SetCurrentSave(Save))
	{
		UE_LOGFMT(GameModeLog, Warning, "!{0}() Failed to update the game's save index, an invalid save game has been provided", *FString(__FUNCTION__));
		return false;
	}

	return true;
}


bool AMultiplayerGameMode::FindCurrentSave(const FString& AccountId, int32 SaveSlot, FString& OutSaveUrl, int32& OutSaveIndex) const
{
	// Construct the SaveGameId
	int32 Index = 0;
	FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, Index);
	
	// Check if there's save information for this slot
	if (!UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() wasn't save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return false;
	}

	// Find the most recent save, and delete the saves // TODO: we should keep a reference for a specific character, and have everything else be handled naturally
	int32 SearchIndex = Index;
	int32 SearchStep = 100;
	bool bFoundCurrentSaveIndex = false;
	int32 bSaveSearchErrorCount = 0;
	
	FString LastValidSaveUrl = SaveUrl;
	int32 LastValidSaveIndex = 0;
	while (!bFoundCurrentSaveIndex || bSaveSearchErrorCount < 5)
	{
		SearchIndex = SearchIndex + SearchStep;
		SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, SearchIndex);

		// If there's still save slots
		if (UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
		{
			LastValidSaveUrl = SaveUrl;
			LastValidSaveIndex = SearchIndex;
			continue;
		}
		
		// If the search step is one, this is the most current save
		if (SearchStep == 1)
		{
			bFoundCurrentSaveIndex = true;
			SearchIndex = SearchIndex - 1;

			bSaveSearchErrorCount++;
		}
		else // Backstep and search again  // IndexToFind = 479 ->  100, 200, 300, 400, 500/, 450, 500/, 475, etc.
		{
			SearchIndex = SearchIndex - SearchStep;
			SearchStep = SearchStep / 2;
			// if (SearchStep == 0) SearchStep = 1;
		}
	}

	// An error occured 
	if (bSaveSearchErrorCount >= 5)
	{
		OutSaveUrl = LastValidSaveUrl;
		OutSaveIndex = LastValidSaveIndex;
	}
	else
	{
		OutSaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, SearchIndex);
		OutSaveIndex = SearchIndex;
	}
	return true;
}


TArray<FString> AMultiplayerGameMode::FindPreviousSaves(const FString& AccountId, int32 SaveSlot, int32 CurrentSaveIndex, int32 SavesToRetrieve) const
{
	TArray<FString> PreviousSaves;
	for (int32 Index = CurrentSaveIndex - 1; Index >= 0; Index--)
	{
		// Check if it's a valid save
		FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, Index);
		if (UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
		{
			PreviousSaves.AddUnique(SaveUrl);
		}

		if (PreviousSaves.Num() >= SavesToRetrieve)
		{
			return PreviousSaves; 
		}
	}

	return PreviousSaves;
}


bool AMultiplayerGameMode::DeleteSaveSlot(const FString& AccountId, int32 SaveSlot)
{
	// Construct the SaveGameId
	int32 Index = 0;
	FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, Index);

	// Check if there's save information for this slot
	if (!UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() wasn't save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return true;
	}

	// Find the most recent save, and delete the saves
	FString CurrentSaveUrl;
	int32 CurrentSaveIndex = 0;
	if (!FindCurrentSave(AccountId, SaveSlot, CurrentSaveUrl, CurrentSaveIndex))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() couldn't find the current save information in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return true;
	}

	// If that didn't break anything, let's just delete all the previous saves
	for (int32 i = CurrentSaveIndex; CurrentSaveIndex > 0; i--)
	{
		FString SaveToDelete = ConstructSaveGameUrl(AccountId, SaveSlot, i);
		UGameplayStatics::DeleteGameInSlot(SaveToDelete, 0);
	}
	
	return true;
}


USave* AMultiplayerGameMode::NewSaveSlot(const FString& AccountId, int32 SaveSlot)
{
	// Construct the SaveGameId
	FString SaveUrl = ConstructSaveGameUrl(AccountId, SaveSlot, 0);

	// Check if there's already save information for this slot
	if (UGameplayStatics::DoesSaveGameExist(SaveUrl, 0))
	{
		UE_LOGFMT(GameModeLog, Error, "!{0}() Unable to create a new save, there's already a save in slot {1}, SaveUrl: {2}", *FString(__FUNCTION__), SaveSlot, *SaveUrl);
		return nullptr;
	}

	// Create the initial save
	USave* Save = NewObject<USave>(this, USave::StaticClass());
	if (Save)
	{
		Save->SaveInformation(AccountId, "", -1, AccountId, SaveSlot, 0);
	}

	return Save;
}


FString AMultiplayerGameMode::GetCurrentSaveGameUrl(const bool bLevel) const
{
	if (!CurrentSave)
	{
		return FString();
	}

	return ConstructSaveGameUrl(CurrentSave->PlatformId, CurrentSave->SaveSlot, CurrentSave->SaveIndex, bLevel);
}


FString AMultiplayerGameMode::ConstructSaveGameUrl(const FString& PlatformId, const int32 Slot, const int32 Index, bool bLevel) const
{
	// - LevelSaveUrl
	// 	- SP -> Adventure_Character1_S1_54_Level_
	// 	- MP -> MP_CustomLevel1_
	
	// - CharacterSaveUrl
	// 	- SP -> Adventure_Character1_S1_54_Character1_
	// 	- MP -> retrieved from an api during join (there isn't a way of handling this right now with the editor using the account / platform id) 

	// (GameMode + PlatformId + SlotId + SaveIndex ++ additional SaveComponent logic for each actor)
	FString SaveUrl = UGameModeLibrary::GameModeTypeToString(GameModeType)
			.Append("_")
			.Append(PlatformId) // TODO: Dedicated multiplayer servers need logic for retrieving the host, or separate functionality for multiplayer
			.Append("_")
			.Append("S").Append(FString::FromInt(Slot))
			.Append("_")
			.Append(FString::FromInt(Index));

	// If this isn't a save specific to a character, and should be saved on the level
	if (bLevel) SaveUrl += "_Level";
	
	ENetMode NetMode = GetNetMode();
	if (GameModeType == EGameModeType::Adventure)
	{
		// Multiplayer
		if (NetMode != NM_Standalone)
		{
			// Subclass this for multiplayer saves that aren't specific to a level, and add character save logic additionally
		}
		else // Singleplayer
		{
			
		}
	}
	else // All other modes are multiplayer
	{
		// TODO: Retrieve from level information, if there is any save data
		// 	Characters -> retrieved from an api during join (there isn't a way of handling this right now with the editor using the account / platform id) 
		// 	Level -> MP_CustomLevel1_
	}
	
	return SaveUrl;
	
	/**

	 SinglePlayer
		- Adventure
			- Specific levels, and save game state for each game

	 Multiplayer
		- Team Deathmatch, Free for All, etc.
		- Custom Games
			- Both could have a custom level, normal multiplayer would just have traditional logic


	Lobby
		- GameMode, Level, CustomSaveState/Level Information, GameMode logic for the type of game ->  ready for play

	// SaveGameBase: (GameMode + PlatformId + SlotId + SaveIndex ++ additional SaveComponent logic for each actor)
	// Retrieve the Save Iteration individually for fallbacks in case certain save components didn't save properly
		// SaveGameBase + "_" Iteration + "_" + Individual Save Component References
		// SaveGameBase + "_" Iteration + "_" + Level + Prop / Object References -> Individual Save Component

	Singleplayer
		- Adventure_Character1_S1_54
			Character:
				->  Adventure_Character1_S1_54_Character1_Combat
				->  Adventure_Character1_S1_54_Character1_Inventory
				->  Adventure_Character1_S1_54_Character1_CameraSettings
				->  Adventure_Character1_S1_54_Character1_Attributes

			Actor: 
				->  Adventure_Character1_S1_54_Level
					->  Adventure_Character1_S1_54_Level_Actor0
						->  Adventure_Character1_S1_54_Level_Actor0_etc

		Actors placed in the level with save information: (SaveComponent)
				-> Adventure_Character1_S1_54_Level + _Actor0
					-> Adventure_Character1_S1_54_Level + _Actor0_Combat
					-> Adventure_Character1_S1_54_Level + _Actor0_Inventory
				-> Adventure_Character1_S1_54_Level + _Actor1
					-> Adventure_Character1_S1_54_Level + _Actor1_Combat
					-> Adventure_Character1_S1_54_Level + _Actor1_Inventory

		Actors spawned in the world with save logic:
			-> Adventure_Character1_S1_54_Level + _ActorSpawnRef_Combat
			-> Adventure_Character1_S1_54_Level + _ActorSpawnRef_Inventory

	Cooperative
		- When players join the lobby, online subsystems would retrieve character information individually from the server
			- If we're still using SaveGame state on the server, it would be retrieved in reference to the host's character reference, and what he saved for each character (using their Account/Platform Id)
				- Adventure_Character1_S1_54
				- Adventure_Character2_S1_54 -> and this breaks the logic
					- If it's a player and it isn't the host, the save logic will treat it as a player, and save and load it's information the same way, let's just add it to the save like this:
					- Adventure_Character1_S1_54 + _Character2 -> and use this as the save game reference
				- So we use the owner's Account/PlatformId for the Save Reference, and additionally for their own save information
					- Adventure_Character1_S1_54_Character1
					- Adventure_Character1_S1_54_Character2
					- Adventure_Character1_S1_54_Character3
					


	Multiplayer (for save state, we're probably just not going to use a character reference, unless it's a custom game mode. we'll figure out how to handle this later)
		- MP_CustomLevel1
		- MP_CustomLevel2
			Props, Actors, Enemies, Weapons, GameMode Objects, Vehicles, etc spawned in the world should be handled separately here before we handle character logic, on client / server, retrieved from the specified level, spawn presets, etc.
				-> MP_CustomLevel1_Props_Ramp (The level's custom components probably won't be spawned on the client automatically)
				-> MP_CustomLevel1_Props_Object1
				-> MP_CustomLevel1_Actors_ShieldPickup_1
					-> MP_CustomLevel1_Actors_ShieldPickup_1_Level (Transform)
				-> MP_CustomLevel1_Actors_WeaponCrate_0
					-> MP_CustomLevel1_Actors_WeaponCrate_0_Level
					-> MP_CustomLevel1_Actors_WeaponCrate_0_Inventory

		// The benefit of saving this way is effecient retrieving of information individually, and that doesn't necessarily mean that this is correct. Finding a way to save each level in their own folder should help make sense of it.
		//		However with online subsystem logic, the information would be saved and retrieved asynchronously from the server while the level information works this way
		//			So you'd just have to add that logic to the save component for properly saving / retrieving the information


	->  SaveGame Reference for retrieving Level / Character information
		- OnMatchBegin build the level, character / spawned information on server, and spawned level objects on both Client and Server 

	 */
}


USave* AMultiplayerGameMode::GetSaveGameBase()
{
	return CurrentSave;
}


void AMultiplayerGameMode::SetSaveGameBase(USave* Save)
{
	CurrentSave = Save;
}


FString AMultiplayerGameMode::GetSavePlatformId() const
{
	return SavePlatformId;
}


void AMultiplayerGameMode::RetrieveSavePlatformId()
{
	SavePlatformId = FGenericPlatformMisc::GetLoginId();
}
#pragma endregion




#pragma region Respawning
void AMultiplayerGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}


void AMultiplayerGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}
#pragma endregion




#pragma region Utility
EGameModeType AMultiplayerGameMode::GetGameModeType()
{
	return GameModeType;
}


TArray<APlayerController*> AMultiplayerGameMode::GetPlayers() const
{
	TArray<APlayerController*> Players;
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			Players.Add(PlayerController);
		}
	}

	return Players;
}


void AMultiplayerGameMode::PrintMessage(const FString& Message)
{
	for(APlayerController* PlayerController : GetPlayers())
	{
		UE_LOGFMT(GameModeLog, Warning, "{0}::{1}() {2}({3}) ->  {4}", *UEnum::GetValueAsString(PlayerController->GetLocalRole()), *FString(__FUNCTION__), GetNameSafe(PlayerController), GetName(), Message);
	}
}


void AMultiplayerGameMode::PrintActorsInLevel(bool bSavedAndLevelActors)
{
	UWorld* World = GetWorld();

	// Update actors in level with saved state information
	TArray<AActor*> LevelActors;
	TArray<ILevelSaveInformationInterface*> SavedActors;
	TArray<AActor*> Actors = GetLevel()->Actors;
	UE_LOGFMT(LogTemp, Log, " ");
	UE_LOGFMT(LogTemp, Log, "//--------------------------------------------------------------------------------------------//");
	UE_LOGFMT(LogTemp, Log, "// Printing the actors of the level");
	UE_LOGFMT(LogTemp, Log, "//--------------------------------------------------------------------------------------------//");
	for (AActor* Actor : Actors)
	{
		ILevelSaveInformationInterface* SavedActor = Cast<ILevelSaveInformationInterface>(Actor);
		if (SavedActor) SavedActors.Add(SavedActor);
		else LevelActors.Add(Actor);
	}

	UE_LOGFMT(LogTemp, Log, " ");
	UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
	UE_LOGFMT(LogTemp, Log, "// Saved actors list");
	UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
	for (ILevelSaveInformationInterface* SavedActor : SavedActors)
	{
		AActor* Actor = Cast<AActor>(SavedActor);
		if (!Actor) continue;

		UE_LOGFMT(LogTemp, Log, "// Id: {0}, Saved: {1}, uniqueId: {2}", *SavedActor->Execute_GetActorLevelId(Actor), *GetNameSafe(Actor), Actor->GetUniqueID());
	}
	UE_LOGFMT(LogTemp, Log, "//----------------------------------------//");

	if (bSavedAndLevelActors)
	{
		UE_LOGFMT(LogTemp, Log, " ");
		UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
		UE_LOGFMT(LogTemp, Log, "// Level actors list");
		UE_LOGFMT(LogTemp, Log, "//----------------------------------------------//");
		for (AActor* LevelActor : LevelActors)
		{
			UE_LOGFMT(LogTemp, Log, "// Level: {0}", *GetNameSafe(LevelActor));
		}
		UE_LOGFMT(LogTemp, Log, "//----------------------------------------//");
	}
	
}
#pragma endregion 