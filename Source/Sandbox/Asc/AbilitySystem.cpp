// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem.h"

#include "EnhancedInputComponent.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(AbilityLog);


void UAbilitySystem::BeginPlay()
{
	Super::BeginPlay();
	
	// AbilityActivatedCallbacks.AddUObject(this, &UGSCAbilitySystemComponent::OnAbilityActivatedCallback);
	// AbilityFailedCallbacks.AddUObject(this, &UGSCAbilitySystemComponent::OnAbilityFailedCallback);
	// AbilityEndedCallbacks.AddUObject(this, &UGSCAbilitySystemComponent::OnAbilityEndedCallback);
	
	// Grant startup effects on begin play instead of from within InitAbilityActorInfo to avoid
	// "ticking" periodic effects when BP is first opened
	// GrantStartupEffects();
}


void UAbilitySystem::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	// Only run if we're any type of game world (including PIE)
	// This prevents unwanted initialization of Ability System during Editor initialization times (ActorPreview, Thumbnail, etc.)
	if (GetWorld() && !GetWorld()->IsGameWorld())
	{
		return;
	}

	// Add the anim instance and initialize the ability system to retrieve information
	if (AbilityActorInfo && InOwnerActor)
	{
		if (AbilityActorInfo->AnimInstance == nullptr)
		{
			AbilityActorInfo->AnimInstance = AbilityActorInfo->GetAnimInstance();
		}

		if (UGameInstance* GameInstance = InOwnerActor->GetGameInstance())
		{
			// Sign up for possess / un possess events so that we can update the cached AbilityActorInfo accordingly
			if (!GameInstance->GetOnPawnControllerChanged().Contains(this, TEXT("OnPawnControllerChanged")))
			{
				GameInstance->GetOnPawnControllerChanged().AddDynamic(this, &UAbilitySystem::OnPawnControllerChanged);
			}
		}

		// UAnimInstance* AnimInstance = AbilityActorInfo->GetAnimInstance();
		// if (IGSCNativeAnimInstanceInterface* AnimInstanceInterface = Cast<IGSCNativeAnimInstanceInterface>(AnimInstance))
		// {
		// 	GSC_WLOG(Verbose, TEXT("Initialize `%s` AnimInstance with Ability System"), *GetNameSafe(AnimInstance))
		// 	AnimInstanceInterface->InitializeWithAbilitySystem(this);
		// }
	}

	// Add attributes and abilities
	
	
	// Broadcast to Blueprint InitAbilityActorInfo was called, this will happen multiple times for both client / server
	OnInitAbilityActorInfo.Broadcast();
}


#pragma region Constructors
void UAbilitySystem::BeginDestroy()
{
	// Reset ...

	// Clear any delegate handled bound previously for this component
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor.IsValid())
	{
		if (UGameInstance* GameInstance = AbilityActorInfo->OwnerActor->GetGameInstance())
		{
			GameInstance->GetOnPawnControllerChanged().RemoveAll(this);
		}
	}

	OnGiveAbilityDelegate.RemoveAll(this);

	/*
	// Remove any added attributes
	for (UAttributeSet* AttribSetInstance : AddedAttributes)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		RemoveSpawnedAttribute(AttribSetInstance);
#else
		GetSpawnedAttributes_Mutable().Remove(AttribSetInstance);
#endif
	}

	// Clear up abilities / bindings
	UGSCAbilityInputBindingComponent* InputComponent = AbilityActorInfo && AbilityActorInfo->AvatarActor.IsValid() ? AbilityActorInfo->AvatarActor->FindComponentByClass<UGSCAbilityInputBindingComponent>() : nullptr;

	for (const FGSCMappedAbility& DefaultAbilityHandle : AddedAbilityHandles)
	{
		if (InputComponent)
		{
			InputComponent->ClearInputBinding(DefaultAbilityHandle.Handle);
		}

		// Only Clear abilities on authority
		if (IsOwnerActorAuthoritative())
		{
			SetRemoveAbilityOnEnd(DefaultAbilityHandle.Handle);
		}
	}
	*/
	

	// Clear up any bound delegates in Core Component that were registered from InitAbilityActorInfo
	ShutdownAbilitySystemDelegates();

	// AddedAbilityHandles.Reset();
	// AddedAttributes.Reset();
	// AddedEffects.Reset();
	// AddedAbilitySets.Reset();

	Super::BeginDestroy();
}


