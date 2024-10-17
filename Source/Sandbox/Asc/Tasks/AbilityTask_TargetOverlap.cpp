// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TargetOverlap.h"

#include "Sandbox/Asc/AbilitySystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/AI/Characters/Enemy.h"
#include "Sandbox/Combat/Weapons/Armament.h"

UAbilityTask_TargetOverlap* UAbilityTask_TargetOverlap::CreateOverlapDataTask(UGameplayAbility* OwningAbility, const TArray<AArmament*> Armaments, const bool bDebug)
{
	UAbilityTask_TargetOverlap* Task = NewAbilityTask<UAbilityTask_TargetOverlap>(OwningAbility);
	Task->Armaments = Armaments;
	Task->bDebugTask = bDebug;
	return Task;
}


void UAbilityTask_TargetOverlap::Activate()
{
	Super::Activate();
	SetWaitingOnAvatar();

	// One interesting thing that the ability system component does for handling client side prediction is create a way to reference sending information from client to server using ability tasks and a target data object
	// The way you handle it is by sending the information on the client while listening for it on the server when an ability has been activated and prediction is valid, here's how you should go about it

	const AActor* Character = Ability && Ability->GetCurrentActorInfo() ? Ability->GetCurrentActorInfo()->AvatarActor.Get() : nullptr;
	UAbilitySystem* ASC = Cast<UAbilitySystem>(AbilitySystemComponent.Get());
	if (!Character || !ASC || Armaments.IsEmpty())
	{
		if (!ASC) UE_LOGFMT(AbilityLog, Error, "{0}() The ability system wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
		if (Armaments.IsEmpty()) UE_LOGFMT(AbilityLog, Error, "{0}() The armament wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
		if (!Character) UE_LOGFMT(AbilityLog, Error, "{0}() The character wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
		EndTask();
		return;
	}
	
	if (bDebugTask)
	{
		UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Activated TargetOverlap task during {3}, LocallyControlled: {4}, PredictingClient: {5}, IsForRemoteClient: {6}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), *GetNameSafe(Ability),
			IsLocallyControlled() ? FString("true") : FString("False"),
			IsPredictingClient() ? FString("true") : FString("False"),
			IsForRemoteClient() ? FString("true") : FString("False")
		);
	}

	// Retrieve the armament hitboxes
	TMap<AArmament*, TArray<UPrimitiveComponent*>> ArmamentHitboxes;
	for (AArmament* Armament : Armaments)
	{
		if (!Armament) continue;

		TArray<UPrimitiveComponent*> Hitboxes = Armament->GetArmamentHitboxes();
		if (Hitboxes.IsEmpty()) continue;

		ArmamentHitboxes.Add(Armament, Hitboxes);
	}

	if (ArmamentHitboxes.IsEmpty())
	{
		UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} none of the armaments had any hitboxes! {3}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), *GetNameSafe(Ability));
		EndTask();
		return;
	}
	
	// Have the client's attack be the target data and what's determined on the client and the server
	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	if (!IsLocallyControlled())
	{
		// Get the ability spec and the prediction key to replicate the target data
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		
		// Bind to the target data delegate so the client information is safely replicated to the server.
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).
		AddUObject(this, &UAbilityTask_TargetOverlap::OnTargetDataReplicatedCallback);
		
		// If this delegate didn't get the information right away send that information to the server
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate) SetWaitingOnRemotePlayerData();
	}
	
	// TODO: Client side prediction
	for (auto &[Armament, Hitboxes] : ArmamentHitboxes)
	{
		for (UPrimitiveComponent* OverlapComponent : Hitboxes)
		{
			if (!OverlapComponent) continue;
		
			OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &UAbilityTask_TargetOverlap::OnTraceOverlap);
		}
	}
}


