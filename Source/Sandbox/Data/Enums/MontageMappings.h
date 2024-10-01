#pragma once



/** A reference for linking the proper montages to different characters without all the confusion while retrieving montages */
UENUM(BlueprintType)
enum class ECharacterToMontageMapping : uint8
{
	CTM_Manny				UMETA(DisplayName = "Manny"),
};
