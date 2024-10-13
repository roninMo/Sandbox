// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/Melee/MeleeCombatAbility.h"

#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Data/Enums/AttackFramesState.h"
#include "Sandbox/Data/Enums/EquipSlot.h"

#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Characters/Components/AdvancedMovement/AdvancedMovementComponent.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Logging/StructuredLog.h"


UMeleeCombatAbility::UMeleeCombatAbility()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_GameplayAbility_MeleeAttack));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Unequipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Armament_Equipping));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_Attacking));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_State_HitStun));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Sliding));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(Tag_Movement_Rolling));
	
	AllowMovementTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AllowMovement);
	AttackFramesTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames);
	HitStunTag = FGameplayTag::RequestGameplayTag(Tag_State_HitStun);
	StaminaCostEffectTag = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Drain_Stamina);
	
	AttackFramesEndTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_End);
	LeftHandAttackFramesEndTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_End_L);
	RightHandAttackFramesEndTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_End_R);
	
	AttackFramesBeginTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_Begin);
	LeftHandAttackFramesBeginTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_Begin_L);
	RightHandAttackFramesBeginTag = FGameplayTag::RequestGameplayTag(Tag_State_Attacking_AttackFrames_Begin_R);
}


void UMeleeCombatAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMeleeCombatAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UMeleeCombatAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}




#pragma region Initialization
bool UMeleeCombatAbility::SetArmamentInformation_Implementation()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component during {2} while getting combo and armament information",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return false;
	}
	
	// Retrieve the attack pattern from the actual input
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	if (Spec)
	{
		AttackPattern = static_cast<EInputAbilities>(Spec->InputID);
		CurrentStance = CombatComponent->GetCurrentStance();
	}
	
	AArmament* EquippedArmament = CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons ?
		CombatComponent->GetArmament(true) : CombatComponent->GetArmament(IsRightHandAbility());
	if (!EquippedArmament || EquippedArmament->GetEquipStatus() != EEquipStatus::Equipped)
	{
		if (!EquippedArmament)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the armament during {2} while getting combo and armament information",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		}
		return false;
	}

	// Retrieve the combo information if the player's equipped an armament or their stance has updated
	if (EquippedArmament == Armament && CurrentStance == CombatComponent->GetCurrentStance() && EquipSlot == EquippedArmament->GetEquipSlot()) return true;
	else
	{
		Armament = nullptr;
		RunningAttackInformation = F_ComboAttacks();
		CrouchingAttackInformation = F_ComboAttacks();
		CurrentAttack = F_ComboAttack();
		ComboAttacks = F_ComboAttacks();
		ComboCount = 0;
		SetCurrentMontage(nullptr);
		EquipSlot = EEquipSlot::None;
		OffhandEquipSlot = EEquipSlot::None;
		OffhandArmament = nullptr;
	}
	
	
	// Retrieve the attacks for the current stance. If the armament has a specific attack pattern ability without combat information, notify us to prevent any problems
	const F_ArmamentInformation& ArmamentInformation = EquippedArmament->GetArmamentInformation();
	for (const auto& [Ability, InputId, InvalidStances, Level] : ArmamentInformation.MeleeAbilities)
	{
		if (AttackPattern != InputId || InvalidStances.Contains(CombatComponent->GetCurrentStance())) continue;

		// If there's no combat information for this attack, then either the information is missing or the ability was added at the wrong time
		Armament = EquippedArmament;
		ComboAttacks = Armament->GetComboAttacks(AttackPattern);
		ComboCount = ComboAttacks.ComboAttacks.Num();
		SetCurrentMontage(EquippedArmament->GetCombatMontage(AttackPattern));
		EquipSlot = EquippedArmament->GetEquipSlot();
		return true;
	}

	if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons)
	{
		OffhandArmament = CombatComponent->GetArmament(false);
		OffhandEquipSlot = OffhandArmament ? OffhandArmament->GetEquipSlot() : EEquipSlot::None;
	}
	
	ensure(!ComboAttacks.ComboAttacks.IsEmpty());
	return false;
}


