// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"

#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"

#include "Sandbox/Data/Enums/ArmamentTypes.h"
#include "Sandbox/Data/Enums/EquipSlot.h"

UCombatAbility::UCombatAbility()
{
	// Combo attacks should be activated on every actor, and the information needs to be saved between attacks
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}


void UCombatAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component during {2}'s initialization while binding to unequipped events!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return;
	}

	CombatComponent->OnUnequippedArmament.AddDynamic(this, &UCombatAbility::OnUnequipArmament);
}


void UCombatAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Abilities that need to be reset during activation
	AlreadyHitActors.Empty();
	bCancelledToEquipArmament = false;
	bIsFinalComboAttack = false;
}


void UCombatAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}




#pragma region Initialization
bool UCombatAbility::SetComboAndArmamentInformation()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component during {2} while getting combo and armament information",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return false;
	}
	
	AArmament* EquippedArmament = CombatComponent->GetArmament(IsRightHandAbility());
	if (!EquippedArmament || EquippedArmament->GetEquipStatus() != EEquipStatus::Equipped)
	{
		if (!EquippedArmament)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the armament during {2} while getting combo and armament information",
				UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		}
		return false;
	}

	// Retrieve the attack pattern from the actual input
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	if (Spec)
	{
		AttackPattern = static_cast<EInputAbilities>(Spec->InputID);
		CurrentStance = CombatComponent->GetCurrentStance();
	}
	
	// Retrieve the combo information if the player's equipped an armament or their stance has updated
	if (EquippedArmament == Armament && CurrentStance == CombatComponent->GetCurrentStance()) return true;
	else
	{
		Armament = nullptr;
		ComboAttacks = F_ComboAttacks();
		ComboCount = 0;
		CurrentAttack = F_ComboAttack();
		EquipSlot = EEquipSlot::None;
	}
	
	// Retrieve the attacks for the current stance
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

		ensure(!ComboAttacks.ComboAttacks.IsEmpty());
		return true;
	}

	return false;
}


void UCombatAbility::InitCombatInformation()
{
	SetComboAttack(); // Current attack and combo index
	SetAttackMontage(Armament); // Current montage
	SetMontageStartSection(); // montage start section (customization for different types of attacks
	CalculateAttributeModifications(); // Damage and attribute calculations
}


void UCombatAbility::SetComboAttack()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1} Failed to retrieve the combat component during {2} while getting the combo attack",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return;
	}
	
	// Retrieve the combo index and the current attack
	if (ComboCount <= 1 || CombatComponent->GetComboIndex() + 1 >= ComboAttacks.ComboAttacks.Num())
	{
		ComboIndex = 0;
		if (!ComboAttacks.ComboAttacks.IsEmpty())
		{
			CurrentAttack = ComboAttacks.ComboAttacks[0];
		}
	}
	else
	{
		ComboIndex = CombatComponent->GetComboIndex() + 1;
		CurrentAttack = ComboAttacks.ComboAttacks[ComboIndex];
	}
	
	CombatComponent->SetComboIndex(ComboIndex);
}


void UCombatAbility::SetAttackMontage(AArmament* Weapon)
{
	if (!Weapon) return;
	SetCurrentMontage(Weapon->GetCombatMontage(AttackPattern));
}


void UCombatAbility::SetMontageStartSection(bool ChargeAttack)
{
	MontageStartSection = DefaultMontageSection;
	
	// Default attacks
	if (bComboAbility)
	{
		if (ComboIndex < Montage_ComboSections.Num()) MontageStartSection = Montage_ComboSections[ComboIndex];
		else MontageStartSection = Montage_ComboSections[0];
	}

	// Charged attacks
	if (ChargeAttack)
	{
		MontageStartSection = FName(MontageStartSection.ToString() + Montage_Section_Charge);
	}
}


