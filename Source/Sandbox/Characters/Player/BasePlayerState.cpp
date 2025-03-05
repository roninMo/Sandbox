// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Sandbox/Game/MultiplayerGameState.h"


#pragma region Constructors
void ABasePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABasePlayerState, SaveUrl, COND_AutonomousOnly);
	DOREPLIFETIME_CONDITION(ABasePlayerState, SaveSlot, COND_AutonomousOnly);
	DOREPLIFETIME_CONDITION(ABasePlayerState, SaveIndex, COND_AutonomousOnly);
}


ABasePlayerState::ABasePlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//////////////////////// Replication stuff (Server/Client rendering) 
	NetUpdateFrequency = 66.f; // default update character on other machines 66 times a second (general fps defaults)
	MinNetUpdateFrequency = 33.f; // To help with bandwidth and lagginess, allow a minNetUpdateFrequency, which is generally 33 in fps games
	// The other value is the server config tick rate, which is in the project defaultEngine.ini -> [/Script/OnlineSubsystemUtils.IpNetDriver] NetServerMaxTickRate = 60
	// also this which is especially crucial for implementing the gameplay ability system defaultEngine.ini -> [SystemSettings] net.UseAdaptiveNetUpdateFrequency = 1
}
#pragma endregion


void ABasePlayerState::Server_SaveGame_Implementation(const FString& Ref, const int32 Index)
{
	// UGameInstance* GameInstance = GetGameInstance();
	// AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	AMultiplayerGameState* GameState = Cast<AMultiplayerGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		SaveUrl = Ref;
		GameState->SaveGameState(Ref, Index);
	}
}



#pragma region Utility
FString ABasePlayerState::GetSaveGameRef() const
{
	return SaveUrl;
}

int32 ABasePlayerState::GetSaveIndex() const
{
	return SaveIndex;
}

int32 ABasePlayerState::GetSaveSlot() const
{
	return SaveSlot;
}

void ABasePlayerState::SetSaveSlot(const int32 Slot)
{
	SaveSlot = Slot;
}

void ABasePlayerState::SetSaveUrl(FString Url)
{
	SaveUrl = Url;
}

void ABasePlayerState::SetSaveIndex(const int32 Index)
{
	SaveIndex = Index;
}
#pragma endregion