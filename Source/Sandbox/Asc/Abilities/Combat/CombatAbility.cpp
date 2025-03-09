// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Abilities/Combat/CombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
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
}


void UCombatAbility::AddStaminaCostEffect(float Stamina)
{
	const FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	
	// This isn't something that's replicated so you'll need a duration once the player's stamina has been drained to prevent them from spamming and causing lag from ability activation discrepancies
	FName StaminaCostEffect = FName(UEnum::GetValueAsString(AttackPattern).Append("_StaminaCost"));
	UGameplayEffect* StaminaCost = NewObject<UGameplayEffect>(ActorInfo->OwnerActor.Get(), StaminaCostEffect);
	if (StaminaCost)
	{
		StaminaCost->DurationPolicy = EGameplayEffectDurationType::Instant;
		FGameplayModifierInfo StaminaDrain = FGameplayModifierInfo();
		StaminaDrain.Attribute = UMMOAttributeSet::GetStaminaAttribute();
		StaminaDrain.ModifierOp = EGameplayModOp::Additive;
		StaminaDrain.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-Stamina));
		StaminaCost->Modifiers.Add(StaminaDrain);
		
		// UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		// check(AbilitySystemComponent != nullptr);
		// if (!AbilitySystemComponent->CanApplyAttributeModifiers(StaminaCost, GetAbilityLevel(Handle, ActorInfo), MakeEffectContext(Handle, ActorInfo)))
		// {
		// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		// 	return;
		// }
		
		FGameplayEffectSpec* StaminaCostSpec = new FGameplayEffectSpec(StaminaCost, MakeEffectContext(Handle, ActorInfo), 1);
		FGameplayEffectSpecHandle StaminaCostHandle = FGameplayEffectSpecHandle(StaminaCostSpec);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, StaminaCostHandle);
	}
}


void UCombatAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}




#pragma region Initialization
bool UCombatAbility::SetArmamentInformation_Implementation()
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
		SetCurrentMontage(nullptr);
		Armament = nullptr;
		EquipSlot = EEquipSlot::None;
	}
	
	// Retrieve the attacks for the current stance. If the armament has a specific attack pattern ability without combat information, notify us to prevent any problems
	const F_ArmamentInformation& ArmamentInformation = EquippedArmament->GetArmamentInformation();
	for (const auto& [Ability, InputId, InvalidStances, Level] : ArmamentInformation.MeleeAbilities)
	{
		if (AttackPattern != InputId || InvalidStances.Contains(CombatComponent->GetCurrentStance())) continue;

		// If there's no combat information for this attack, then either the information is missing or the ability was added at the wrong time
		Armament = EquippedArmament;
		EquipSlot = EquippedArmament->GetEquipSlot();
		SetCurrentMontage(EquippedArmament->GetCombatMontage(AttackPattern));
		return true;
	}

	return false;
}


void UCombatAbility::InitCombatInformation_Implementation()
{
	SetMontageStartSection(); // montage start section (customization for different types of attacks
	CalculateAttributeModifications(); // Damage and attribute calculations
	SetAttackMontage(Armament); // Current montage
}

void UCombatAbility::SetAttackMontage_Implementation(AArmament* Weapon)
{
	if (!Weapon) return;
	SetCurrentMontage(Weapon->GetCombatMontage(AttackPattern));
}


void UCombatAbility::SetMontageStartSection_Implementation(bool ChargeAttack)
{
	MontageStartSection = DefaultMontageSection;
}


void UCombatAbility::CalculateAttributeModifications_Implementation()
{
	// Don't try to add attribute calculations if the weapon isn't valid 
	if (!Armament) return;

	// Empty out the previous attack's attribute calculations
	AttackInfo.Empty();
	
	// Weapon damage and attribute calculations
	const F_ArmamentInformation& ArmamentInformation = Armament->GetArmamentInformation();
	AttackInfo = bUseTestCombatInformation ? TestDamageStats : ArmamentInformation.BaseDamageStats;
}
#pragma endregion 




