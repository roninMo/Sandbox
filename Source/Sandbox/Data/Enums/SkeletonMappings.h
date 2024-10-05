#pragma once


enum class ECharacterSkeletonMapping : uint8;
enum class EInputAbilities : uint8;

#include "CoreMinimal.h"
#include "SkeletonMappings.generated.h"



/** A reference for linking the proper skeleton to different characters without all the confusion while retrieving meshes and montages */
UENUM(BlueprintType)
enum class ECharacterSkeletonMapping : uint8
{
	None				UMETA(DisplayName = "None"),
	Manny				UMETA(DisplayName = "Manny"),
	Other				UMETA(DisplayName = "Other")
};


/**
 * An object containing the same montage for different characters. Used for untangling retrieving montages while having multiple characters in a game
 */
USTRUCT(BlueprintType)
struct F_CharacterToMontage
{
	GENERATED_USTRUCT_BODY()
	F_CharacterToMontage() = default;

	/** Character to montage map. Used for any montage, add the different character's montages for a specific montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<ECharacterSkeletonMapping, UAnimMontage*> MontageMappings;
};