void UAbilityTask_TargetOverlap::OnTraceOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* Character = Ability->GetAvatarActorFromActorInfo();
 	if (Character == OtherActor) return;

	ACharacterBase* Target = Cast<ACharacterBase>(OtherActor);
	if (!Target) return;


	AEnemy* AITarget = Cast<AEnemy>(OtherActor);
	UAbilitySystem* TargetAsc = Target->GetAbilitySystem<UAbilitySystem>();
	if (!TargetAsc && !AITarget)
	{
		UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Attacked a character with an invalid ability system component! Target: {3}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), *GetNameSafe(Target)
		);
		return;
	}
	
	if (IsPredictingClient() || IsLocallyControlled())
	{
		// This might need to always activate so every attack has their data mapped to an individual place in the AbilityTargetDataMap
		if (!GetActivationPredictionKey().IsValidForMorePrediction())
		{
			FScopedPredictionWindow(Ability->GetAbilitySystemComponentFromActorInfo(), true);
		}

		if (bDebugTask)
		{
			UE_LOGFMT(AbilityLog, Warning, "{0}: {1}'s armament {2} overlapped with {3}! {4} {5}()",
				*UEnum::GetValueAsString(Character->GetLocalRole()),
				*GetNameSafe(Character),
				*GetNameSafe(OverlappedComponent),
				*GetNameSafe(OtherActor),
				*GetName(),
				FString(__FUNCTION__)
			);
		}


		if (ShouldBroadcastAbilityTaskDelegates())
		{
			for (AArmament* Armament : Armaments)
			{
				// Search for the armament we attacked with using it's hitboxes
				if (Armament && Armament->GetArmamentHitboxes().Contains(OverlappedComponent))
				{
					// TODO: Find out how to send valid information across the server
					FGameplayAbilityTargetDataHandle TargetData = FGameplayAbilityTargetDataHandle();
					// FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit(); // We can just use the weapon location on the server for proper hit reaction
					FGameplayAbilityTargetData_ActorArray* Data = new FGameplayAbilityTargetData_ActorArray();

					FGameplayAbilityTargetingLocationInfo LocationInfo;
					LocationInfo.LiteralTransform.SetLocation(SweepResult.ImpactPoint);
					LocationInfo.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
					LocationInfo.SourceAbility = Ability;
					LocationInfo.SourceActor = Armament;
					Data->SourceLocation = LocationInfo;
					
					TArray<TWeakObjectPtr<AActor>> TargetInformation;
					TargetInformation.Add(Armament); // Add the armament that we attacked with
					TargetInformation.Add(OtherActor); // Add the target character
					Data->SetActors(TargetInformation);
					TargetData.Add(Data);
					
					// Send the replicated data to the server
					if (AbilitySystemComponent.Get())
					{
						AbilitySystemComponent->ServerSetReplicatedTargetData(
							GetAbilitySpecHandle(),
							GetActivationPredictionKey(),
							TargetData,
							FGameplayTag(),
							AbilitySystemComponent->ScopedPredictionKey
						);
					}

					if (OnValidOverlap.IsBound())
					{
						if (bDebugTask) UE_LOGFMT(AbilityLog, Log, "{0} sending attack information from weapon {1}", *GetNameSafe(Character), *GetNameSafe(Armament));
						OnValidOverlap.Broadcast(TargetData, TargetAsc);
					}
					else if (bDebugTask)
					{
						UE_LOGFMT(AbilityLog, Log, "{0}'s {1} traced a target without any listeners. Target {2}", *GetNameSafe(Character), *GetNameSafe(Armament), *GetNameSafe(OtherActor));
					}
				}
			}
		}
		else if (bDebugTask) UE_LOGFMT(AbilityLog, Warning, "{0} did not broadcast overlap event!: {1} {2}()", *GetNameSafe(Character), *GetName(), *FString(__FUNCTION__));
	}
}


void UAbilityTask_TargetOverlap::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	ACharacterBase* Character = AbilitySystemComponent.Get() ? Cast<ACharacterBase>(AbilitySystemComponent->GetAvatarActor()) : nullptr;
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{1}() {2}'s Replicated weapon overlap target data was sent when the character information wasn't valid!",
			*FString(__FUNCTION__), *GetNameSafe(AbilitySystemComponent.Get()));
		return;
	}

	// Retrieve the target information
	TArray<TWeakObjectPtr<AActor>> TargetInformation;
	for (TSharedPtr<FGameplayAbilityTargetData> Data : DataHandle.Data)
	{
		if (!Data.IsValid()) continue;
		TargetInformation = Data->GetActors();
	}

	if (TargetInformation.IsEmpty())
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s Replicated weapon overlap target data was sent without any information!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		return;
	}
	
	// Retrieve the character and the weapon
	AArmament* OverlappedWeapon = nullptr;
	ACharacterBase* TargetCharacter = nullptr;
	for (TWeakObjectPtr<AActor> Actor : TargetInformation)
	{
		if (!Actor.Get()) continue;

		// Check if this is the target we attacked
		if (Cast<ACharacterBase>(Actor.Get())) TargetCharacter = Cast<ACharacterBase>(Actor.Get());

		// Check if this is the weapon we attacked with
		if (Cast<AArmament>(Actor.Get())) OverlappedWeapon = Cast<AArmament>(Actor.Get());
	}

	if (!OverlappedWeapon || !TargetCharacter)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s Replicated weapon overlap target data is missing information! Weapon: {3}, Target: {4}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), *GetNameSafe(OverlappedWeapon), *GetNameSafe(TargetCharacter));
	}

	UAbilitySystem* TargetAsc = TargetCharacter->GetAbilitySystem<UAbilitySystem>();
	if (!TargetAsc)
	{
		// AI character replicated information on clients (shouldn't happen)
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s weapon overlap attacked a character without an ability system!",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		// return;
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
		OnValidOverlap.Broadcast(DataHandle, TargetAsc);
	}
}


void UAbilityTask_TargetOverlap::OnDestroy(bool AbilityEnded)
{
	for (AArmament* Armament : Armaments)
	{
		if (!Armament) continue;

		TArray<UPrimitiveComponent*> Hitboxes = Armament->GetArmamentHitboxes();
		for (UPrimitiveComponent* OverlapComponent : Hitboxes)
		{
			if (!OverlapComponent) continue;
			
			OverlapComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UAbilityTask_TargetOverlap::OnTraceOverlap);
		}
	}
	
	Super::OnDestroy(AbilityEnded);
}
