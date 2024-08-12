#pragma once

#include "CoreMinimal.h"
#include "AnimModiferInformation.generated.h"


/**
 * The information for creating an animation curve
 */
USTRUCT(BlueprintType)
struct F_AnimationCurveInfo
{
	GENERATED_USTRUCT_BODY()
		F_AnimationCurveInfo(
			const FName Name = FName(),
			const float DefaultValue = 0,
			const bool bAdjustFrames = false,
			const TArray<uint32>& Frames = {}
		) :

		Name(Name),
		DefaultValue(DefaultValue),
		bAdjustFrames(bAdjustFrames),
		Frames(Frames)
	{}

public:
	/** The name of the curve */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Name;
	
	/** The default values of the frames */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DefaultValue;
	
	/** If we should adjust every frame with a specific value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bAdjustFrames;
	
	/** The values for each frame of the animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<uint32> Frames;
};
