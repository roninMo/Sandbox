#pragma once


#include "CoreMinimal.h"
#include "AbilityInformation.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Engine/DataTable.h"
#include "ArmorInformation.generated.h"

struct FGameplayEffectInfo;
class UCharacterGameplayAbility;
class UGameplayEffect;
struct F_Item;
enum class EArmorSlot : uint8;


/**
 * Handle for storing the armor information that's been granted from it
 */
USTRUCT(BlueprintType)
struct F_Information_Armor_Handle
{
    GENERATED_USTRUCT_BODY()

	/** The id of the armor from table */
	UPROPERTY() FGuid Id;
	
	/** The attributes that were granted from the armor */
	UPROPERTY() FActiveGameplayEffectHandle ArmorStats;
	
	/** The stored passives that were granted from the armor */
	UPROPERTY() TArray<FActiveGameplayEffectHandle> PassiveHandles;

	/** The stored abilities that were granted from the armor */
	UPROPERTY() TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	
	/** Default constructor */
	F_Information_Armor_Handle() = default;

	/** Returns whether the handle is valid by checking the original Ability Set pathname */
	bool IsValid() const
	{
		return !Id.IsValid() || !ArmorStats.IsValid();
	}

	/** Cleans up the handle structure for any stored handles and pathname*/
	void Invalidate()
	{
		Id.Invalidate();
		ArmorStats.Invalidate();
		PassiveHandles.Empty();
		AbilityHandles.Empty();
	}

	/** Returns a String representation of the Armor */
	FString ToString(const bool bVerbose = false) const
	{
		TArray<FString> Results;
		Results.Add(FString::Printf(
			TEXT("Id: %s, Armor Stats: %s, Passive Handles: %d, Ability Handles: %d"),
			*Id.ToString(),
			*ArmorStats.ToString(),
			PassiveHandles.Num(),
			AbilityHandles.Num()
		));

		// No verbose output, only print high lvl info
		if (!bVerbose)
		{
			return FString::Join(Results, LINE_TERMINATOR);
		}

		Results.Add(FString::Printf(TEXT("\t - Armor Stats Handle: %s"), *ArmorStats.ToString()));

		Results.Add(FString::Printf(TEXT("Passive Handles: %d"), PassiveHandles.Num()));
		for (const FActiveGameplayEffectHandle& PassiveHandle : PassiveHandles)
		{
			Results.Add(FString::Printf(TEXT("\t - Passive Handle: %s"), *PassiveHandle.ToString()));
		}

		Results.Add(FString::Printf(TEXT("Ability Handles: %d"), AbilityHandles.Num()));
		for (const FGameplayAbilitySpecHandle& AbilityHandle : AbilityHandles)
		{
			Results.Add(FString::Printf(TEXT("\t - Ability Handle: %s"), *AbilityHandle.ToString()));
		}

		return FString::Join(Results, LINE_TERMINATOR);
	}

	friend bool operator==(const F_Information_Armor_Handle& LHS, const F_Information_Armor_Handle& RHS)
	{
		return LHS.Id == RHS.Id;
	}

	friend bool operator!=(const F_Information_Armor_Handle& LHS, const F_Information_Armor_Handle& RHS)
	{
		return !(LHS == RHS);
	}
};




/**
 * The information and stats of an armor
 */
USTRUCT(BlueprintType)
struct F_Information_Armor
{
    GENERATED_USTRUCT_BODY()
    F_Information_Armor() = default;

	/** The database id of the armor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;
	
	/** The slot the armor goes in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EArmorSlot ArmorSlot;

	/** The armor mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMesh* ArmorMesh;

	/** Gameplay effect for adjusting the player's stats from the armor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayEffectInfo ArmorStats;

	/** The passives the armor grants to the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FGameplayEffectInfo> Passives;

	/** The abilities the armor grants to the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FGameplayAbilityInfo> Abilities;
};




/**
 * This is the data table that holds all the armor
 */
USTRUCT(BlueprintType)
struct F_Table_Armors : public FTableRowBase
{
    GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_Information_Armor ArmorInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};
