// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TargetMovementData.h"

#include "Sandbox/Asc/AbilitySystem.h"
#include "Logging/StructuredLog.h"


UAbilityTask_TargetMovementData* UAbilityTask_TargetMovementData::CreateMovementDataTask(UGameplayAbility* OwningAbility, bool bDebug)
{
	UAbilityTask_TargetMovementData* Task = NewAbilityTask<UAbilityTask_TargetMovementData>(OwningAbility);
	Task->bDebugTask = bDebug;
	return Task;
}


void UAbilityTask_TargetMovementData::Activate()
{
	Super::Activate();

	// One interesting thing that the ability system component does for handling client side prediction is create a way to reference sending information from client to server using ability tasks and a target data object
	// The way you handle it is by sending the information on the client while listening for it on the server when an ability has been activated and prediction is valid, here's how you should go about it

	const AActor* Character = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Character || !ASC)
	{
		if (!Character) UE_LOGFMT(AbilityLog, Error, "{0}() The character wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
		if (!ASC) UE_LOGFMT(AbilityLog, Error, "{0}() The ability system wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetName());
		EndTask();
		return;
	}
	
	if (bDebugTask)
	{
		UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Activated TargetMovementData task during {3}, LocallyControlled: {4}, PredictingClient: {5}, IsForRemoteClient: {6}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), *GetNameSafe(Ability),
			IsLocallyControlled() ? FString("true") : FString("False"),
			IsPredictingClient() ? FString("true") : FString("False"),
			IsForRemoteClient() ? FString("true") : FString("False")
		);
	}
	
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		if (bDebugTask) UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Sending movement data to server", *UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Ability));
		SendMovementData();
	}
	else
	{
		if (bDebugTask) UE_LOGFMT(LogTemp, Log, "{0}::{1}() {2} binding to the target data delegate to retrieve movement data from the client", *UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetName());
		
		// Get the ability spec and the prediction key to replicate the target data
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		// Bind to the target data delegate so the client information is safely replicated to the server.
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_TargetMovementData::OnTargetDataReplicatedCallback);

		// If this delegate didn't get the information right away send that information to the server
		const bool bCalledDelegate = ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate) SetWaitingOnRemotePlayerData();
	}
}


void UAbilityTask_TargetMovementData::SendMovementData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), true);
	AActor* Character = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	USkeletalMeshComponent* Mesh = Ability->GetCurrentActorInfo()->SkeletalMeshComponent.Get();

	// Location information
	FVector2D InputVector; // PlayerCharacter->GetMovementInput();
	FTransform TargetTransform = FTransform();
	TargetTransform.SetLocation(FVector(InputVector.X, InputVector.Y, 0.f));

	// Setup the target data
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_LocationInfo* Data = new FGameplayAbilityTargetData_LocationInfo();
	FGameplayAbilityTargetingLocationInfo SourceLocation = FGameplayAbilityTargetingLocationInfo();
	FGameplayAbilityTargetingLocationInfo TargetLocation = SourceLocation;

	// In this case, the source location is the player's world location, and the target location is a vector of it's movement values
	// SourceLocation.LiteralTransform = Character->GetTransform();
	// SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	// SourceLocation.SourceAbility = Ability;
	// SourceLocation.SourceActor = Character;
	// SourceLocation.SourceComponent = Mesh;
	
	TargetLocation.LiteralTransform = TargetTransform; // This is the only information that needs to be sent across the server
	TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	TargetLocation.SourceAbility = Ability;
	TargetLocation.SourceActor = Character;
	TargetLocation.SourceComponent = Mesh;

	Data->SourceLocation = SourceLocation;
	Data->TargetLocation = TargetLocation;
	DataHandle.Add(Data);
	
	// Send the replicated data to the server
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		InputData.Broadcast(DataHandle);
	}
}


void UAbilityTask_TargetMovementData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	if (bDebugTask)
	{
		UE_LOGFMT(LogTemp, Warning, "{0}::{1}() Ability: {2}, {3} OnTargetDataReplicatedCallback delegate",
			*UEnum::GetValueAsString(GetAvatarActor()->GetLocalRole()),
			*FString(__FUNCTION__),
			*GetNameSafe(GetAvatarActor()),
			*GetNameSafe(Ability)
		);
	}
	
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		InputData.Broadcast(DataHandle);
		EndTask();
	}
}