void UAbilitySystem::RegisterAbilitySystemDelegates()
{
	// GSC_WLOG(Verbose, TEXT("Registering delegates for ASC: %s"), *GetNameSafe(ASC))
	
	// Make sure to shut down delegates previously registered, if RegisterAbilitySystemDelegates is called more than once (likely from AbilityActorInfo)
	// ShutdownAbilitySystemDelegates(ASC);
	
	/*
	TArray<FGameplayAttribute> Attributes;
	GetAllAttributes(Attributes);
	for (FGameplayAttribute Attribute : Attributes)
	{
		if (Attribute == UGSCAttributeSet::GetDamageAttribute() || Attribute == UGSCAttributeSet::GetStaminaDamageAttribute())
		{
			GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAbilitySystem::OnDamageAttributeChanged);
		}
		else
		{
			GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAbilitySystem::OnAttributeChanged);
		}
	}
	*/

	// OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UAbilitySystem::OnActiveGameplayEffectAdded); // Handle GameplayEffects added / remove
	// OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UAbilitySystem::OnAnyGameplayEffectRemoved);
	// RegisterGenericGameplayTagEvent().AddUObject(this, &UAbilitySystem::OnAnyGameplayTagChanged); // Handle generic GameplayTags added / removed
	// AbilityCommittedCallbacks.AddUObject(this, &UAbilitySystem::OnAbilityCommitted); // Handle Ability Commit events
}

void UAbilitySystem::ShutdownAbilitySystemDelegates()
{
	// GSC_LOG(Log, TEXT("UAbilitySystem::ShutdownAbilitySystemDelegates for ASC: %s"), ASC ? *ASC->GetName() : TEXT("NONE"))

	/*
	TArray<FGameplayAttribute> Attributes;
	ASC->GetAllAttributes(Attributes);
	for (const FGameplayAttribute& Attribute : Attributes)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
	}
	*/

	// OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	// OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	// RegisterGenericGameplayTagEvent().RemoveAll(this);
	// AbilityCommittedCallbacks.RemoveAll(this);

	/*
	for (const FActiveGameplayEffectHandle GameplayEffectAddedHandle : GameplayEffectAddedHandles)
	{
		if (!GameplayEffectAddedHandle.IsValid()) continue;
		
		FOnActiveGameplayEffectStackChange* EffectStackChangeDelegate = OnGameplayEffectStackChangeDelegate(GameplayEffectAddedHandle);
		if (EffectStackChangeDelegate) EffectStackChangeDelegate->RemoveAll(this);

		FOnActiveGameplayEffectTimeChange* EffectTimeChangeDelegate = OnGameplayEffectTimeChangeDelegate(GameplayEffectAddedHandle);
		if (EffectTimeChangeDelegate) EffectTimeChangeDelegate->RemoveAll(this);
	}
	*/
}
#pragma endregion 