void UMeleeCombatAbility::InitCombatInformation_Implementation()
{
	Super::InitCombatInformation_Implementation();

	// Reset state using handle
	PrimaryAttackState = EAttackFramesState::Disabled;
	SecondaryAttackState = EAttackFramesState::Disabled;
	bCrouchingAttack = IsValidForCrouchAttack();
	bRunningAttack = IsValidForRunningAttack();
	
	
	SetComboIndex();
	SetComboAttack(); // Current attack and combo index
	SetAttackMontage(Armament); // Current montage
	SetMontageStartSection(); // montage start section (customization for different types of attacks
	CalculateAttributeModifications(); // Damage and attribute calculations

	UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} {3}::{4}{5}, ComboIndex: {6}, Montage: {7}({8})",
		UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()),
		bRunningAttack ? FString("(RunningAttack)") : bCrouchingAttack ? FString("(CrouchAttack)") : FString(""),
		*GetNameSafe(Armament), *UEnum::GetValueAsString(AttackPattern), ComboIndex, *GetNameSafe(GetCurrentMontage()), MontageStartSection
	);
}


void UMeleeCombatAbility::SetComboIndex_Implementation()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while adjusting the combo index",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		ComboIndex = 0;
		return;
	}

	// Either have separate combo indexing for normal / strong attacks, or when transitioning from one to the other have that finish the combo. Having both combined would be interesting I just think it'd be tough to balance
	ComboIndex = CombatComponent ? CombatComponent->GetComboIndex() : 0;
	const TArray<F_ComboAttack>& Attacks = bRunningAttack ? RunningAttackInformation.ComboAttacks : bCrouchingAttack ? CrouchingAttackInformation.ComboAttacks : ComboAttacks.ComboAttacks;
	if (ComboIndex + 1 >= Attacks.Num())
	{

		// We got here from another attack pattern's combo, adjust the index to 1 to prevent doing the initial attack twice
		if (ComboIndex >= Attacks.Num())
		{
			ComboIndex = 0;
			CombatComponent->SetComboIndex(1);
		}
		else
		{
			CombatComponent->SetComboIndex(0);
		}
	}
	else
	{
		CombatComponent->SetComboIndex(ComboIndex + 1);
	}

	// UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Combo Index: {3}, Combat Component's: {4}",
	// 	UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), ComboIndex, CombatComponent->GetComboIndex());
}


void UMeleeCombatAbility::SetComboAttack_Implementation()
{
	SecondaryHitActors.Empty();
	PrimaryHitActors.Empty();
	
	// Retrieve the current attack
	const TArray<F_ComboAttack>& Attacks = bRunningAttack ? RunningAttackInformation.ComboAttacks : bCrouchingAttack ? CrouchingAttackInformation.ComboAttacks : ComboAttacks.ComboAttacks;
	if (Attacks.IsValidIndex(ComboIndex))
	{
		CurrentAttack = Attacks[ComboIndex];
	}
	else
	{
		CurrentAttack = Attacks.Num() > 0 ? Attacks[0] : F_ComboAttack();
	}
	
	// Retrieve the current attack
	// const TArray<F_ComboAttack>& Attacks = bRunningAttack ? RunningAttackInformation.ComboAttacks : CrouchingAttackInformation.ComboAttacks; 
	// if (ComboCount <= 1 || ComboIndex + 1 >= Attacks.Num())
	// {
	// 	if (!Attacks.IsEmpty())
	// 	{
	// 		CurrentAttack = Attacks[0];
	// 	}
	// }

	

	// custom attack information for easily handling different combat logic
	if (bUseTestCombatInformation)
	{
		CurrentAttack = TestAttackInformation;
	}
}


void UMeleeCombatAbility::SetAttackMontage_Implementation(AArmament* Weapon)
{
	if (!bRunningAttack && !bCrouchingAttack)
	{
		Super::SetAttackMontage_Implementation(Weapon);
		return;
	}
	
	if (!Weapon)
	{
		UCombatComponent* CombatComponent = GetCombatComponent();
		if (!CombatComponent)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component during {2} while retrieving the montage",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
			return;
		}
	
		Weapon = CombatComponent->GetArmament(IsRightHandAbility());
		if (!Weapon)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the armament during {2} while retrieving the montage",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
			return;
		}
	}

	if (bCrouchingAttack)
	{
		SetCurrentMontage(Weapon->GetCombatMontage(EInputAbilities::Crouch));
	}
	if (bRunningAttack)
	{
		SetCurrentMontage(Weapon->GetCombatMontage(EInputAbilities::Sprint));
	}
}


