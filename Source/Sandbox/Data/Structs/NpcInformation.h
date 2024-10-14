#pragma once


#include "CoreMinimal.h"
#include "AISenseInformation.h"
#include "InventoryInformation.h"
#include "Engine/DataTable.h"
#include "NpcInformation.generated.h"

enum class ETeamId : uint8;
enum class EEquipSlot : uint8;
class UGameplayEffect;


/*
* The information pertaining to an npc character
*/
USTRUCT(BlueprintType)
struct F_NpcInformation
{
	GENERATED_USTRUCT_BODY()
	
public:
	F_NpcInformation() = default;

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
