#pragma once


#include "CoreMinimal.h"
#include "AISenseInformation.h"
#include "InventoryInformation.h"
#include "Engine/DataTable.h"
#include "NpcInformation.generated.h"

class UArmorData;
class UAbilityData;
class UEquipmentData;
class UAttributeData;
class UCharacterAbilityDataSet;
class UEnemyEquipmentDataSet;
enum class ECombatClassification : uint8;
enum class ETeamId : uint8;
enum class EEquipSlot : uint8;
struct F_ArmamentAbilityInformation;
class UCharacterGameplayAbility;
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

	/**** Inventory ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FS_Item> InventoryItems;
	
	/**** Behavior tree and combat npc information ****/
	/** The values of this character's senses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_AISenseConfigurations SenseConfiguration;

	/** The generic team that the player is on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ETeamId Team;

	/** The combat classification of this character (this is mainly for how different tasks are distributed to this character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ECombatClassification CombatClassification;

	/** Combo attacks that aren't specific to weapon that belong to this character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> AttackPatterns;
};


/* This is the data table to hold the npc character information and class references for the game */
USTRUCT(BlueprintType)
struct F_Table_NpcInformation : public FTableRowBase
{
	GENERATED_BODY()

public: UPROPERTY(EditAnywhere, BlueprintReadWrite) F_NpcInformation CharacterInformation;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};



/* This is the data table to hold attribute information for characters in the game */
USTRUCT(BlueprintType)
struct F_Table_AttributeData : public FTableRowBase
{
	GENERATED_BODY()

public: UPROPERTY(EditAnywhere, BlueprintReadWrite) UAttributeData* AttributeData;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};


/* This is the data table to hold equipment information for characters in the game */
USTRUCT(BlueprintType)
struct F_Table_EquipmentData : public FTableRowBase
{
	GENERATED_BODY()

public: UPROPERTY(EditAnywhere, BlueprintReadWrite) UEquipmentData* EquipmentData;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};


/* This is the data table to hold armor information for characters in the game */
USTRUCT(BlueprintType)
struct F_Table_ArmorData : public FTableRowBase
{
	GENERATED_BODY()

public: UPROPERTY(EditAnywhere, BlueprintReadWrite) UArmorData* ArmorData;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};


/* This is the data table to hold ability information for characters in the game */
USTRUCT(BlueprintType)
struct F_Table_AbilityData : public FTableRowBase
{
	GENERATED_BODY()

public: UPROPERTY(EditAnywhere, BlueprintReadWrite) UAbilityData* AbilityData;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};









/* Different Npc combat information */
USTRUCT(BlueprintType)
struct F_Table_NpcCombatInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UEnemyEquipmentDataSet* Equipment;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UCharacterAbilityDataSet* AttributeAndAbilityData;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};