void UMeleeCombatAbility::SetMontageStartSection_Implementation(bool ChargeAttack)
{
	MontageStartSection = DefaultMontageSection;
	
	// Default attacks
	if (bRunningAttack)
	{
		MontageStartSection = Montage_Section_RunningAttack;
		return;
	}
	
	if (bCrouchingAttack)
	{
		MontageStartSection = Montage_Section_CrouchAttack;
		return;
	}
	
	if (bComboAbility)
	{
		if (ComboIndex < Montage_ComboSections.Num()) MontageStartSection = Montage_ComboSections[ComboIndex];
		else MontageStartSection = Montage_ComboSections[0];
		
		// Charged attacks
		if (ChargeAttack)
		{
			MontageStartSection = FName(MontageStartSection.ToString() + Montage_Section_Charge);
		}
	}
}


void UMeleeCombatAbility::CalculateAttributeModifications_Implementation()
{
	// Don't try to add attribute calculations if the weapon isn't valid 
	if (!Armament) return;

	// Empty out the previous attack's attribute calculations
	AttackInfo.Empty();
	
	// Weapon damage and attribute calculations
	const F_ArmamentInformation& ArmamentInformation = Armament->GetArmamentInformation();
	// const TMap<FGameplayAttribute, float>& DamageMultipliers = CurrentAttack.AttackInformation.BaseDamagesOrMultipliers;
	if (ArmamentInformation.DamageCalculations == EDamageInformationSource::Armament)
	{
		// Armament/Skill based -> retrieve the armament's base damage, add damage scaling, and the current attack multiplier
		const TMap<FGameplayAttribute, float>& WeaponStats = bUseTestCombatInformation ? TestDamageStats : ArmamentInformation.BaseDamageStats;
		for (auto &[Attribute, Value] : WeaponStats)
		{
			// Damages
			AttackInfo.Add(UMMOAttributeSet::GetDamage_PoiseAttribute(), CurrentAttack.PoiseDamage);
			
			if (Attribute == UMMOAttributeSet::GetDamage_StandardAttribute() ||
				Attribute == UMMOAttributeSet::GetDamage_SlashAttribute() ||
				Attribute == UMMOAttributeSet::GetDamage_PierceAttribute() ||
				Attribute == UMMOAttributeSet::GetDamage_StrikeAttribute())
			{
				float EquipmentMultiplier = 1;
				float AttackMotionValue = CurrentAttack.MotionValue * 0.01;
				AttackInfo.Add(Attribute, (Value * EquipmentMultiplier) * AttackMotionValue);
			}

			if (Attribute == UMMOAttributeSet::GetDamage_MagicAttribute() ||
				Attribute == UMMOAttributeSet::GetDamage_IceAttribute() ||
				Attribute == UMMOAttributeSet::GetDamage_FireAttribute() ||
				Attribute == UMMOAttributeSet::GetDamage_HolyAttribute() ||
				Attribute == UMMOAttributeSet::GetDamage_LightningAttribute())
			{
				float EquipmentMultiplier = 1;
				float AttackMotionValue = CurrentAttack.MotionValue * 0.01;
				AttackInfo.Add(Attribute, (Value * EquipmentMultiplier) * AttackMotionValue);
			}
			
			// Statuses
			if (Attribute == UMMOAttributeSet::GetCurseAttribute() ||
				Attribute == UMMOAttributeSet::GetBleedAttribute() ||
				Attribute == UMMOAttributeSet::GetFrostbiteAttribute() ||
				Attribute == UMMOAttributeSet::GetPoisonAttribute() ||
				Attribute == UMMOAttributeSet::GetMadnessAttribute() ||
				Attribute == UMMOAttributeSet::GetSleepAttribute())
			{
				float EquipmentMultiplier = 1;
				float AttackMotionValue = CurrentAttack.StatusMotionValue * 0.01;
				AttackInfo.Add(Attribute, (Value * EquipmentMultiplier) * AttackMotionValue);
			}

			// Add any custom attribute information here
			
			
		}
	}
	else if (ArmamentInformation.DamageCalculations == EDamageInformationSource::Combo)
	{
		// Combo based -> retrieve the damage from the combo attack, and add damage scaling from attributes
		AttackInfo.Add(UMMOAttributeSet::GetDamage_StandardAttribute(), CurrentAttack.MotionValue);
	}

	// TODO: Add a function from the combat component that handles attribute adjustments based on the weapon stats, current attack, and armament stance
	
}
#pragma endregion




