// Fill out your copyright notice in the Description page of Project Settings.


#include "Saving_Attributes.h"

#include "Kismet/GameplayStatics.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Save/Attributes/Saved_Attributes.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"

bool USaving_Attributes::SaveData_Implementation()
{
	ACharacterBase* Character;
	USaveComponent* SaveComponent;
	if (!GetCharacterAndSaveComponent(SaveComponent, Character) || !Character->GetAbilitySystem<UAbilitySystem>())
	{
		return false;
	}

	// We need the player's attribute set
	UMMOAttributeSet* AttributeSet = Cast<UMMOAttributeSet>(UGameplayAbilityUtilities::GetAttributeSet(Character->GetAbilitySystemComponent(), UMMOAttributeSet::StaticClass()));
	if (!AttributeSet)
	{
		return false;
	}

	USaved_Attributes* SaveInformation = NewObject<USaved_Attributes>();
	if (!SaveInformation)
	{
		return false;
	}
	
	// Save the character's current attributes
	SaveInformation->RetrieveAttributesFromAttributeSet(AttributeSet);
	FString AttributeSaveSlot = SaveComponent->GetSaveSlotReference(SaveType);
	UGameplayStatics::SaveGameToSlot(SaveInformation, AttributeSaveSlot, 0);
	return true;
}


bool USaving_Attributes::LoadData_Implementation()
{
	ACharacterBase* Character;
	USaveComponent* SaveComponent;
	if (!GetCharacterAndSaveComponent(SaveComponent, Character) || !Character->GetAbilitySystem<UAbilitySystem>())
	{
		return false;
	}

	// We need the player's attribute set
	UAttributeSet* AttributeSet = nullptr;
	UGameplayAbilityUtilities::GetAttributeSet(Character->GetAbilitySystemComponent(), UMMOAttributeSet::StaticClass());
	if (!AttributeSet)
	{
		return false;
	}

	// Retrieve the attribute information
	FString AttributeSaveSlot = SaveComponent->GetSaveSlotReference(SaveType);
	USaved_Attributes* SavedAttributes = Cast<USaved_Attributes>(UGameplayStatics::LoadGameFromSlot(AttributeSaveSlot, 0));
	if (!SavedAttributes)
	{
		return false;
	}

	// Add the attribute information to the ability system for replication
	Character->GetAbilitySystem<UAbilitySystem>()->SavedAttributes = SavedAttributes->GetCurrentAttributes(AttributeSet);
	return true;
}
