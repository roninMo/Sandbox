// Fill out your copyright notice in the Description page of Project Settings.


#include "Saving_Attributes.h"

#include "Kismet/GameplayStatics.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Save/Attributes/Saved_Attributes.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"

bool USaving_Attributes::SaveData_Implementation(int32 Index)
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
	FString AttributeSaveSlot = SaveComponent->GetSaveUrl(SaveType);
	UGameplayStatics::SaveGameToSlot(SaveInformation, AttributeSaveSlot, SaveComponent->GetUserIndex());
	return true;
}


bool USaving_Attributes::LoadData_Implementation(int32 Index)
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
	FString AttributeSaveSlot = SaveComponent->GetSaveUrl(SaveType);
	USaved_Attributes* SavedAttributes = Cast<USaved_Attributes>(UGameplayStatics::LoadGameFromSlot(AttributeSaveSlot, SaveComponent->GetUserIndex()));
	if (!SavedAttributes)
	{
		return false;
	}

	// Add the attribute information to the ability system for replication
	Character->GetAbilitySystem<UAbilitySystem>()->SavedAttributes = SavedAttributes->GetCurrentAttributes(AttributeSet);
	return true;
}


FString USaving_Attributes::FormattedSaveInformation(const FString Slot) const
{
	// Retrieve Saved Combat Info
	USaved_Attributes* Attributes = Cast<USaved_Attributes>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
	if (!Attributes)
	{
		return FString();
	}

	// Retrieve the primary and secondary weapon information
	TArray<FString> Results;
	Results.Add(FString::Printf(TEXT("Current Attributes")));
	Results.Add(FString::Printf(TEXT("{")));

	Results.Add(FString::Printf(TEXT("\tHealth: %f,"), Attributes->Health));
	Results.Add(FString::Printf(TEXT("\tStamina: %f,"), Attributes->Stamina));
	Results.Add(FString::Printf(TEXT("\tPoise: %f,"), Attributes->Poise));
	Results.Add(FString::Printf(TEXT("\tMana: %f,"), Attributes->Mana));
	
	Results.Add(FString::Printf(TEXT("")));
	Results.Add(FString::Printf(TEXT("\tStatuses: {")));
	Results.Add(FString::Printf(TEXT("\t\tExperience: %f,"), Attributes->Experience));
	Results.Add(FString::Printf(TEXT("\t\tBleedBuildup: %f,"), Attributes->BleedBuildup));
	Results.Add(FString::Printf(TEXT("\t\tFrostbiteBuildup: %f,"), Attributes->FrostbiteBuildup));
	Results.Add(FString::Printf(TEXT("\t\tPoisonBuildup: %f,"), Attributes->PoisonBuildup));
	Results.Add(FString::Printf(TEXT("\t\tMadnessBuildup: %f,"), Attributes->MadnessBuildup));
	Results.Add(FString::Printf(TEXT("\t\tCurseBuildup: %f,"), Attributes->CurseBuildup));
	Results.Add(FString::Printf(TEXT("\t\tSleepBuildup: %f,"), Attributes->SleepBuildup));
	Results.Add(TEXT("\t}"));
	
	Results.Add(FString::Printf(TEXT("}")));
	return FString::Join(Results, TEXT("\n"));
}

/*

{
	Health:	 0,
	Stamina: 0,
	Poise:	 0,
	Mana:	 0,
	
	Statuses: {
		Experience: 0,
		BleedBuildup: 0,
		FrostbiteBuildup: 0,
		PoisonBuildup: 0,
		MadnessBuildup: 0,
		CurseBuildup: 0,
		SleepBuildup: 0,
	}
}



*/
