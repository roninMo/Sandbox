// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/CombatAbilitySystem.h"

void UCombatAbilitySystem::AbilityLocalInputPressed(int32 InputID)
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
		// Ability activation / input replication logic
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

					// Non replicated input press
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

		
		// Invoke input replication events of specific attack patterns to other attack patterns during combat
		// Helpful with certain attack that have specific combinations and multiple inputs for attacking
		if (
			Spec.InputID == static_cast<int32>(EInputAbilities::PrimaryAttack) ||
			Spec.InputID == static_cast<int32>(EInputAbilities::SecondaryAttack) ||
			Spec.InputID == static_cast<int32>(EInputAbilities::SpecialAttack) ||
			Spec.InputID == static_cast<int32>(EInputAbilities::StrongAttack) )
		{
			const EInputAbilities InputAttackPattern = static_cast<EInputAbilities>(InputID);
			if (Spec.Ability && Spec.IsActive())
			{
				// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server. @ref WaitInputPress
				if (InputAttackPattern == EInputAbilities::PrimaryAttack && Spec.InputID != InputID)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
				else if (InputAttackPattern == EInputAbilities::SecondaryAttack && Spec.InputID != InputID)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom2, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
				else if (InputAttackPattern == EInputAbilities::SpecialAttack && Spec.InputID != InputID)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom3, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
				else if (InputAttackPattern == EInputAbilities::StrongAttack && Spec.InputID != InputID)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom4, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
			}
			
		}
	}
}


void UCombatAbilitySystem::AbilityLocalInputReleased(int32 InputID)
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
