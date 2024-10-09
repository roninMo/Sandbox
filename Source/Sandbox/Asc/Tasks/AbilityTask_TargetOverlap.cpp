// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TargetOverlap.h"

#include "Sandbox/Asc/AbilitySystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Logging/StructuredLog.h"

UAbilityTask_TargetOverlap* UAbilityTask_TargetOverlap::CreateOverlapDataTask(UGameplayAbility* OwningAbility, TArray<UPrimitiveComponent*> CollisionComponents, bool bDebug)
{
	UAbilityTask_TargetOverlap* Task = NewAbilityTask<UAbilityTask_TargetOverlap>(OwningAbility);
	Task->OverlapComponents = CollisionComponents;
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
	if (!Character || !ASC || OverlapComponents.IsEmpty())
	{
		if (!Character) UE_LOGFMT(AbilityLog, Error, "{0}() The character wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
		if (!ASC) UE_LOGFMT(AbilityLog, Error, "{0}() The ability system wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
		if (OverlapComponents.IsEmpty()) UE_LOGFMT(AbilityLog, Error, "{0}() The hitbox wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
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
	for (UPrimitiveComponent* OverlapComponent : OverlapComponents)
	{
		if (!OverlapComponent) continue;
	
		OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &UAbilityTask_TargetOverlap::OnTraceOverlap);
	}
}


void UAbilityTask_TargetOverlap::OnTraceOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const AActor* Character = Ability->GetAvatarActorFromActorInfo();
 	if (Character == OtherActor) return;

	ACharacterBase* Target = Cast<ACharacterBase>(OtherActor);
	if (!Target) return;

	UAbilitySystem* TargetAsc = Target->GetAbilitySystem<UAbilitySystem>();
	if (!TargetAsc)
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
		

		// This is dummy hit result data, just use the armament location
		// TODO: Find out how to send valid information across the server
		FGameplayAbilityTargetDataHandle TargetData = FGameplayAbilityTargetDataHandle();
		FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
		Data->HitResult = SweepResult;

		TArray<TWeakObjectPtr<AActor>> Targets;
		Targets.Add(OtherActor);
		Data->SetActors(Targets);
		TargetData.Add(Data);
		
		// Send the replicated data to the server'
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

		// UE_LOGFMT(AbilityLog, Log, "{0} overlapped with {1}", *GetNameSafe(Character), *GetNameSafe(Target));
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnValidOverlap.Broadcast(TargetData, TargetAsc);
		}
		else if (bDebugTask) UE_LOGFMT(AbilityLog, Warning, "{0} did not broadcast overlap event!: {1} {2}()", *GetNameSafe(Character), *GetName(), *FString(__FUNCTION__));
	}
}


void UAbilityTask_TargetOverlap::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	FHitResult HitResult;
	TArray<TWeakObjectPtr<AActor>> TargetActors;
	for (TSharedPtr<FGameplayAbilityTargetData> Data : DataHandle.Data)
	{
		if (!Data.IsValid()) continue;
		TargetActors = Data->GetActors();
		HitResult = *Data->GetHitResult();
	}

	if (TargetActors.IsEmpty()) return;
	
	AActor* TargetActor = TargetActors[0].Get();
	if (!TargetActor) return;

	UAbilitySystem* TargetAsc = Cast<UAbilitySystem>(AbilitySystemComponent.Get());
	if (!TargetAsc) return;
	
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnValidOverlap.Broadcast(DataHandle, TargetAsc);
	}
}


void UAbilityTask_TargetOverlap::OnDestroy(bool AbilityEnded)
{
	for (UPrimitiveComponent* OverlapComponent : OverlapComponents)
	{
		if (!OverlapComponent) continue;
		
		OverlapComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UAbilityTask_TargetOverlap::OnTraceOverlap);
	}
	
	Super::OnDestroy(AbilityEnded);
}
