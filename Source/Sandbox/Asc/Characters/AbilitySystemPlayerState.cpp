// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemPlayerState.h"

#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeLogic.h"

AAbilitySystemPlayerState::AAbilitySystemPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystem>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	
	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<UMMOAttributeLogic>(TEXT("AttributeSet"));
	
	//////////////////////// Replication stuff (Server/Client rendering) 
	NetUpdateFrequency = 66.f; // default update character on other machines 66 times a second (general fps defaults)
	MinNetUpdateFrequency = 33.f; // To help with bandwidth and lagginess, allow a minNetUpdateFrequency, which is generally 33 in fps games
	// The other value is the server config tick rate, which is in the project defaultEngine.ini -> [/Script/OnlineSubsystemUtils.IpNetDriver] NetServerMaxTickRate = 60
	// also this which is especially crucial for implementing the gameplay ability system defaultEngine.ini -> [SystemSettings] net.UseAdaptiveNetUpdateFrequency = 1
}


UAbilitySystemComponent* AAbilitySystemPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
