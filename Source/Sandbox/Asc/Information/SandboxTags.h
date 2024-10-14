#pragma once

#include "NativeGameplayTags.h"


/*
 
	Yeah, we’ve looked at this solution as well but the one I posted seems cleaner and more robust. I am not even sure that these macros will work in all cases (read more below).
	Having a custom TagManager singleton is a concept that I’ve taken from Epic’s Lyra project which is supposed to be representing good practices. The only thing that was not optimal there is that they initialized the Native Tags through a custom Asset Manager class. But that proved to be problematic in some cases (Asset Manager initialization happens way after Module Initialization)
	We found out that when Native Tags are referenced in a Blueprint calling Static Functions, UE tries to load and read the Blueprint even before the Engine is started. If Native Tags are not loaded at that point, this Blueprint will become faulty.

		
*/



// ;Ability System Debugging Tags
#define Tag_Activation_Fail_BlockedByTags FName("Activation.Fail.BlockedByTags")
#define Tag_Activation_Fail_CantAffordCost FName("Activation.Fail.CantAffordCost")
#define Tag_Activation_Fail_IsDead FName("Activation.Fail.IsDead")
#define Tag_Activation_Fail_MissingTags FName("Activation.Fail.MissingTags")
#define Tag_Activation_Fail_Networking FName("Activation.Fail.Networking")
#define Tag_Activation_Fail_OnCooldown FName("Activation.Fail.OnCooldown")
#define Tag_Activation_WeaponEquipped FName("Activation.WeaponEquipped")


// ; Gameplay Abilities
#define Tag_GameplayAbility FName("GameplayAbility")
#define Tag_GameplayAbility_Unequip FName("GameplayAbility.Unequip")
#define Tag_GameplayAbility_Equip FName("GameplayAbility.Equip")
#define Tag_GameplayAbility_PrimaryAttack FName("GameplayAbility.PrimaryAttack")
#define Tag_GameplayAbility_SecondaryAttack FName("GameplayAbility.SecondaryAttack")
#define Tag_GameplayAbility_SpecialAttack FName("GameplayAbility.SpecialAttack")
#define Tag_GameplayAbility_StrongAttack FName("GameplayAbility.StrongAttack")
#define Tag_GameplayAbility_MeleeAttack FName("GameplayAbility.MeleeAttack")
#define Tag_GameplayAbility_Block FName("GameplayAbility.Block")
#define Tag_GameplayAbility_Parry FName("GameplayAbility.Parry")
#define Tag_GameplayAbility_Aim FName("GameplayAbility.Aim")
#define Tag_GameplayAbility_Reload FName("GameplayAbility.Reload")
#define Tag_GameplayAbility_Sprint FName("GameplayAbility.Sprint")
#define Tag_GameplayAbility_Roll FName("GameplayAbility.Roll")
#define Tag_GameplayAbility_Crouch FName("GameplayAbility.Crouch")
#define Tag_GameplayAbility_Jump FName("GameplayAbility.Jump")


// ; Gameplay Cues


// ; Gameplay Effects
#define Tag_GameplayEffect FName("GameplayEffect")
#define Tag_GameplayEffect_Drain FName("GameplayEffect.Drain")
#define Tag_GameplayEffect_Drain_Health FName("GameplayEffect.Drain.Health")
#define Tag_GameplayEffect_Drain_Stamina FName("GameplayEffect.Drain.Stamina")

#define Tag_GameplayEffect_Regen FName("GameplayEffect.Regen")
#define Tag_GameplayEffect_Regen_Health FName("GameplayEffect.Regen.Health")
#define Tag_GameplayEffect_Regen_Stamina FName("GameplayEffect.Regen.Stamina")

#define Tag_GameplayEffect_Attack FName("GameplayEffect.Attack")
#define Tag_GameplayEffect_Attack_HitStun FName("GameplayEffect.Attack.HitStun")

