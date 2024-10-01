#pragma once



/** A reference for linking the proper skeleton to different characters without all the confusion while retrieving meshes and montages */
UENUM(BlueprintType)
enum class ECharacterSkeletonMapping : uint8
{
	Manny				UMETA(DisplayName = "Manny"),
	Other				UMETA(DisplayName = "Other")
};