#pragma region Combat functions
void UCombatAbility::HandleMeleeAttack(const FGameplayAbilityTargetDataHandle& TargetData, AArmament* OverlappedArmament, UAbilitySystem* TargetAsc, TSubclassOf<UGameplayEffect> DamageCalculation)
{

	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (!HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &ActivationInfo))
	{
		// UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s handle melee attack({3}) is only valid on the server! ",
		// 	*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetNameSafe(OverlappedArmament));
		return;
	}
	
	// Validity Checks
	if (!TargetData.IsValid(0)) // TODO: check that the proper client / server instances are accessing and invoking information that they need to invoke
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
	
	AActor* TargetCharacter = TargetAsc->GetAvatarActor();
	if (!TargetCharacter)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2} failed to retrieve it's avatar actor!",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}
	
	if (!DamageCalculation)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s {3} does not have a execution calculation for the current attack!",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()), *GetNameSafe(Armament));
	}
	
	if (!Armament)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}() {2}'s is missing it's armament!",
			*UEnum::GetValueAsString(GetOwningActorFromActorInfo()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwningActorFromActorInfo()));
		return;
	}

	// Prep and send an exec calc to the target
	const FGameplayEffectSpecHandle ExecCalcHandle = PrepExecCalcForTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		DamageCalculation
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
		if (AttackInfo.Contains(Attributes->GetDamage_PoiseAttribute())) Attributes->SetDamage_Poise(AttackInfo[Attributes->GetDamage_PoiseAttribute()]);

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
	
	// TODO: Add custom context information
	// ContextHandle.AddHitResult(Impact);
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(ExecCalcHandle, FGameplayTag::RequestGameplayTag(Tag_Data_IncomingDamage_Physical), CalculatedDamage);
	// USanboxAscLibrary::SetPoiseDamage(ContextHandle, AttackInformation.PoiseDamage);
	// USanboxAscLibrary::SetHitReactDirection(ContextHandle, HitReactDirection);

	
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


void UCombatAbility::CheckAndAttackIfAlreadyOverlappingAnything(AArmament* OverlappedArmament, TArray<AActor*>& AlreadyHitActors, TSubclassOf<UGameplayEffect> DamageCalculation)
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
			FGameplayAbilityTargetData_ActorArray* Data = new FGameplayAbilityTargetData_ActorArray();
			
            FGameplayAbilityTargetingLocationInfo LocationInfo;
            LocationInfo.LiteralTransform.SetLocation(TargetActor->GetActorLocation());
            LocationInfo.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
            LocationInfo.SourceAbility = this;
            LocationInfo.SourceActor = OverlappedArmament;
            Data->SourceLocation = LocationInfo;
            
            TArray<TWeakObjectPtr<AActor>> TargetInformation;
            TargetInformation.Add(OverlappedArmament); // Add the armament that we attacked with
            TargetInformation.Add(TargetActor); // Add the target character
            Data->SetActors(TargetInformation);
            TargetData.Add(Data);
	
			HandleMeleeAttack(TargetData, OverlappedArmament, TargetCharacter->GetAbilitySystem<UAbilitySystem>(), DamageCalculation);
			AlreadyHitActors.Add(TargetCharacter);
		}
	}
}
#pragma endregion 





#pragma region Utility
bool UCombatAbility::IsRightHandAbility_Implementation() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return IsRightHandAbilityInput(static_cast<EInputAbilities>(Spec->InputID));
}


bool UCombatAbility::IsRightHandAbilityInput_Implementation(const EInputAbilities AbilityInput) const
{
	return AbilityInput != EInputAbilities::SecondaryAttack;
}


bool UCombatAbility::IsOutOfStamina_Implementation(UAbilitySystemComponent* AbilitySystemComponent) const
{
	if (!AbilitySystemComponent) return false;
	
	// If we don't have any stamina don't attack
	const UMMOAttributeSet* Attributes = Cast<UMMOAttributeSet>(AbilitySystemComponent->GetAttributeSet(UMMOAttributeSet::StaticClass()));
	if (Attributes && Attributes->GetStamina() == 0)
	{
		return true;
	}

	return false;
}


bool UCombatAbility::ShouldActivateAbilityToRetrieveArmament_Implementation() const
{
	if (Armament == nullptr)
	{
		return true;
	}

	return false;
}


bool UCombatAbility::IsWeaponEquipped_Implementation(const EInputAbilities AbilityInput, UCombatComponent* CombatComponent) const
{
	if (!CombatComponent)
	{
		return false;
	}

	// Check if there's a valid weapon for this specific ability (This is here to prevent spamming replicated events)
	bool bCanActivateAbility = false;
	if (IsRightHandAbilityInput(AbilityInput)
		&& CombatComponent->GetArmament()
		&& CombatComponent->GetArmament()->GetEquipStatus() == EEquipStatus::Equipped)
	{
		bCanActivateAbility = true;
	}
	else if (!IsRightHandAbilityInput(AbilityInput)
		&& CombatComponent->GetArmament(false)
		&& CombatComponent->GetArmament(false)->GetEquipStatus() == EEquipStatus::Equipped)
	{
		bCanActivateAbility = true;
	}

	return bCanActivateAbility;
}


void UCombatAbility::SetArmament_Implementation(AArmament* NewArmament)
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
	}
	
	BP_OnUnequipArmament(ArmamentName, Id, Slot);
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
