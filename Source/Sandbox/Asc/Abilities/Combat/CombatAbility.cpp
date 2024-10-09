// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Asc/Information/SandboxTags.h"
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

	ACharacterBase* Character = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		// UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the character while retrieving the combat component!!",
		// 	UEnum::GetValueAsString(ActorInfo->OwnerActor.Get()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(ActorInfo->OwnerActor.Get()));
		return;
	}
	
	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component during {2}'s initialization while binding to unequipped events!",
			UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character));
		return;
	}

	CombatComponent->OnUnequippedArmament.AddDynamic(this, &UCombatAbility::OnUnequipArmament);
}


void UCombatAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Abilities that need to be reset during activation
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
	if (EquippedArmament == Armament && CurrentStance == CombatComponent->GetCurrentStance() && EquipSlot == EquippedArmament->GetEquipSlot()) return true;
	else
	{
		Armament = nullptr;
		ComboAttacks = F_ComboAttacks();
		CurrentAttack = F_ComboAttack();
		ComboCount = 0;
		SetCurrentMontage(nullptr);
		EquipSlot = EEquipSlot::None;
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

	ensure(!ComboAttacks.ComboAttacks.IsEmpty());
	return false;
}


void UCombatAbility::InitCombatInformation()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while getting the combo attack",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
	}
	
	ComboIndex = CombatComponent ? CombatComponent->GetComboIndex() : 0;
	SetComboIndex();
	SetComboAttack(); // Current attack and combo index
	SetAttackMontage(Armament); // Current montage
	SetMontageStartSection(); // montage start section (customization for different types of attacks
	CalculateAttributeModifications(); // Damage and attribute calculations
}


void UCombatAbility::SetComboAttack()
{
	// Retrieve the current attack
	if (ComboCount <= 1 || ComboIndex + 1 >= ComboAttacks.ComboAttacks.Num())
	{
		if (!ComboAttacks.ComboAttacks.IsEmpty())
		{
			CurrentAttack = ComboAttacks.ComboAttacks[0];
			UCombatComponent* CombatComponent = GetCombatComponent();
			if (CombatComponent) CombatComponent->SetComboIndex(0);
		}
	}
	else
	{
		if (ComboAttacks.ComboAttacks.IsValidIndex(ComboIndex))
		{
			CurrentAttack = ComboAttacks.ComboAttacks[ComboIndex];
		}
		else
		{
			CurrentAttack = ComboAttacks.ComboAttacks.Num() > 0 ? ComboAttacks.ComboAttacks[0] : F_ComboAttack();
		}
	}
}