#pragma region Gameplay Ability Functions
FGameplayAbilitySpecHandle UAbilitySystem::AddAbility(const FGameplayAbilityInfo& NewAbility)
{
	if (!IsOwnerActorAuthoritative())
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Add Ability called on non authoritative actor! {2}",
			*UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwnerActor())
		);
		return FGameplayAbilitySpecHandle();
	}

	const TSubclassOf<UGameplayAbility> AbilityClass = NewAbility.Ability;
	if (!AbilityClass)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Add Ability called with an invalid ability! {2}",
			*UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwnerActor())
		);
		return FGameplayAbilitySpecHandle();
	}
	

	// Handling for if we already have this ability
	for (auto &[CurrentAbility, Map] : AddedAbilityHandles)
	{
		if (AbilityClass != CurrentAbility) continue;
			
		// Add the ability to the ability handle, if we don't have this instance of the ability added
		if (!Map.GrantedAbilities.Contains(NewAbility.Id))
		{
			Map.GrantedAbilities.Add(NewAbility.Id, NewAbility);
		}
		else if (Map.Ability.Level < NewAbility.Level)
		{
			// Update the ability level
			RemoveGameplayAbility(Map.AbilityHandle);
			Map.Ability = FGameplayAbilitySpec();
			Map.AbilityHandle = FGameplayAbilitySpecHandle();
			Map.GrantedAbilities.Remove(Map.CurrentlyActivatedAbility);
			Map.CurrentlyActivatedAbility = FGuid();
		}

		// Add the ability if there isn't a current instance of the ability already
		if (!Map.Ability.Ability)
		{
			// Create the new ability specification
			Map.Ability = BuildAbilitySpecFromClass(AbilityClass, NewAbility.Level, static_cast<int32>(NewAbility.InputId));

			// Add the ability to the character, and update the handle
			Map.AbilityHandle = GiveAbility(Map.Ability);
			Map.CurrentlyActivatedAbility = NewAbility.Id;
			Map.GrantedAbilities.Add(NewAbility.Id, NewAbility);
		}

		// Map.AbilityHandle.IsValid()
		return Map.AbilityHandle;
	}
	
	// If we haven't added this ability yet
	if (!AddedAbilityHandles.Contains(AbilityClass))
	{
		// Build another ability handle and add the ability
		F_MultiAbilityHandle NewAbilityHandle;

		// Create the new ability specification
		NewAbilityHandle.Ability = BuildAbilitySpecFromClass(AbilityClass, NewAbility.Level, static_cast<int32>(NewAbility.InputId));
		
		// Add the ability to the character, and update the handle
		NewAbilityHandle.AbilityHandle = GiveAbility(NewAbilityHandle.Ability);
		NewAbilityHandle.CurrentlyActivatedAbility = NewAbility.Id;
		NewAbilityHandle.GrantedAbilities.Add(NewAbility.Id, NewAbility);

		// Only add the ability if it was successfully applied
		if (NewAbilityHandle.AbilityHandle.IsValid()) AddedAbilityHandles.Add(AbilityClass, NewAbilityHandle);
		return NewAbilityHandle.AbilityHandle;
	}

	UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Something happened while adding an ability! {2}",
		*UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwnerActor()));
	return FGameplayAbilitySpecHandle();
}


TArray<FGameplayAbilitySpecHandle> UAbilitySystem::AddAbilities(TArray<FGameplayAbilityInfo> NewAbilities)
{
	if (!IsOwnerActorAuthoritative())
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Adding Abilities on non authoritative actor is not allowed! {2}",
			*UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwnerActor()));
		return {};
	}
	
	TArray<FGameplayAbilitySpecHandle> SpecHandles;
	for (const FGameplayAbilityInfo& AbilityMapping : NewAbilities)
	{
		FGameplayAbilitySpecHandle SpecHandle = AddAbility(AbilityMapping);
		if (SpecHandle.IsValid())
		{
			SpecHandles.Add(SpecHandle);
		}
	}
	
	return SpecHandles;
}


FGameplayAbilitySpec UAbilitySystem::GetAbilitySpec(TSubclassOf<UGameplayAbility> GameplayAbility)
{
	for (auto &[Id, Map] : AddedAbilityHandles)
	{
		if (GameplayAbility == Map.Ability.Ability.GetClass())
		{
			return Map.Ability;
		}
	}

	return FGameplayAbilitySpec();
}