#define Tag_GameplayEffect_Block FName("GameplayEffect.Block")
#define Tag_GameplayEffect_Block_Regen FName("GameplayEffect.Block.Regen")
#define Tag_GameplayEffect_Block_Regen_Health FName("GameplayEffect.Block.Regen.Health")
#define Tag_GameplayEffect_Block_Regen_Poise FName("GameplayEffect.Block.Regen.Poise")
#define Tag_GameplayEffect_Block_Regen_Stamina FName("GameplayEffect.Block.Regen.Stamina")
#define Tag_GameplayEffect_Block_Regen_Mana FName("GameplayEffect.Block.Regen.Mana")
#define Tag_GameplayEffect_Block_Buildup FName("GameplayEffect.Block.Buildup")
#define Tag_GameplayEffect_Block_Buildup_Curse FName("GameplayEffect.Block.Buildup.Curse")
#define Tag_GameplayEffect_Block_Buildup_Bleed FName("GameplayEffect.Block.Buildup.Bleed")
#define Tag_GameplayEffect_Block_Buildup_Poison FName("GameplayEffect.Block.Buildup.Poison")
#define Tag_GameplayEffect_Block_Buildup_Frostbite FName("GameplayEffect.Block.Buildup.Frostbite")
#define Tag_GameplayEffect_Block_Buildup_Madness FName("GameplayEffect.Block.Buildup.Madness")
#define Tag_GameplayEffect_Block_Buildup_Sleep FName("GameplayEffect.Block.Buildup.Sleep")

#define Tag_GameplayEffect_Status FName("GameplayEffect.Status")
#define Tag_GameplayEffect_Status_Poison FName("GameplayEffect.Status.Poison")
#define Tag_GameplayEffect_Status_Curse FName("GameplayEffect.Status.Curse")
#define Tag_GameplayEffect_Status_Frostbitten FName("GameplayEffect.Status.Frostbitten")
#define Tag_GameplayEffect_Status_Madness FName("GameplayEffect.Status.Madness")
#define Tag_GameplayEffect_Status_Sleep FName("GameplayEffect.Status.Sleep")


// ; Gameplay Ability Tag Events
#define Tag_Event_Montage FName("Event.Montage")
#define Tag_Event_Montage_SpawnProjectile FName("Event.Montage.SpawnProjectile")
#define Tag_Event_Montage_Action FName("Event.Montage.Action")


// ; Movement
#define Tag_Movement FName("Movement")
#define Tag_Movement_Crouching FName("Movement.Crouching")
#define Tag_Movement_Sliding FName("Movement.Sliding")
#define Tag_Movement_Sprinting FName("Movement.Sprinting")
#define Tag_Movement_Falling FName("Movement.Falling")
#define Tag_Movement_Aiming FName("Movement.Aiming")
#define Tag_Movement_Jumping FName("Movement.Jumping")
#define Tag_Movement_Rolling FName("Movement.Rolling")
#define Tag_Movement_WallRunning FName("Movement.WallRunning")
#define Tag_Movement_WallClimbing FName("Movement.WallClimbing")
#define Tag_Movement_WallMantling FName("Movement.WallMantling")
#define Tag_Movement_WallLedgeClimbing FName("Movement.WallLedgeClimbing")


// ; Blocks (state handles for preventing things like regen and status buildups)
#define Tag_Block FName("Block")
#define Tag_Block_Regen FName("Block.Regen")
#define Tag_Block_Regen_Health FName("Block.Regen.Health")
#define Tag_Block_Regen_Poise FName("Block.Regen.Poise")
#define Tag_Block_Regen_Stamina FName("Block.Regen.Stamina")
#define Tag_Block_Regen_Mana FName("Block.Regen.Mana")