void UCombatAbility::CalculateAttributeModifications()
{
	// Don't try to add attribute calculations if the weapon isn't valid 
	if (!Armament) return;

	// Empty out the previous attack's attribute calculations
	AdjustedAttributes.Empty();
	
	// Weapon damage and attribute calculations
	const F_ArmamentInformation& ArmamentInformation = Armament->GetArmamentInformation();
	const TMap<FGameplayAttribute, float>& DamageMultipliers = CurrentAttack.AttackInformation.BaseDamagesOrMultipliers;
	if (ArmamentInformation.DamageCalculations == EDamageInformationSource::Armament)
	{
		// Armament/Skill based -> retrieve the armament's base damage, add damage scaling, and the current attack multiplier
		for (auto &[Attribute, Value] : ArmamentInformation.BaseDamageStats)
		{
			const float CurrentAttackMultiplier = DamageMultipliers.Contains(Attribute) ? DamageMultipliers[Attribute] : 1;
			AdjustedAttributes.Add(Attribute, Value * CurrentAttackMultiplier);
		}
	}
	else if (ArmamentInformation.DamageCalculations == EDamageInformationSource::Combo)
	{
		// Combo based -> retrieve the damage from the combo attack, and add damage scaling from attributes
		AdjustedAttributes = CurrentAttack.AttackInformation.BaseDamagesOrMultipliers;
	}

	// TODO: Add a function from the combat component that handles attribute adjustments based on the weapon stats, current attack, and armament stance
	
}
#pragma endregion 




#pragma region Combat functions
void UCombatAbility::HandleMeleeAttack(const FGameplayAbilityTargetDataHandle& TargetData, UAbilitySystem* TargetAsc)
{
	// Validity Checks
	if (!TargetData.IsValid(0))
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s Target data is not valid! ",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}
	
	if (!Armament || !ComboAttacks.DamageEffectClass)
	{
		if (!Armament)
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s is missing it's armament!",
				*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		}
		else
		{
			UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s {3} does not have a execution calculation!",
				*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetNameSafe(Armament));
		}
		return;
	}

	AActor* TargetCharacter = TargetAsc->GetAvatarActor();
	if (!TargetCharacter)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} failed to retrieve it's avatar actor!",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}
	
	// UE_LOGFMT(LogTemp, Log, "{0} {1}() {2} Landed an attack on {3}", *UEnum::GetValueAsString(BaseCharacter->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(BaseCharacter), *GetNameSafe(TargetCharacter));
	// Prep and send an exec calc to the target
	const FGameplayEffectSpecHandle ExecCalcHandle = PrepExecCalcForTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		ComboAttacks.DamageEffectClass
	);
	
	if (!ExecCalcHandle.IsValid())
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s execution calculation handle for an attack was not valid!",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}

	// This is just to have the context handle information also (for the client) passed to other events like the character blocking, parrying, etc.
	const FGameplayEffectSpec* ExecCalc = ExecCalcHandle.Data.Get();
	FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle();
	if (ExecCalc) ContextHandle = ExecCalc->GetContext();

	// Create the execution calculation and add the attack information to the context handle
	// const float CalculatedDamage = GetCalculatedDamage();
	// FHitResult Impact = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetData, 0);
	// Impact.ImpactPoint = Armament->GetActorLocation();
	// Impact.ImpactNormal = Impact.ImpactPoint.GetSafeNormal();
	// const EHitReact HitReactDirection = UCharacterBlueprintLibrary::GetHitReactDirection(TargetCharacter, TargetCharacter->GetActorLocation(), Impact.ImpactPoint);

	// TODO: Some of this was just to learn some things about how calculations are handled and aren't necessary 
	// ContextHandle.AddHitResult(Impact);
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(ExecCalcHandle, FGameplayTag::RequestGameplayTag(Tag_Data_IncomingDamage_Physical), CalculatedDamage);
	// USanboxAscLibrary::SetPoiseDamage(ContextHandle, AttackInformation.PoiseDamage);
	// USanboxAscLibrary::SetKnockbackType(ContextHandle, AttackInformation.RecoilEffect);
	// USanboxAscLibrary::SetHitReactDirection(ContextHandle, HitReactDirection);
	// USanboxAscLibrary::SetKnockbackForce(ContextHandle, FVector(13.0f)); // TODO: add combat calculations for knock back amounts for different attacks?

	// Only calculate on the authority
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &ActivationInfo))
	{
		TArray<FActiveGameplayEffectHandle> EffectHandles = ApplyExecCalcToTarget(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			TargetData,
			ExecCalcHandle
		);
	}
}