void UAbilitySystem::RemoveGameplayAbility(const FGameplayAbilitySpecHandle& Handle)
{
	if (!IsOwnerActorAuthoritative())
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Removing Abilities on non authoritative actor is not allowed! {2}",
			*UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwnerActor()));
		return;
	}

	// Remove the current instance, and add the next valid current instance
	bool bRemovedAbility = false; // If we don't find the ability within our own handles, just remove it later
	for (auto &[CurrentAbility, Map] : AddedAbilityHandles)
	{
		if (Map.AbilityHandle != Handle) continue;

		// Remove the current instance of the ability
		ClearAbility(Handle);
		Map.GrantedAbilities.Remove(Map.CurrentlyActivatedAbility);
		Map.CurrentlyActivatedAbility = FGuid();
		Map.Ability = FGameplayAbilitySpec();
		Map.AbilityHandle = FGameplayAbilitySpecHandle();
		bRemovedAbility = true;

		// Find the next instance of this ability to use
		FGameplayAbilityInfo AbilityInstance;
		for (auto &[Id, AbilityInfo] : Map.GrantedAbilities)
		{
			if (!AbilityInfo.Ability) continue;
			
			// Just use the current ability if we don't have a valid one yet
			if (!AbilityInstance.Ability) AbilityInstance = AbilityInfo;
			else if (AbilityInfo.Level > AbilityInstance.Level) AbilityInstance = AbilityInfo;
		}

		// Add the new instance of the ability
		if (AbilityInstance.Ability)
		{
			AddAbility(AbilityInstance);
		}
	}

	if (!bRemovedAbility)
	{
		ClearAbility(Handle);
	}
}


void UAbilitySystem::RemoveGameplayAbilities(const TArray<FGameplayAbilitySpecHandle> Handles)
{
	if (!IsOwnerActorAuthoritative())
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Removing Abilities on non authoritative actor is not allowed! {2}",
			*UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwnerActor()));
		return;
	}
	
	for (const FGameplayAbilitySpecHandle& Handle : Handles)
	{
		RemoveGameplayAbility(Handle);
	}
}


FActiveGameplayEffectHandle UAbilitySystem::AddGameplayEffect(const FGameplayEffectInfo& EffectMapping)
{
	const TSubclassOf<UGameplayEffect> GameplayEffect = EffectMapping.Effect;
	if (!GameplayEffect)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Tried to add a gameplay effect without a valid class reference!",
			*UEnum::GetValueAsString(GetAvatarActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetAvatarActor()));
		return FActiveGameplayEffectHandle();
	}
	
	// Create the effect context and handle, and add the additional information
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	const FGameplayEffectSpecHandle GameplayEffectSpec = MakeOutgoingSpec(GameplayEffect, EffectMapping.Level, EffectContext);
	EffectContext.AddSourceObject(GetAvatarActor());
	
	// If it's a valid effect, add it to the character
	FActiveGameplayEffectHandle EffectHandle;
	if (GameplayEffectSpec.IsValid())
	{
		EffectHandle = ApplyGameplayEffectSpecToTarget(*GameplayEffectSpec.Data.Get(), this);
		AddedEffects.Add(EffectHandle);
		return EffectHandle;
	}
	
	return EffectHandle;
}


TArray<FActiveGameplayEffectHandle> UAbilitySystem::AddGameplayEffects(const TArray<FGameplayEffectInfo> EffectMappings)
{
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
	for (const FGameplayEffectInfo& EffectMapping : EffectMappings)
	{
		FActiveGameplayEffectHandle EffectHandle = AddGameplayEffect(EffectMapping);
		if (EffectHandle.IsValid())
		{
			GameplayEffectHandles.Add(EffectHandle);
		}
	}

	return GameplayEffectHandles;
}


bool UAbilitySystem::RemoveGameplayEffect(const FActiveGameplayEffectHandle& Handle, int32 StacksToRemove)
{
	// Remove the gameplay effect, and the handles we have stored
	const bool bSuccessfullyRemoved = RemoveActiveGameplayEffect(Handle, StacksToRemove);
	if (bSuccessfullyRemoved)
	{
		int32 AbilityIndex = -1;
		for (int32 Index = 0; Index < AddedEffects.Num(); Index++)
		{
			const FActiveGameplayEffectHandle& Ability = AddedEffects[Index];
			if (Ability == Handle)
			{
				AbilityIndex = Index;
				break;
			}
		}
	
		AddedEffects.RemoveAt(AbilityIndex);
	}
	
	return bSuccessfullyRemoved;
}


