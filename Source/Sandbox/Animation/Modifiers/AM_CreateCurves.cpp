// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Animation/Modifiers/AM_CreateCurves.h"

#include "Sandbox/Data/Enums/MovementAnimCurveValues.h"


UAM_CreateCurves::UAM_CreateCurves(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Head, 1));

	// Montage Overrides
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Head, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Pelvis, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Spine, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Legs, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Arm_L, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Arm_R, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Hand_L, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Hand_R, 1));
	
	// Overlay Overrides
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Head, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Pelvis, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Spine, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Legs, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Arm_L, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Arm_R, 1));
	
	// IK influence
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Head, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Pelvis, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Spine, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Feet, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Arm_L, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Arm_R, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Hand_L, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Hand_R, 1));
}

void UAM_CreateCurves::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	if (!AnimationSequence) return;

	for (F_AnimationCurveInfo Curve : Curves)
	{
		// Delete curve if it already exists
		if (UAnimationBlueprintLibrary::DoesCurveExist(AnimationSequence, Curve.Name, ERawCurveTrackTypes::RCT_Float))
		{
			UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, Curve.Name);
		}

		// Add curve
		UAnimationBlueprintLibrary::AddCurve(AnimationSequence, Curve.Name);

		// Adjust values
		float Time = 0;
		UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, 0, Time);
		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, Curve.Name, Time, Curve.DefaultValue);

		int32 Frames = AnimationSequence->GetNumberOfSampledKeys();
		for (const int32 Frame : Frames)
		{
			UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, Frame, Time);
			UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, Curve.Name, Time, Curve.DefaultValue);
		}
	}
}
