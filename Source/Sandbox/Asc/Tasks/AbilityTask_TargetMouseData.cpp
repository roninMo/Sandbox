// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TargetMouseData.h"

#include "Sandbox/Asc/AbilitySystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

UAbilityTask_TargetMouseData* UAbilityTask_TargetMouseData::CreateTargetMouseData(UGameplayAbility* OwningAbility, bool bDebug)
{
	UAbilityTask_TargetMouseData* Task = NewAbilityTask<UAbilityTask_TargetMouseData>(OwningAbility);
	Task->bDebugTask = bDebug;
	return Task;
}


void UAbilityTask_TargetMouseData::Activate()
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
		UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Activated TargetMouseData task during {3}, LocallyControlled: {4}, PredictingClient: {5}, IsForRemoteClient: {6}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), *GetNameSafe(Ability),
			IsLocallyControlled() ? FString("true") : FString("False"),
			IsPredictingClient() ? FString("true") : FString("False"),
			IsForRemoteClient() ? FString("true") : FString("False")
		);
	}
	
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		if (bDebugTask) UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Sending mouse data to server", *UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Ability));
		SendMouseData();
	}
	else
	{
		if (bDebugTask) UE_LOGFMT(LogTemp, Log, "{0}::{1}() {2} binding to the target data delegate to retrieve mouse data from the client", *UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetName());
		
		// Get the ability spec and the prediction key to replicate the target data
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		// Bind to the target data delegate so the client information is safely replicated to the server.
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_TargetMouseData::OnTargetDataReplicatedCallback);

		// If this delegate didn't get the information right away send that information to the server
		const bool bCalledDelegate = ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate) SetWaitingOnRemotePlayerData();
	}
}


void UAbilityTask_TargetMouseData::SendMouseData()
{
	const AActor* Character = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Character)
	{
		if (!Character) UE_LOGFMT(AbilityLog, Error, "{0}() The character wasn't valid on the ability while retrieving input information! {1}", *FString(__FUNCTION__), *GetNameSafe(Character));
		EndTask();
		return;
	}
	
	// Calculate the aim location
	FVector_NetQuantize10 AimLocation; // Center of the screen deprojected to the world
	FVector_NetQuantize10 AimForwardVector; // The forward vector (where the vector is aiming)
	FVector2D ViewportSize;
	
	if (GEngine && GEngine->GameViewport) GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D CrosshairScreenLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	const bool bSuccessfullyRetrievedCrosshairLocation = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(Character, 0), CrosshairScreenLocation, AimLocation, AimForwardVector
	);
	
	if (!bSuccessfullyRetrievedCrosshairLocation)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}() failed to get the aim location for client {1}!", *FString(__FUNCTION__), *GetNameSafe(Character));
	}
	
	// Make the target data
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = GetAimDirection(AimLocation, AimForwardVector, 10000);
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

FHitResult UAbilityTask_TargetMouseData::GetAimDirection(FVector_NetQuantize10 AimLocation, FVector_NetQuantize10 AimForwardVector, float Length) const
{
	if (!GetWorld()) return FHitResult();
	
	TArray<AActor*> IgnoredActors;
	if (Ability->GetAvatarActorFromActorInfo()) IgnoredActors.AddUnique(Ability->GetAvatarActorFromActorInfo());
	if (Ability->GetOwningActorFromActorInfo()) IgnoredActors.AddUnique(Ability->GetOwningActorFromActorInfo());
	
	FVector StartLocation = AimLocation;
	const FVector_NetQuantize AimDirection = AimLocation + AimForwardVector * Length; // This calculation is an fvector from our crosshair outwards
	FHitResult TraceHitResult;
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLocation,
		AimDirection,
		TraceTypeQuery1,
		false, // complex trace
		IgnoredActors,
		bDebugTask ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		TraceHitResult,
		true, // ignore self
		FColor::Red,
		FColor::Green,
		1
	);

	if (!TraceHitResult.bBlockingHit)
	{
		TraceHitResult.ImpactPoint = AimDirection;
	}
	
	return TraceHitResult;
}


void UAbilityTask_TargetMouseData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
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