bool UAbilitySystem::TryActivateAbilityByTag(const FGameplayTag Tag, bool bAllowRemoteActivation)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	bool bSuccessfullyActivated = false;
	// AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityEquipTags);
	// GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityEquipTags, AbilitiesToActivate); // I had trouble getting this to work so I created another function that searches for a single ability and an exact tag 

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!Spec.Ability) continue;
			
		FGameplayTagContainer AbilitySpecTags = Spec.Ability->AbilityTags;
		if (Spec.Ability && AbilitySpecTags.HasTagExact(Tag))
		{
			if (Spec.Ability->DoesAbilitySatisfyTagRequirements(*this))
			{
				bSuccessfullyActivated |= TryActivateAbility(Spec.Handle, bAllowRemoteActivation);

				if (bSuccessfullyActivated)
				{
					return true;
				}
			}
		}
	}

	return bSuccessfullyActivated;
}

#pragma endregion 



#pragma region Ability Input Logic
void UAbilitySystem::BindAbilityActivationToEnhancedInput(UEnhancedInputComponent* InputComponent, const TArray<FInputActionAbilityMap>& AbilityInputActions)
{
	// Enhanced inputs is the norm, and it's more interactive and customizable than the old version
	if (!InputComponent)
	{
		// UE_LOGFMT(AbilityLog, Error, "{0}::{1}() The input component was invalid while trying to add input bindings to {2}!",
		// 	*UEnum::GetValueAsString(GetAvatarActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetAvatarActor()));
		// TODO: Add a check for whether the input component was invalid during both SetupPlayerInput() and OnInitAbilityActorInfo()
		// Right now on the server only SetupPlayerInput() works, and on client OnInitAbilityActorInfo() 
		return;
	}

	// Remove the old bindings, if there's already binds
	for (const int32 RegisteredHandle : RegisteredAbilityInputHandles)
	{
		InputComponent->ClearActionBindings();
		InputComponent->RemoveActionBinding(RegisteredHandle);
	}
	RegisteredAbilityInputHandles.Empty();

	// Add the ability input actions
	UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Adding Ability Inputs", *UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
	for (FInputActionAbilityMap InputAction : AbilityInputActions)
	{
		for (int32 i = 0; i < InputAction.KeyEvents.Num(); i++)
		{
			// Pressed Events
			if (ETriggerEvent::Started == InputAction.KeyEvents[i])
			{
				int32 InputActionHandle = InputComponent->BindAction(
					InputAction.InputAction,
					InputAction.KeyEvents[i],
					this,
					&UAbilitySystem::AbilityLocalInputPressed,
					static_cast<int32>(InputAction.InputId)
				).GetHandle();
				RegisteredAbilityInputHandles.Add(InputActionHandle);
				
				UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} adding ability input bind {3} on press events. Input action: {4}", *UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__),
					*GetNameSafe(GetOwner()), *UEnum::GetValueAsString(InputAction.InputId), *InputAction.InputAction.GetName());
			}

			// Released Events
			if (ETriggerEvent::Completed == InputAction.KeyEvents[i])
			{
				int32 InputActionHandle = InputComponent->BindAction(
					InputAction.InputAction,
					InputAction.KeyEvents[i],
					this,
					&UAbilitySystem::AbilityLocalInputReleased,
					static_cast<int32>(InputAction.InputId)
				).GetHandle();
				RegisteredAbilityInputHandles.Add(InputActionHandle);
				
				UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} adding ability input bind {3} on released events. Input action: {4}", *UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__),
					*GetNameSafe(GetOwner()), *UEnum::GetValueAsString(InputAction.InputId), *InputAction.InputAction.GetName());
			}

			// Add Other Events
		}
	}

}