void UCombatAbility::CheckAndAttackIfAlreadyOverlappingAnything(TArray<AActor*>& Actors)
{
	// Check if there are any targets that are already overlapping that haven't been attacked during the attack frames yet
	TArray<AActor*> TargetActors;
	for (const UPrimitiveComponent* Hitbox : Armament->GetArmamentHitboxes())
	{
		Hitbox->GetOverlappingActors(TargetActors);
	}

	// Frame one calculations sometimes occur after the weapon is already overlapping with an enemy
	for (AActor* TargetActor : TargetActors)
	{
		ACharacterBase* TargetCharacter = Cast<ACharacterBase>(TargetActor);
		if (!TargetCharacter || TargetActor == GetAvatarActorFromActorInfo()) continue;
			
		if (!Actors.Contains(TargetCharacter) && TargetCharacter->GetAbilitySystemComponent())
		{
			FGameplayAbilityTargetDataHandle TargetData = FGameplayAbilityTargetDataHandle();
			FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
			Data->HitResult = FHitResult(TargetActor, nullptr, TargetActor->GetActorLocation(), TargetActor->GetActorLocation().GetSafeNormal());
				
			TArray<TWeakObjectPtr<AActor>> Targets;
			Targets.Add(TargetActor);
			Data->SetActors(Targets);
			TargetData.Add(Data);
	
			HandleMeleeAttack(TargetData, TargetCharacter->GetAbilitySystem<UAbilitySystem>());
		}
	}
}
#pragma endregion 





#pragma region Utility
bool UCombatAbility::DetermineIfItsTheFinalAttack(FName MontageSection, int32 Combos) const
{
	if (!bComboAbility) return true;
	return ComboAttacks.ComboAttacks.Num() > 1 && ComboIndex + 1 == ComboAttacks.ComboAttacks.Num() ? true : false;
}


int32 UCombatAbility::GetNumComboCount() const
{
	const int32 MontageSections = GetNumMontageSections();
	return ComboAttacks.ComboAttacks.Num() < MontageSections ? ComboAttacks.ComboAttacks.Num() : MontageSections;
}


int32 UCombatAbility::GetNumMontageSections() const
{
	if (GetCurrentMontage() == nullptr) return 1;
	return GetCurrentMontage()->CompositeSections.Num();
}


TArray<AActor*>& UCombatAbility::GetAlreadyHitActors()
{
	return AlreadyHitActors;
}


bool UCombatAbility::IsRightHandAbility() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return IsRightHandAbilityInput(static_cast<EInputAbilities>(Spec->InputID));
}


bool UCombatAbility::IsRightHandAbilityInput(const EInputAbilities AbilityInput) const
{
	return AbilityInput != EInputAbilities::SecondaryAttack;
}


void UCombatAbility::SetArmament(AArmament* NewArmament)
{
	Armament = NewArmament;
	EquipSlot = Armament ? Armament->GetEquipSlot() : EEquipSlot::None;
}


void UCombatAbility::OnUnequipArmament(FName ArmamentName, FGuid Id, EEquipSlot Slot)
{
	if (Slot == EquipSlot)
	{
		Armament = nullptr;
		EquipSlot = EEquipSlot::None;
		AttackPattern = EInputAbilities::None;
	}
}


UCombatComponent* UCombatAbility::GetCombatComponent() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the character while retrieving the combat component!!",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return nullptr;
	}
	
	return Character->GetCombatComponent();
}
#pragma endregion 