#define Tag_Block_Buildup FName("Block.Buildup")
#define Tag_Block_Buildup_Curse FName("Block.Buildup.Curse")
#define Tag_Block_Buildup_Bleed FName("Block.Buildup.Bleed")
#define Tag_Block_Buildup_Poison FName("Block.Buildup.Poison")
#define Tag_Block_Buildup_Frostbite FName("Block.Buildup.Frostbite")
#define Tag_Block_Buildup_Madness FName("Block.Buildup.Madness")
#define Tag_Block_Buildup_Sleep FName("Block.Buildup.Sleep")


// ; Passives
#define Tag_Passive FName("Passive")
#define Tag_Passive_Drain FName("Passive.Drain")
#define Tag_Passive_Drain_Health FName("Passive.Drain.Health")
#define Tag_Passive_Drain_Stamina FName("Passive.Drain.Stamina")

#define Tag_Passive_Regen FName("Passive.Regen")
#define Tag_Passive_Regen_Health FName("Passive.Regen.Health")
#define Tag_Passive_Regen_Stamina FName("Passive.Regen.Stamina")


// ; States
#define Tag_Status FName("Status")
#define Tag_Status_Poison FName("Status.Poison")
#define Tag_Status_Curse FName("Status.Curse")
#define Tag_Status_Frostbite FName("Status.Frostbite")
#define Tag_Status_Madness FName("Status.Madness")
#define Tag_Status_Sleep FName("Status.Sleep")


// ; States
#define Tag_State FName("State")
#define Tag_State_Slowed FName("State.Slowed")
#define Tag_State_HitStun FName("State.HitStun")
#define Tag_State_Dead FName("State.Dead")
#define Tag_State_Stunned FName("State.Stunned")

#define Tag_State_Armament FName("State.Armament")
#define Tag_State_Armament_Equipping FName("State.Armament.Equipping")
#define Tag_State_Armament_Unequipping FName("State.Armament.Unequipping")

#define Tag_State_Attacking FName("State.Attacking")
#define Tag_State_Attacking_Charging FName("State.Attacking.Charging")
#define Tag_State_Attacking_Startup FName("State.Attacking.Startup")
#define Tag_State_Attacking_AttackFrames FName("State.Attacking.AttackFrames")
#define Tag_State_Attacking_AttackFrames_End FName("State.Attacking.AttackFrames.End")
#define Tag_State_Attacking_AttackFrames_End_L FName("State.Attacking.AttackFrames.End.L")
#define Tag_State_Attacking_AttackFrames_End_R FName("State.Attacking.AttackFrames.End.R")
#define Tag_State_Attacking_AttackFrames_Begin FName("State.Attacking.AttackFrames.Begin")
#define Tag_State_Attacking_AttackFrames_Begin_L FName("State.Attacking.AttackFrames.Begin.L")
#define Tag_State_Attacking_AttackFrames_Begin_R FName("State.Attacking.AttackFrames.Begin.R")
#define Tag_State_Attacking_AllowMovement FName("State.Attacking.AllowMovement")
#define Tag_State_Invincibility FName("State.Invincibility")


// ; UI
#define Tag_UI FName("UI")
#define Tag_UI_Action FName("UI.Action")
#define Tag_UI_Action_Cancel FName("UI.Action.Cancel")
#define Tag_UI_Action_Confirm FName("UI.Action.Confirm")
#define Tag_UI_Action_NextTab FName("UI.Action.NextTab")
#define Tag_UI_Action_PreviousTab FName("UI.Action.PreviousTab")

#define Tag_UI_Message FName("UI.Message")
#define Tag_UI_Message_Potion_Health FName("UI.Message.Potion.Health")
#define Tag_UI_Message_Potion_HealthRegen FName("UI.Message.Potion.HealthRegen")
#define Tag_UI_Message_Potion_Mana FName("UI.Message.Potion.Mana")
#define Tag_UI_Message_Potion_ManaRegen FName("UI.Message.Potion.ManaRegen")
#define Tag_UI_Message_Potion_Stamina FName("UI.Message.Potion.Stamina")
#define Tag_UI_Message_Potion_StaminaRegen FName("UI.Message.Potion.StaminaRegen")