void UAbilitySystem::BindAbilityActivationToInputComponent(UInputComponent* InputComponent, FGameplayAbilityInputBinds BindInfo)
{
	// Add the ability input bindings
	// FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(FName("/Script/GameplayAbilityUtilities"), FName("EInputAbilities"));
	// FGameplayAbilityInputBinds AbilityInputBinds = FGameplayAbilityInputBinds(
	// 	*UEnum::GetValueAsString(EInputAbilities::Confirm),
	// 	*UEnum::GetValueAsString(EInputAbilities::Cancel),
	// 	AbilityEnumAssetPath,
	// 	static_cast<int32>(EInputAbilities::Confirm),
	// 	static_cast<int32>(EInputAbilities::Cancel)
	// );
	
	if (!InputComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() The input component was invalid while trying to add input bindings to {2}!",
			*UEnum::GetValueAsString(GetAvatarActor()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetAvatarActor()));
		return;
	}
	
	// Remove the old input bindings, if there's already binds
	InputComponent->RemoveActionBinding(FName(*BindInfo.ConfirmTargetCommand), IE_Pressed);
	InputComponent->RemoveActionBinding(FName(*BindInfo.CancelTargetCommand), IE_Pressed);
	LocalConfirmInput_Old.ActionDelegate.Unbind();
	LocalCancelInput_Old.ActionDelegate.Unbind();
	for (FInputActionBindingAbilityMap AbilityInputAction : AbilityInputActions_Old)
	{
		InputComponent->RemoveActionBinding(AbilityInputAction.InputAction, AbilityInputAction.InputId);
		AbilityInputAction.InputAction.ActionDelegate.Unbind();
	}
	AbilityInputActions_Old.Empty();

	
	// Retrieve the input action names and indexes from the enum binds, and use this as reference for mapping abilities to input invocations
	UEnum* EnumBinds = BindInfo.GetBindEnum();
	SetBlockAbilityBindingsArray(BindInfo);
	
	for(int32 idx=0; idx < EnumBinds->NumEnums(); ++idx)
	{
		const FString AbilityInputActionName = EnumBinds->GetNameStringByIndex(idx);
		
		// Pressed event
		{
			FInputActionBinding AB(FName(*AbilityInputActionName), IE_Pressed);
			AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::AbilityLocalInputPressed, idx);
			InputComponent->AddActionBinding(AB);
			AbilityInputActions_Old.Add(FInputActionBindingAbilityMap(InputComponent->AddActionBinding(AB), idx));
		}

		// Released event
		{
			FInputActionBinding AB(FName(*AbilityInputActionName), IE_Released);
			AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::AbilityLocalInputReleased, idx);
			InputComponent->AddActionBinding(AB);
			AbilityInputActions_Old.Add(FInputActionBindingAbilityMap(InputComponent->AddActionBinding(AB), idx));
		}
	}

	// Bind Confirm/Cancel. Note: these have to come last!
	if (BindInfo.ConfirmTargetCommand.IsEmpty() == false)
	{
		LocalConfirmInput_Old = FInputActionBinding(FName(*BindInfo.ConfirmTargetCommand), IE_Pressed);
		LocalConfirmInput_Old.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::LocalInputConfirm);
		InputComponent->AddActionBinding(LocalConfirmInput_Old);
	}
	
	if (BindInfo.CancelTargetCommand.IsEmpty() == false)
	{
		LocalCancelInput_Old = FInputActionBinding(FName(*BindInfo.CancelTargetCommand), IE_Pressed);
		LocalCancelInput_Old.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::LocalInputCancel);
		InputComponent->AddActionBinding(LocalCancelInput_Old);
	}

	if (BindInfo.CancelTargetInputID >= 0)
	{
		GenericCancelInputID = BindInfo.CancelTargetInputID;
	}
	if (BindInfo.ConfirmTargetInputID >= 0)
	{
		GenericConfirmInputID = BindInfo.ConfirmTargetInputID;
	}
}


