// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

ABasePlayerState::ABasePlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//////////////////////// Replication stuff (Server/Client rendering) 
	NetUpdateFrequency = 66.f; // default update character on other machines 66 times a second (general fps defaults)
	MinNetUpdateFrequency = 33.f; // To help with bandwidth and lagginess, allow a minNetUpdateFrequency, which is generally 33 in fps games
	// The other value is the server config tick rate, which is in the project defaultEngine.ini -> [/Script/OnlineSubsystemUtils.IpNetDriver] NetServerMaxTickRate = 60
	// also this which is especially crucial for implementing the gameplay ability system defaultEngine.ini -> [SystemSettings] net.UseAdaptiveNetUpdateFrequency = 1
}