#pragma region Combat Logic
void UMeleeCombatAbility::OnAttackFrameEvent_Implementation(FGameplayEventData EventData)
{
	/**
		One hand / Two hand
			- Attack frames
				- Begin attack frames
				- End attack frames

		At the beginning of the attack we're just searching for frames
		The overlap component mediates attacks without notifying the weapon
		The weapon checks for traces and sends the data to the server before it gets deleted
		Once it's deleted, the replicated data is sent to the server regardless of the current combat logic
		This data is used for combat for the entirety of the attack



			Dual wield attack
				- Attack frames
					- Left hand
						- Begin attack frames
						- End attack frames
					- Right hand
						- Begin attack frames
						- End attack frames

		At the beginning of the attack we handle both weapons, and activate when we find attack frames for one of the weapons
		All the values persist, and we do traces specific to each weapon using only one overlap component (There's a limit to these, and it's okay to be inefficient because this is more fun)
		The weapon checks for traces, calculates combat logic, and sends it to each armament to handle individual attack calculations specific to the trace frame

	*/
		

	if (CurrentStance == EArmamentStance::OneHanding || CurrentStance == EArmamentStance::TwoHanding)
	{
		if (EventData.EventTag.MatchesTag(AttackFramesEndTag))
		{
			OnEndAttackFrames(IsRightHandAbility());
		}
		else if (EventData.EventTag.MatchesTag(AttackFramesBeginTag))
		{
			OnBeginAttackFrames(IsRightHandAbility());
		}
	}
	// Two weapons that aren't dual wielding don't normally attack together, however combat is much more intricate than that, and it's easy to prioritize the logic properly
	else
	{
		
		// End attack frames (war is never over)
		if (EventData.EventTag.MatchesTag(AttackFramesEndTag))
		{
			if (bDebug) UE_LOGFMT(AbilityLog, Log, "{0}::AttackFrames End, {1}", GetOwningActorFromActorInfo()->HasAuthority() ? FString("Server") : FString("Client"), *EventData.EventTag.ToString());
			
			if (EventData.EventTag.MatchesTag(LeftHandAttackFramesEndTag) && SecondaryAttackState == EAttackFramesState::Enabled)
			{
				OnEndAttackFrames(false);
			}
			else if (EventData.EventTag.MatchesTag(RightHandAttackFramesEndTag) && PrimaryAttackState == EAttackFramesState::Enabled)
			{
				OnEndAttackFrames(true);
			}
			// This shouldn't happen, however some auxiliary attacks (like crouching and other ones non specific to attack patterns) could cause it to happen
			else if (EventData.EventTag.MatchesTagExact(AttackFramesEndTag))
			{
				UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Something broke with the attack frame logic! {2}, Stance: {3}, AtkP: {4}, Montage: {5}, EventTag: {6}",
					*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()),
					*UEnum::GetValueAsString(CurrentStance), *UEnum::GetValueAsString(AttackPattern), *GetNameSafe(GetCurrentMontage()), *EventData.EventTag.ToString()
				);
				
				OnEndAttackFrames(IsRightHandAbility());
			}
		}
		// Begin attack frames (I'm always ready)
		else if (EventData.EventTag.MatchesTag(AttackFramesBeginTag))
		{
			if (bDebug) UE_LOGFMT(AbilityLog, Log, "{0}::AttackFrames Begin, {1}", GetOwningActorFromActorInfo()->HasAuthority() ? FString("Server") : FString("Client"), *EventData.EventTag.ToString());
			
			// We're only enabling the frames for one attack, if you have animations with multiple frames you'll have to adjust this logic, specifically for enemies
			if (EventData.EventTag.MatchesTag(LeftHandAttackFramesBeginTag) && SecondaryAttackState == EAttackFramesState::Disabled)
			{
				OnBeginAttackFrames(false);
			}
			else if (EventData.EventTag.MatchesTag(RightHandAttackFramesBeginTag) && PrimaryAttackState == EAttackFramesState::Disabled)
			{
				OnBeginAttackFrames(true);
			}
			// This shouldn't happen, however some auxiliary attacks (like crouching and other ones non specific to attack patterns) could cause it to happen
			else if (EventData.EventTag.MatchesTagExact(AttackFramesBeginTag))
			{
				UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Something broke with the attack frame logic! {2}, Stance: {3}, AtkP: {4}, Montage: {5}, EventTag: {6}",
					*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()),
					*UEnum::GetValueAsString(CurrentStance), *UEnum::GetValueAsString(AttackPattern), *GetNameSafe(GetCurrentMontage()), *EventData.EventTag.ToString()
				);

				OnBeginAttackFrames(IsRightHandAbility());
			}
		}
	}
}


