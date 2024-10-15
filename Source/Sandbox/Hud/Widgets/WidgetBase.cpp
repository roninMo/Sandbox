// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Hud/Widgets/WidgetBase.h"

#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"


void UWidgetBase::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}


void UWidgetBase::ShowWidget(float ZOrder)
{
	AddToViewport(ZOrder);
}


void UWidgetBase::HideWidget()
{
	RemoveFromParent();
}


ACharacterBase* UWidgetBase::GetCharacter_Implementation() const
{
	return Cast<ACharacterBase>(GetOwningPlayerPawn());
}


UCombatComponent* UWidgetBase::GetCombatComponent_Implementation(ACharacterBase* Character) const
{
	Character = Character ? Character : GetCharacter();
	if (!Character)
	{
		return nullptr;
	}

	return Character->GetCombatComponent();
}


UAbilitySystem* UWidgetBase::GetAbilitySystem_Implementation(ACharacterBase* Character) const
{
	Character = Character ? Character : GetCharacter();
	if (!Character)
	{
		return nullptr;
	}

	return Character->GetAbilitySystem<UAbilitySystem>();
}


UInventoryComponent* UWidgetBase::GetInventory_Implementation(ACharacterBase* Character) const
{
	Character = Character ? Character : GetCharacter();
	if (!Character)
	{
		return nullptr;
	}

	return Character->GetInventoryComponent();
}
