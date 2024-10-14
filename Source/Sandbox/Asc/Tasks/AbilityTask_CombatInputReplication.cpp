// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Tasks/AbilityTask_CombatInputReplication.h"

#include "AbilitySystemComponent.h"
#include "Sandbox/Data/Structs/CombatInformation.h"


UAbilityTask_CombatInputReplication::UAbilityTask_CombatInputReplication(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	StartTime = 0.f;
	bTestInitialState = false;
	bDebugTask = false;
}


UAbilityTask_CombatInputReplication* UAbilityTask_CombatInputReplication::CombatInputReplication(UGameplayAbility* OwningAbility, EInputAbilities InputPattern, bool bTestAlreadyPressed, bool bDebug)
{
	UAbilityTask_CombatInputReplication* Task = NewAbilityTask<UAbilityTask_CombatInputReplication>(OwningAbility);
	Task->bTestInitialState = bTestAlreadyPressed;
	Task->AttackPattern = InputPattern;
	Task->bDebugTask = bDebug;
	return Task;
}


void UAbilityTask_CombatInputReplication::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		if (bTestInitialState && IsLocallyControlled())
		{
			// Send the input replication events for the different attack patterns to the server, and do not overwrite the current attack pattern's input pressed event
			const TArray<FGameplayAbilitySpec>& Abilities = ASC->GetActivatableAbilities();
			for (const FGameplayAbilitySpec& CurrentAbility : Abilities)
			{
				if (!CurrentAbility.IsActive() && !CurrentAbility.InputPressed) continue;
				// if (AttackPattern == static_cast<EInputAbilities>(CurrentAbility.InputID)) continue;

				// Primary attack input
				if (EInputAbilities::PrimaryAttack == static_cast<EInputAbilities>(CurrentAbility.InputID))
				{
					OnPrimaryAttackCallback();
				}
				
				// Secondary attack input
				if (EInputAbilities::SecondaryAttack == static_cast<EInputAbilities>(CurrentAbility.InputID))
				{
					OnSecondaryAttackCallback();
				}
				
				// Special attack input
				if (EInputAbilities::SpecialAttack == static_cast<EInputAbilities>(CurrentAbility.InputID))
				{
					OnSpecialAttackCallback();
				}
				
				// Strong attack input
				if (EInputAbilities::StrongAttack == static_cast<EInputAbilities>(CurrentAbility.InputID))
				{
					OnStrongAttackCallback();
				}
			}
		}
		

		// Create the delegate functions to listen for input events
		bool bWaitingOnRemotePlayerData = false;
		if (AttackPattern != EInputAbilities::PrimaryAttack)
		{
			PrimaryAttackHandle.Handle = ASC->AbilityReplicatedEventDelegate(
				EAbilityGenericReplicatedEvent::GameCustom1,
				GetAbilitySpecHandle(),
				GetActivationPredictionKey()
			).AddUObject(this, &UAbilityTask_CombatInputReplication::OnPrimaryAttackCallback);
			
			if (IsForRemoteClient())
			{
				if (!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()))
				{
					bWaitingOnRemotePlayerData = true;
				}
			}
		}

		if (AttackPattern != EInputAbilities::SecondaryAttack)
		{
			SecondaryAttackHandle.Handle = ASC->AbilityReplicatedEventDelegate(
				EAbilityGenericReplicatedEvent::GameCustom2,
				GetAbilitySpecHandle(),
				GetActivationPredictionKey()
			).AddUObject(this, &UAbilityTask_CombatInputReplication::OnSecondaryAttackCallback);
			
			if (IsForRemoteClient())
			{
				if (!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom2, GetAbilitySpecHandle(), GetActivationPredictionKey()))
				{
					bWaitingOnRemotePlayerData = true;
				}
			}
		}

		if (AttackPattern != EInputAbilities::SpecialAttack)
		{
			SpecialAttackHandle.Handle = ASC->AbilityReplicatedEventDelegate(
				EAbilityGenericReplicatedEvent::GameCustom3,
				GetAbilitySpecHandle(),
				GetActivationPredictionKey()
			).AddUObject(this, &UAbilityTask_CombatInputReplication::OnSpecialAttackCallback);
			
			if (IsForRemoteClient())
			{
				if (!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom3, GetAbilitySpecHandle(), GetActivationPredictionKey()))
				{
					bWaitingOnRemotePlayerData = true;
				}
			}
		}

		if (AttackPattern != EInputAbilities::StrongAttack)
		{
			StrongAttackHandle.Handle = ASC->AbilityReplicatedEventDelegate(
				EAbilityGenericReplicatedEvent::GameCustom4,
				GetAbilitySpecHandle(),
				GetActivationPredictionKey()
			).AddUObject(this, &UAbilityTask_CombatInputReplication::OnStrongAttackCallback);
			
			if (IsForRemoteClient())
			{
				if (!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom4, GetAbilitySpecHandle(), GetActivationPredictionKey()))
				{
					bWaitingOnRemotePlayerData = true;
				}
			}
		}

		if (bWaitingOnRemotePlayerData)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}


void UAbilityTask_CombatInputReplication::OnPrimaryAttackCallback()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC)
	{
		return;
	}

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(PrimaryAttackHandle.Handle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	PrimaryAttackHandle.InputPressed = true;
	if (IsPredictingClient())
	{
		// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnPrimaryAttackInputPressed.Broadcast(ElapsedTime);
	}
	// EndTask();
}


void UAbilityTask_CombatInputReplication::OnSecondaryAttackCallback()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC)
	{
		return;
	}

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom2, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(SecondaryAttackHandle.Handle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	SecondaryAttackHandle.InputPressed = true;
	if (IsPredictingClient())
	{
		// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom2, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom2, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnSecondaryAttackInputPressed.Broadcast(ElapsedTime);
	}
	// EndTask();
}


void UAbilityTask_CombatInputReplication::OnSpecialAttackCallback()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC)
	{
		return;
	}

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom3, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(SpecialAttackHandle.Handle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	SpecialAttackHandle.InputPressed = true;
	if (IsPredictingClient())
	{
		// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom3, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom3, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnSpecialAttackInputPressed.Broadcast(ElapsedTime);
	}
	// EndTask();
}


void UAbilityTask_CombatInputReplication::OnStrongAttackCallback()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC)
	{
		return;
	}

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom4, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(StrongAttackHandle.Handle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	StrongAttackHandle.InputPressed = true;
	if (IsPredictingClient())
	{
		// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom4, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom4, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnStrongAttackInputPressed.Broadcast(ElapsedTime);
	}
	// EndTask();
}