void UMeleeCombatAbility::OnBeginAttackFrames_Implementation(bool bRightHand)
{
	// Update the attack frames state (and capture the previous attack frame state to properly capture frame one calculations
	// const bool bOverlapPreviouslyDisabled = PrimaryAttackState == EAttackFramesState::Disabled && SecondaryAttackState == EAttackFramesState::Disabled;
	if (!bRightHand) SecondaryAttackState = EAttackFramesState::Enabled;
	else PrimaryAttackState = EAttackFramesState::Enabled;
	
	
	// Initialize the attack frames if it hasn't been activated yet
	// if (bOverlapPreviouslyDisabled)
	// {
	// 	if (MeleeOverlapHandle && !MeleeOverlapHandle->IsActive())
	// 	{
	// 		MeleeOverlapHandle->ReadyForActivation();
	// 	}
	// }


	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component to check for already traced enemies the beginning of the attack frames!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return;
	}
	
	if (!bRightHand)
	{
		CheckAndAttackIfAlreadyOverlappingAnything(CombatComponent->GetArmament(false), SecondaryHitActors, ComboAttacks.DamageEffectClass);
	}
	else
	{
		CheckAndAttackIfAlreadyOverlappingAnything(CombatComponent->GetArmament(), PrimaryHitActors, ComboAttacks.DamageEffectClass);
	}
}


void UMeleeCombatAbility::OnEndAttackFrames_Implementation(bool bRightHand)
{
	if (!bRightHand) SecondaryAttackState = EAttackFramesState::Finished;
	else PrimaryAttackState = EAttackFramesState::Finished;

	// If the attack frames have finished for the weapons just end the overlap task
	// if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons)
	// {
	// 	if (MeleeOverlapHandle && PrimaryAttackState == EAttackFramesState::Finished && SecondaryAttackState == EAttackFramesState::Finished)
	// 	{
	// 		MeleeOverlapHandle->EndTask();
	// 	}
	// }
	// else if (MeleeOverlapHandle)
	// {
	// 	MeleeOverlapHandle->EndTask();
	// }
}


