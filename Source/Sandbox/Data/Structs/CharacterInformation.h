#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterInformation.generated.h"

class UAbilityData;
class UArmorData;
class UEquipmentData;
class UAttributeData;


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
