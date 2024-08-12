#pragma once


// Montage Overrides // 0 = Disabled, 1 = Enabled
#define Curve_Montage_Head FName("Montage_Head")
#define Curve_Montage_Pelvis FName("Montage_Pelvis")
#define Curve_Montage_Spine FName("Montage_Spine")
#define Curve_Montage_Legs FName("Montage_Legs")
#define Curve_Montage_Arm_L FName("Montage_Arm_L")
#define Curve_Montage_Arm_R FName("Montage_Arm_R")
#define Curve_Montage_Hand_L FName("Montage_Hand_L")
#define Curve_Montage_Hand_R FName("Montage_Hand_R")

// Overlay Overrides // 0 = Disabled, 1 = Enabled
#define Curve_Layering_Head FName("Layering_Head")
#define Curve_Layering_Pelvis FName("Layering_Pelvis")
#define Curve_Layering_Spine FName("Layering_Spine")
#define Curve_Layering_Legs FName("Layering_Legs")
#define Curve_Layering_Arm_L FName("Layering_Arm_L")
#define Curve_Layering_Arm_R FName("Layering_Arm_R")

// IK influence // 0 = Disabled, 1 = Enabled
#define Curve_IK_Head FName("IK_Head")
#define Curve_IK_Pelvis FName("IK_Pelvis")
#define Curve_IK_Spine FName("IK_Spine")
#define Curve_IK_Feet FName("IK_Feet")
#define Curve_IK_Arm_L FName("IK_Arm_L")
#define Curve_IK_Arm_R FName("IK_Arm_R")
#define Curve_IK_Hand_L FName("IK_Hand_L")
#define Curve_IK_Hand_R FName("IK_Hand_R")

// AO influence // 0 = Disabled, 1 = Enabled
#define Curve_AO_Head FName("AO_Head")
#define Curve_AO_Pelvis FName("AO_Pelvis")
#define Curve_AO_Spine FName("AO_Spine")
#define Curve_AO_Legs FName("AO_Legs")
#define Curve_AO_Arm_L FName("AO_Arm_L")
#define Curve_AO_Arm_R FName("AO_Arm_R")
#define Curve_AO_Hand_L FName("AO_Hand_L")
#define Curve_AO_Hand_R FName("AO_Hand_R")

// Primary values
#define Curve_Feet_Plant FName("Feet_Plant") // -1 = Left foot, 1 = Right foot
#define Curve_Turn_RotationAmount FName("Turn_RotationAmount") // -1, 1 = Left, Right
#define Curve_Mask_Sprint FName("Mask_Sprint") // 1 = Prevent sprint
#define Curve_Mask_Lean FName("Mask_Lean") // 1 = Prevent lean