void UMeleeCombatAbility::OnOverlappedTarget_Implementation(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc)
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while overlapping a target! Defaulting to one of the armament's hitboxes!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
	}

	if (bDebug)
	{
		UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} OnOverlapped event, checking if they've already attacked the player!",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(OverlappedArmament));
	}

	// Ovlerap logic
	const bool bRightHandArmament = CombatComponent && OverlappedArmament == CombatComponent->GetArmament(false) ? false : true;
	TArray<AActor*>& ArmamentHitActors = !bRightHandArmament ? SecondaryHitActors : PrimaryHitActors;
	if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons)
	{
		// Check if there's attack frames for the current attack
		const EAttackFramesState& AttackFramesState = bRightHandArmament ? PrimaryAttackState : SecondaryAttackState;
		if ((!bRightHandArmament && AttackFramesState == EAttackFramesState::Enabled) ||
			(bRightHandArmament && AttackFramesState == EAttackFramesState::Enabled))
		{
			if (ArmamentHitActors.Contains(TargetAsc->GetAvatarActor()))
			{
				if (bDebug)
				{
					UE_LOGFMT(AbilityLog, Log, "{0} already overlapping with {1}", *GetNameSafe(OverlappedArmament), *GetNameSafe(TargetAsc->GetAvatarActor()));
				}
				
				return;
			}

			// Handle the attack logic
			if (TargetAsc->GetAvatarActor())
			{
				ArmamentHitActors.AddUnique(TargetAsc->GetAvatarActor());
				HandleMeleeAttack(TargetData, OverlappedArmament, TargetAsc, ComboAttacks.DamageEffectClass);
			}
		}
		else if (bDebug)
		{
			UE_LOGFMT(AbilityLog, Log, "{0} invalid attack frame state: {1}, rightHandArmament({2}): {3}", *GetNameSafe(OverlappedArmament),
				*UEnum::GetValueAsString(AttackFramesState), bRightHandArmament ? *FString("true") : *FString("false"), GetNameSafe(CombatComponent->GetArmament(bRightHandArmament)));
		}
	}
	else
	{
		// Don't attack twice, just continue // TODO: this should be handled on the overlap task to prevent extra replication
		if (ArmamentHitActors.Contains(TargetAsc->GetAvatarActor()))
		{
			return;
		}

		// Handle the attack logic
		if (TargetAsc->GetAvatarActor())
		{
			ArmamentHitActors.AddUnique(TargetAsc->GetAvatarActor());
			HandleMeleeAttack(TargetData, OverlappedArmament, TargetAsc, ComboAttacks.DamageEffectClass);
		}
	}
}
#pragma endregion 




#pragma region Utility
bool UMeleeCombatAbility::IsValidForCrouchAttack_Implementation() const
{
	if (!bUseCrouchingAttacks)
	{
		return false;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	UAdvancedMovementComponent* MovementComponent = Character ? Character->GetMovementComp<UAdvancedMovementComponent>() : nullptr;
	
	// Check if it's a crouching attack
	return MovementComponent && MovementComponent->IsCrouching();
}


bool UMeleeCombatAbility::IsValidForRunningAttack_Implementation() const
{
	if (!bUseRunningAttacks)
	{
		return false;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	UAdvancedMovementComponent* MovementComponent = Character ? Character->GetMovementComp<UAdvancedMovementComponent>() : nullptr;
	if (!MovementComponent)
	{
		return false;
	}

	float MoveSpeed = MovementComponent->GetLastUpdateVelocity().Length();
	float SprintSpeed = MovementComponent->GetMaxWalkSpeed() * MovementComponent->SprintSpeedMultiplier;
	float SprintAttackSpeed = FMath::Clamp(MoveSpeed * MovementComponent->SprintSpeedMultiplier * 0.64, MovementComponent->GetMaxWalkSpeed(), SprintSpeed);
	
	// Check if it's a running attack
	return MovementComponent->IsWalking() && MovementComponent->GetLastUpdateVelocity().Length() >= SprintAttackSpeed;
}


bool UMeleeCombatAbility::DetermineIfItsTheFinalAttack_Implementation() const
{
	if (!bComboAbility) return true;
	return ComboAttacks.ComboAttacks.Num() > 1 && ComboIndex + 1 == ComboAttacks.ComboAttacks.Num() ? true : false;
}


int32 UMeleeCombatAbility::GetNumComboCount_Implementation() const
{
	const int32 MontageSections = GetNumMontageSections();
	return ComboAttacks.ComboAttacks.Num() < MontageSections ? ComboAttacks.ComboAttacks.Num() : MontageSections;
}


int32 UMeleeCombatAbility::GetNumMontageSections_Implementation() const
{
	if (GetCurrentMontage() == nullptr) return 1;
	return GetCurrentMontage()->CompositeSections.Num();
}
#pragma endregion 