void UCombatAbility::SetComboIndex()
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while adjusting the combo index",
			UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetName());
		return;
	}

	// Either have separate combo indexing for normal / strong attacks, or when transitioning from one to the other have that finish the combo. Having both combined would be interesting I just think it'd be tough to balance
	if (ComboIndex + 1 >= ComboAttacks.ComboAttacks.Num())
	{
		CombatComponent->SetComboIndex(0);
	}
	else
	{
		CombatComponent->SetComboIndex(ComboIndex + 1);
	}
	
	// UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Adjusted combo index from {3} to {4}",
	// 	UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), ComboIndex, CombatComponent->GetComboIndex());
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
	AttackInfo.Empty();
	
	// Weapon damage and attribute calculations
	const F_ArmamentInformation& ArmamentInformation = Armament->GetArmamentInformation();
	// const TMap<FGameplayAttribute, float>& DamageMultipliers = CurrentAttack.AttackInformation.BaseDamagesOrMultipliers;
	if (ArmamentInformation.DamageCalculations == EDamageInformationSource::Armament)
	{
		// Armament/Skill based -> retrieve the armament's base damage, add damage scaling, and the current attack multiplier
		for (auto &[Attribute, Value] : ArmamentInformation.BaseDamageStats)
		{
			// Damages
			AttackInfo.Add(UMMOAttributeSet::GetPoiseAttribute(), CurrentAttack.PoiseDamage);
			
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
				float AttackMotionValue = CurrentAttack.MotionValue * 0.01;;
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
				float AttackMotionValue = CurrentAttack.StatusMotionValue * 0.01;;
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




#pragma region Combat functions
void UCombatAbility::HandleMeleeAttack(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc)
{

	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (!HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &ActivationInfo))
	{
		// UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s handle melee attack({3}) is only valid on the server! ",
		// 	*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetNameSafe(OverlappedArmament));
		return;
	}
	
	// Validity Checks
	if (!TargetData.IsValid(0)) // TODO: This is happening sometimes and it's either the client not attacking another client, or an addition trace when prediction isn't valid. Fix traces to always have valid prediction
	{
		// UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s Target data is not valid! ",
		// 	*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the character while handling a melee attack",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}

	UAbilitySystem* AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Failed to retrieve the charcter's ability system while handling a melee attack",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}
	
	const UMMOAttributeSet* AttributeSet = Cast<UMMOAttributeSet>(AbilitySystem->GetAttributeSet(UMMOAttributeSet::StaticClass()));
	if (!AttributeSet)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() Something happened to the ({2})'s attribute set while trying to handle a melee attack",
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
	
	// UE_LOGFMT(AbilityLog, Log, "{0}::{1}() {2} Landed an attack on {3}",
	// 	*UEnum::GetValueAsString(GetAvatarActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetAvatarActorFromActorInfo()), *GetNameSafe(TargetCharacter));

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

	// There are a few ways to send data to an ExecutionCalculation in addition to capturing Attributes.
	// Any SetByCallers set on the GameplayEffectSpec can be directly read in the ExecutionCalculation, CalculationModifiers let you add static values
	// Custom gameplay effect information. Modifying gameplay effect spec is valid, but dangerous when retrieving the owning spec for pre execute -> /* Non const access. Be careful with this, especially when modifying a spec after attribute capture. */
	// ValidTransientAggregatorIdentifiers -> The ExecutionCalculation reads this value in using special capture functions similar to the Attribute capture functions. (ExecutionParams.AttemptCalculateTransientAggregatorMagnitude())
	
	// We're just going to use attributes, valid client side prediction attack calculations, and if we need any other information let's just use the custom gameplay effect
	UMMOAttributeSet* Attributes = const_cast<UMMOAttributeSet*>(AttributeSet);
	if (Attributes)
	{
		if (AttackInfo.Contains(Attributes->GetDamage_StandardAttribute())) Attributes->SetDamage_Standard(AttackInfo[Attributes->GetDamage_StandardAttribute()]);
		if (AttackInfo.Contains(Attributes->GetDamage_SlashAttribute())) Attributes->SetDamage_Slash(AttackInfo[Attributes->GetDamage_SlashAttribute()]);
		if (AttackInfo.Contains(Attributes->GetDamage_PierceAttribute())) Attributes->SetDamage_Pierce(AttackInfo[Attributes->GetDamage_PierceAttribute()]);
		if (AttackInfo.Contains(Attributes->GetDamage_StrikeAttribute())) Attributes->SetDamage_Strike(AttackInfo[Attributes->GetDamage_StrikeAttribute()]);

		if (AttackInfo.Contains(Attributes->GetDamage_MagicAttribute())) Attributes->SetDamage_Magic(AttackInfo[Attributes->GetDamage_MagicAttribute()]);
		if (AttackInfo.Contains(Attributes->GetDamage_IceAttribute())) Attributes->SetDamage_Ice(AttackInfo[Attributes->GetDamage_IceAttribute()]);
		if (AttackInfo.Contains(Attributes->GetDamage_FireAttribute())) Attributes->SetDamage_Fire(AttackInfo[Attributes->GetDamage_FireAttribute()]);
		if (AttackInfo.Contains(Attributes->GetDamage_HolyAttribute())) Attributes->SetDamage_Holy(AttackInfo[Attributes->GetDamage_HolyAttribute()]);
		if (AttackInfo.Contains(Attributes->GetDamage_LightningAttribute())) Attributes->SetDamage_Lightning(AttackInfo[Attributes->GetDamage_LightningAttribute()]);
		
		if (AttackInfo.Contains(Attributes->GetBleedAttribute())) Attributes->SetBleed(AttackInfo[Attributes->GetBleedAttribute()]);
		if (AttackInfo.Contains(Attributes->GetPoisonAttribute())) Attributes->SetPoison(AttackInfo[Attributes->GetPoisonAttribute()]);
		if (AttackInfo.Contains(Attributes->GetFrostbiteAttribute())) Attributes->SetFrostbite(AttackInfo[Attributes->GetFrostbiteAttribute()]);
		if (AttackInfo.Contains(Attributes->GetCurseAttribute())) Attributes->SetCurse(AttackInfo[Attributes->GetCurseAttribute()]);
		if (AttackInfo.Contains(Attributes->GetMadnessAttribute())) Attributes->SetMadness(AttackInfo[Attributes->GetMadnessAttribute()]);
		if (AttackInfo.Contains(Attributes->GetSleepAttribute())) Attributes->SetSleep(AttackInfo[Attributes->GetSleepAttribute()]);

	}
	
	// Create the execution calculation and add any additional information to the handle
	const FGameplayEffectSpec* ExecCalc = ExecCalcHandle.Data.Get();

	// Add the weapon to the effect context
	FGameplayEffectContextHandle EffectContext = ExecCalc->GetContext();
	EffectContext.AddSourceObject(OverlappedArmament);

	
	// If we want to handle hit reacts here to prevent any problems with montages, here is the place we should handle it
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

	
	// Retrieve the gameplay effect attributes, and additional information to the specification

	
	// Handle damage calculations
	TArray<FActiveGameplayEffectHandle> EffectHandles = ApplyExecCalcToTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		TargetData,
		ExecCalcHandle
	);
}


void UCombatAbility::CheckAndAttackIfAlreadyOverlappingAnything(AArmament* OverlappedArmament, TArray<AActor*>& AlreadyHitActors)
{
	if (!OverlappedArmament)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} Tried attacking without a valid armament! {3}{4}", *UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()),
			*FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *UEnum::GetValueAsString(CurrentStance), *UEnum::GetValueAsString(AttackPattern));
		return;
	}

	// Check if there are any targets that are already overlapping that haven't been attacked during the attack frames yet
	TArray<AActor*> TargetActors;
	for (const UPrimitiveComponent* Hitbox : OverlappedArmament->GetArmamentHitboxes())
	{
		Hitbox->GetOverlappingActors(TargetActors);
	}

	// Frame one calculations sometimes occur after the weapon is already overlapping with an enemy
	for (AActor* TargetActor : TargetActors)
	{
		ACharacterBase* TargetCharacter = Cast<ACharacterBase>(TargetActor);
		if (!TargetCharacter || TargetActor == GetAvatarActorFromActorInfo()) continue;
			
		if (!AlreadyHitActors.Contains(TargetCharacter) && TargetCharacter->GetAbilitySystemComponent())
		{
			FGameplayAbilityTargetDataHandle TargetData = FGameplayAbilityTargetDataHandle();
			FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
			Data->HitResult = FHitResult(TargetActor, nullptr, TargetActor->GetActorLocation(), TargetActor->GetActorLocation().GetSafeNormal());
				
			TArray<TWeakObjectPtr<AActor>> Targets;
			Targets.Add(TargetActor);
			Data->SetActors(Targets);
			TargetData.Add(Data);
	
			HandleMeleeAttack(TargetData, OverlappedArmament, TargetCharacter->GetAbilitySystem<UAbilitySystem>());
			AlreadyHitActors.Add(TargetCharacter);
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