void UAbilitySystem::AbilityLocalInputPressed(int32 InputID)
{
	// Consume the input if this InputID is overloaded with GenericConfirm/Cancel and the GenericConfim/Cancel callback is bound
	if (IsGenericConfirmInputBound(InputID))
	{
		LocalInputConfirm();
		return;
	}

	if (IsGenericCancelInputBound(InputID))
	{
		LocalInputCancel();
		return;
	}

	// ---------------------------------------------------------

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			// Custom ability behavior
			
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				if (Spec.IsActive())
				{
					if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
					{
						ServerSetInputPressed(Spec.Handle);
					}

					AbilitySpecInputPressed(Spec);

					// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());					
				}
				else
				{
					// Ability is not active, so try to activate it
					TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}


void UAbilitySystem::AbilityLocalInputReleased(int32 InputID)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			Spec.InputPressed = false;
			if (Spec.Ability && Spec.IsActive())
			{
				if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
				{
					ServerSetInputReleased(Spec.Handle);
				}

				AbilitySpecInputReleased(Spec);
				
				// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}
#pragma endregion 



#pragma region Gameplay Effects
void UAbilitySystem::OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectAdded.Broadcast(AssetTags, GrantedTags, ActiveHandle);

	if (FOnActiveGameplayEffectStackChange* Delegate = OnGameplayEffectStackChangeDelegate(ActiveHandle))
	{
		Delegate->AddUObject(this, &UAbilitySystem::OnActiveGameplayEffectStackChanged);
	}

	if (FOnActiveGameplayEffectTimeChange* Delegate = OnGameplayEffectTimeChangeDelegate(ActiveHandle))
	{
		Delegate->AddUObject(this, &UAbilitySystem::OnActiveGameplayEffectTimeChanged);
	}

	// Store active handles to clear out bound delegates when shutting down listeners
	GameplayEffectAddedHandles.AddUnique(ActiveHandle);
}


void UAbilitySystem::OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount)
{
	const FActiveGameplayEffect* GameplayEffect = GetActiveGameplayEffect(ActiveHandle);
	if (!GameplayEffect)
	{
		return;
	}

	FGameplayTagContainer AssetTags;
	GameplayEffect->Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	GameplayEffect->Spec.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectStackChange.Broadcast(AssetTags, GrantedTags, ActiveHandle, NewStackCount, PreviousStackCount);
}


void UAbilitySystem::OnActiveGameplayEffectTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration)
{
	const FActiveGameplayEffect* GameplayEffect = GetActiveGameplayEffect(ActiveHandle);
	if (!GameplayEffect)
	{
		return;
	}

	FGameplayTagContainer AssetTags;
	GameplayEffect->Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	GameplayEffect->Spec.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectTimeChange.Broadcast(AssetTags, GrantedTags, ActiveHandle, NewStartTime, NewDuration);
}


void UAbilitySystem::OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& EffectRemoved)
{
	FGameplayTagContainer AssetTags;
	EffectRemoved.Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	EffectRemoved.Spec.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectStackChange.Broadcast(AssetTags, GrantedTags, EffectRemoved.Handle, 0, 1);
	OnGameplayEffectRemoved.Broadcast(AssetTags, GrantedTags, EffectRemoved.Handle);
}
#pragma endregion




#pragma region Utility
void UAbilitySystem::K2_AddLooseGameplayTag(FGameplayTag Tag)
{
	AddLooseGameplayTag(Tag);
}


void UAbilitySystem::K2_AddReplicatedLooseGameplayTag(FGameplayTag Tag)
{
	AddReplicatedLooseGameplayTag(Tag);
}


void UAbilitySystem::K2_RemoveLooseGameplayTag(FGameplayTag Tag)
{
	RemoveLooseGameplayTag(Tag);
}


void UAbilitySystem::K2_RemoveReplicatedLooseGameplayTag(FGameplayTag Tag)
{
	RemoveReplicatedLooseGameplayTag(Tag);
}


void UAbilitySystem::OnPawnControllerChanged(APawn* Pawn, AController* NewController)
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor == Pawn && AbilityActorInfo->PlayerController != NewController)
	{
		if (!NewController)
		{
			// NewController null, prevent refresh actor info. Needed to ensure TargetActor EndPlay properly unbind from GenericLocalConfirmCallbacks/GenericLocalCancelCallbacks
			// and avoid an ensure error if ActorInfo PlayerController is invalid
			return;
		}

		AbilityActorInfo->InitFromActor(AbilityActorInfo->OwnerActor.Get(), AbilityActorInfo->AvatarActor.Get(), this);
	}
}
#pragma endregion 

