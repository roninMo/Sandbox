#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterMontages.generated.h"



/**
 * This object holds all the montages for this character
 */
USTRUCT(BlueprintType)
struct FM_CharacterMontages
{
	GENERATED_USTRUCT_BODY()
		FM_CharacterMontages(
			UAnimMontage* RollMontage = nullptr,
			UAnimMontage* HitReactMontage = nullptr,
			UAnimMontage* DeathMontage = nullptr,
			UAnimMontage* BlockMontage = nullptr
		) :
    
		RollMontage(RollMontage),
		HitReactMontage(HitReactMontage),
		DeathMontage(DeathMontage),
		BlockMontage(BlockMontage)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* RollMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* BlockMontage;
};


/**
 * This is the data table to hold all the montages for a specific character
 */
USTRUCT(BlueprintType)
struct F_Table_CharacterMontages : public FTableRowBase
{
	GENERATED_BODY()

public:     UPROPERTY(EditAnywhere, BlueprintReadWrite) FM_CharacterMontages Montages;
protected:  UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};
