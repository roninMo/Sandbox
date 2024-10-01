#pragma once


#include "CoreMinimal.h"
#include "AISenseInformation.h"
#include "InventoryInformation.h"
#include "Engine/DataTable.h"
#include "Sandbox/Data/Enums/AISenseTypes.h"
#include "..\Enums\EquipSlot.h"
#include "NpcInformation.generated.h"

class UGameplayEffect;


/*
* The information pertaining to an npc character
*/
USTRUCT(BlueprintType)
struct F_NpcInformation
{
	GENERATED_USTRUCT_BODY()
		F_NpcInformation(
			const FName NpcId = FName(),
			const FString& PlatformId = FString(),
			
			const TArray<FS_Item>& InventoryItems = {},
			// const TArray<FS_WeaponInformation>& WeaponInformation = {},
			
			const EEquipSlot CurrentlyEquippedActiveWeapon = EEquipSlot::None,
			const EEquipSlot CurrentlyEquippedOffhandWeapon = EEquipSlot::None,
			const TArray<FS_Item>& LeftHandWeapons = {},
			const TArray<FS_Item>& RightHandWeapons = {},
			
			const FS_Item& Gauntlets = FS_Item(),
			const FS_Item& Leggings = FS_Item(),
			const FS_Item& Helm = FS_Item(),
			const FS_Item& Chest = FS_Item(),
			
			// const TSubclassOf<UGameplayEffect> PrimaryAttributes = nullptr,
			// const TArray<TSubclassOf<UGameplayEffect>>& Statuses = {},

			const F_AISenseConfigurations& SenseConfiguration = F_AISenseConfigurations(),
			ETeamId Team = ETeamId::Team_None
		) :
		NpcId(NpcId),
		PlatformId(PlatformId),
	
		CurrentlyEquippedActiveWeapon(CurrentlyEquippedActiveWeapon),
		CurrentlyEquippedOffhandWeapon(CurrentlyEquippedOffhandWeapon),
		LeftHandWeapons(LeftHandWeapons),
		RightHandWeapons(RightHandWeapons),
	
		Gauntlets(Gauntlets),
		Leggings(Leggings),
		Helm(Helm),
		Chest(Chest),
	
		InventoryItems(InventoryItems),
		// WeaponInformation(WeaponInformation),
		
		// PrimaryAttributes(PrimaryAttributes),
		// Statuses(Statuses),
		
		SenseConfiguration(SenseConfiguration),
		Team(Team)
	{}

public:
	virtual ~F_NpcInformation() {}
	/**** Equipment, stats, and inventory information ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName NpcId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString PlatformId;

	/**** Weapons ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEquipSlot CurrentlyEquippedActiveWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEquipSlot CurrentlyEquippedOffhandWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FS_Item> LeftHandWeapons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FS_Item> RightHandWeapons;

	/**** Armor ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_Item Gauntlets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_Item Leggings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_Item Helm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FS_Item Chest;

	/**** Inventory ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FS_Item> InventoryItems;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FS_WeaponInformation> WeaponInformation;

	/**** Stats ****/
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UGameplayEffect> PrimaryAttributes; // Health, Mana, etc
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TSubclassOf<UGameplayEffect>> Statuses; // Poison, buffs, etc
	
	/**** Behavior tree and combat npc information ****/
	/** The values of this character's senses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_AISenseConfigurations SenseConfiguration;

	/** The generic team that the player is on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ETeamId Team;

	/** The combat classification of this character (this is mainly for how different tasks are distributed to this character */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) ECombatClassification CombatClassification;

	/** Combo attacks that aren't specific to weapon that belong to this character */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> CombatNpcComboAttacks;
};


/*
* This is the data table to hold all the npc character information and class references for the game.
*/
USTRUCT(BlueprintType)
struct F_Table_NpcInformation : public FTableRowBase
{
	GENERATED_BODY()

public: UPROPERTY(EditAnywhere, BlueprintReadWrite) F_NpcInformation CharacterInformation;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};
