// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Animation/Modifiers/AM_CreateCurves.h"

#include "Sandbox/Data/Enums/MovementAnimCurveValues.h"
#include "AnimationBlueprintLibrary.h"


UAM_CreateCurves::UAM_CreateCurves()
{
	// Montage Overrides
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Head, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Pelvis, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Spine, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Legs, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Arm_L, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Arm_R, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Hand_L, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Montage_Hand_R, 0));
	
	// Overlay Overrides
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Head, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Pelvis, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Spine, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Legs, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Arm_L, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_Layering_Arm_R, 0));
	
	// IK influence
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Head, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Pelvis, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Spine, 1));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Feet, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Arm_L, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Arm_R, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Hand_L, 0));
	Curves.Add(F_AnimationCurveInfo(Curve_IK_Hand_R, 0));
}

void UAM_CreateCurves::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	if (!AnimationSequence) return;
	
	for (const F_AnimationCurveInfo Curve : Curves)
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

		if (Curve.bAdjustFrames)
		{
			const int32 Frames = AnimationSequence->GetNumberOfSampledKeys();
			for (int32 Frame = 0; Frame < Frames; Frame++)
			{
				UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, Frame, Time);
				UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, Curve.Name, Time, Curve.DefaultValue);
			}
		}
	}
}
