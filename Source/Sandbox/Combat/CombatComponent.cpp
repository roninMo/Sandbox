// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/CombatComponent.h"

#include "Sandbox/Data/Enums/HitReacts.h"
#include "Sandbox/Data/Enums/EquipSlot.h"
#include "Sandbox/Data/Enums/ArmorTypes.h"
#include "Sandbox/Data/Enums/AttributeTypes.h"
#include "Sandbox/Asc/Information/SandboxTags.h"

#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Logging/StructuredLog.h"

#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Data/Enums/HitDirection.h"
#include "Weapons/Armament.h"

DEFINE_LOG_CATEGORY(CombatComponentLog);
// TODO: Custom logging to remove the extra message logic for clarification


#pragma region Constructors
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Prev hitstun durations
	// HitStunDurations.Add(EHitStun::None, 0);
	// HitStunDurations.Add(EHitStun::VeryShort, 0.1);
	// HitStunDurations.Add(EHitStun::Short, 0.2);
	// HitStunDurations.Add(EHitStun::Medium, 0.45);
	// HitStunDurations.Add(EHitStun::Long, 0.64);
	// HitStunDurations.Add(EHitStun::Knockdown, 0.9);
	// HitStunDurations.Add(EHitStun::FacePlant, 1.5);
	// HitStunDurations.Add(EHitStun::FrontFlip, 2);

	
	// Cached tags
	HitStunEffectTag = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Attack_HitStun);
	HitStunTag = FGameplayTag::RequestGameplayTag(Tag_State_HitStun);
	PreventHealthRegenEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Regen_Health);
	PreventHealthRegen = FGameplayTag::RequestGameplayTag(Tag_Block_Regen_Health);
	PreventPoiseRegenEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Regen_Poise);
	PreventPoiseRegen = FGameplayTag::RequestGameplayTag(Tag_Block_Regen_Poise);
	PreventStaminaRegenEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Regen_Stamina);
	PreventStaminaRegen = FGameplayTag::RequestGameplayTag(Tag_Block_Regen_Stamina);
	PreventManaRegenEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Regen_Mana);
	PreventManaRegen = FGameplayTag::RequestGameplayTag(Tag_Block_Regen_Mana);
	PreventCurseBuildupEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Buildup_Curse);
	PreventCurseBuildup = FGameplayTag::RequestGameplayTag(Tag_Block_Buildup_Curse);
	PreventBleedBuildupEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Buildup_Bleed);
	PreventBleedBuildup = FGameplayTag::RequestGameplayTag(Tag_Block_Buildup_Bleed);
	PreventPoisonBuildupEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Buildup_Poison);
	PreventPoisonBuildup = FGameplayTag::RequestGameplayTag(Tag_Block_Buildup_Poison);
	PreventFrostbiteBuildupEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Buildup_Frostbite);
	PreventFrostbiteBuildup = FGameplayTag::RequestGameplayTag(Tag_Block_Buildup_Frostbite);
	PreventMadnessBuildupEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Buildup_Madness);
	PreventMadnessBuildup = FGameplayTag::RequestGameplayTag(Tag_Block_Buildup_Madness);
	PreventSleepBuildupEffect = FGameplayTag::RequestGameplayTag(Tag_GameplayEffect_Block_Buildup_Sleep);
	PreventSleepBuildup = FGameplayTag::RequestGameplayTag(Tag_Block_Buildup_Sleep);
}


void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatComponent, PrimaryArmament, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatComponent, SecondaryArmament, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatComponent, CurrentStance, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatComponent, ComboIndex, COND_Custom, REPNOTIFY_OnChanged);
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatComponent::CombatCalculations(const FGAttributeSetExecutionData& Props)
{
}
#pragma endregion 



void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HandleClearingStatuses();
}




#pragma region Armaments
void UCombatComponent::AddArmamentToEquipSlot(const F_Item& ArmamentInventoryInformation, const EEquipSlot EquipSlot)
{
	if (!ArmamentInventoryInformation.IsValid())
	{
		return;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		return;
	}

	// Handle logic on the server
	if (Character->IsLocallyControlled() && !Character->HasAuthority())
	{
		// TODO: Only send the id across the network, handle retrieving the information from the inventory
		Server_AddArmamentToEquipSlot(ArmamentInventoryInformation, EquipSlot);
	}

	
	// Add the armament to the equipped armaments
	if (EquipSlot == EEquipSlot::LeftHandSlotOne) LeftHandEquipSlot_One = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::LeftHandSlotTwo) LeftHandEquipSlot_Two = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::LeftHandSlotThree) LeftHandEquipSlot_Three = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotOne) RightHandEquipSlot_One = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotTwo) RightHandEquipSlot_Two = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotThree) RightHandEquipSlot_Three = ArmamentInventoryInformation;
	
	// Equip the new armament if you've updated one of the currently equipped slots
	if (Character->HasAuthority())
	{
		if (PrimaryArmament && EquipSlot == PrimaryArmament->GetEquipSlot())
		{
			CreateArmament(EquipSlot);
		}
		
		if (SecondaryArmament && EquipSlot == SecondaryArmament->GetEquipSlot())
		{
			CreateArmament(EquipSlot);
		}
	}
}


void UCombatComponent::RemoveArmamentFromEquipSlot(const EEquipSlot EquipSlot)
{
	if (EquipSlot == EEquipSlot::None)
	{
		return;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		return;
	}

	// Handle logic on the server
	if (Character->IsLocallyControlled() && !Character->HasAuthority())
	{
		Server_RemoveArmamentFromEquipSlot(EquipSlot);
	}

	
	// Remove the armament from the equipped armaments
	if (EquipSlot == EEquipSlot::LeftHandSlotOne) LeftHandEquipSlot_One = F_Item();
	else if (EquipSlot == EEquipSlot::LeftHandSlotTwo) LeftHandEquipSlot_Two = F_Item();
	else if (EquipSlot == EEquipSlot::LeftHandSlotThree) LeftHandEquipSlot_Three = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotOne) RightHandEquipSlot_One = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotTwo) RightHandEquipSlot_Two = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotThree) RightHandEquipSlot_Three = F_Item();
	
	// Delete the armament if it's currently equipped
	if (Character->HasAuthority())
	{
		if (PrimaryArmament && EquipSlot == PrimaryArmament->GetEquipSlot())
		{
			DeleteEquippedArmament(PrimaryArmament);
		}
		else if (SecondaryArmament && EquipSlot == SecondaryArmament->GetEquipSlot())
		{
			DeleteEquippedArmament(SecondaryArmament);
		}
	}
}


AArmament* UCombatComponent::CreateArmament(const EEquipSlot EquipSlot)
{
	// Sanity checks
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while creating the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	if (!Character->HasAuthority())
	{
		// UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Only create the armament on the server!",
		// 	UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystemComponent)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system while creating the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	
	// Retrieve the armament information
	F_Item ArmamentItemData = GetArmamentInventoryInformation(EquipSlot);
	if (!ArmamentItemData.ActualClass || !ArmamentItemData.ItemName.IsValid())
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve valid armament class while creating the armament! Item: {3} ->  {4}",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), ArmamentItemData.ItemName, *GetNameSafe(ArmamentItemData.ActualClass));
		return nullptr;
	}

	F_ArmamentInformation ArmamentData = GetArmamentInformationFromDatabase(ArmamentItemData.ItemName);
	if (!ArmamentData.IsValid())
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve valid armament information while creating the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}
	
	// If there's nothing to attach the armament to, don't equip the armament
	FName EquipSocket = GetEquippedSocketName(ArmamentData.Classification, EquipSlot);
	const USkeletalMeshSocket* CharacterSocket = GetSkeletalSocket(EquipSocket);
	if (!CharacterSocket)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to find an equip socket while creating the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = GetOwner();
	const FTransform SpawnLocation = CharacterSocket->GetSocketTransform(Character->GetMesh());
	
	// Spawn the armament and retrieve it's information, and if any of that fails delete the armament
	AArmament* Armament = Cast<AArmament>(GetWorld()->SpawnActor(ArmamentItemData.ActualClass, &SpawnLocation, SpawnParameters));
	if (Armament)
	{
		bool bRightHand = IsRightHandedArmament(EquipSlot);
		UE_LOGFMT(CombatComponentLog, Log, "{0}::{1}() {2} Created the armament -> {3}({4}): {5} ",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()),
			ArmamentItemData.ItemName, *ArmamentItemData.Id.ToString(), *UEnum::GetValueAsString(EquipSlot)
		);

		// Add the armament's information and construct the armament
		Armament->SetArmamentInformation(ArmamentData); // Replicated during EquipSlot OnRep ->  Retrieved from combat component
		Armament->SetArmamentMontagesFromDB(MontageInformationTable, Character->GetCharacterSkeletonMapping());
		Armament->SetArmamentEquipSlot(EquipSlot);
		Armament->Execute_SetItem(Armament, ArmamentItemData);
		Armament->Execute_SetId(Armament, ArmamentItemData.Id);
		Armament->SetEquipStatus(EEquipStatus::Unequipped); // Not replicated 
		if (Armament->IsValidArmanent() && Armament->ConstructArmament())
		{
			// Delete the currently equipped armament, if there is one
			DeleteEquippedArmament(GetArmament(bRightHand));

			// Equip the armament
			if (!bRightHand)
			{
				SecondaryArmament = Armament;
			}
			else
			{
				PrimaryArmament = Armament;
			}
			
			// Update the armament stance and combat abilities
			UpdateArmamentStanceAndAbilities();

			OnEquippedArmament.Broadcast(Armament, EquipSlot);
			return Armament;
		}
	}

	// If it wasn't created successfully
	if (Armament)
	{
		Armament->DeconstructArmament();
		Armament->Destroy();
	}

	UE_LOGFMT(LogTemp, Error, "{0}::{1}() {2} failed to create the armament {3}!",
		*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), ArmamentItemData.ItemName);
	return nullptr;
}


bool UCombatComponent::DeleteEquippedArmament(AArmament* Armament)
{
	if (!Armament)
	{
		return true;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while deleting the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	if (!Character->HasAuthority())
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Only delete the armament on the server!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	// Deconstruct the armament and remove the armament abilities before deleting the armament
	if (Armament->DeconstructArmament())
	{
		if (IsRightHandedArmament(Armament->GetEquipSlot())) PrimaryArmament = nullptr;
		else SecondaryArmament = nullptr;

		// Update the armament stance and combat abilities
		UpdateArmamentStanceAndAbilities();

		OnUnequippedArmament.Broadcast(Armament->GetArmamentId(), Armament->Execute_GetId(Armament), Armament->GetEquipSlot());
		Armament->Destroy();
		return true;
	}
	
	return false;
}


void UCombatComponent::SetArmamentStance(const EArmamentStance Stance)
{
	const EArmamentStance PreviousStance = CurrentStance;
	if (CurrentStance != Stance)
	{
		CurrentStance = Stance;
		UpdateArmamentCombatAbilities(PreviousStance);
	}
}


void UCombatComponent::UpdateArmamentStanceAndAbilities()
{
	// Update the armament stance based on the equipped weapons
	EArmamentStance PreviousStance = CurrentStance;
	bool bPrimaryEquipped = PrimaryArmament && PrimaryArmament->GetEquipStatus() == EEquipStatus::Equipped;
	bool bSecondaryEquipped = SecondaryArmament && SecondaryArmament->GetEquipStatus() == EEquipStatus::Equipped;
	bool bDualWielding = bPrimaryEquipped && bSecondaryEquipped && PrimaryArmament->GetArmamentInformation().Classification == SecondaryArmament->GetArmamentInformation().Classification;
	// bool bOnlyTwoHandPrimary = bPrimaryEquipped && PrimaryArmament->GetEquipRestrictions() == EEquipRestrictions::TwoHandOnly; // TODO: add this logic 
	// bool bOnlyTwoHandSecondary = bSecondaryEquipped && SecondaryArmament->GetEquipRestrictions() == EEquipRestrictions::TwoHandOnly;

	// Both weapons
	if (!PrimaryArmament && !SecondaryArmament)
	{
		CurrentStance = EArmamentStance::None;
	}
	else if (PrimaryArmament && SecondaryArmament)
	{
		if (PrimaryArmament->GetArmamentInformation().Classification == SecondaryArmament->GetArmamentInformation().Classification)
		{
			CurrentStance = EArmamentStance::DualWielding;
		}
		else
		{
			CurrentStance = EArmamentStance::TwoWeapons;
		}
	}
	// One weapon
	else if (PrimaryArmament || SecondaryArmament)
	{
		// On unequip we're just going to default to the player one handing
		CurrentStance = EArmamentStance::OneHanding;
	}

	UpdateArmamentCombatAbilities(PreviousStance);
}


void UCombatComponent::UpdateArmamentCombatAbilities(EArmamentStance PreviousStance)
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while updating the combat abilities!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	if (!Character->HasAuthority())
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Only update armaments combat abilities on the server!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UAbilitySystem* Asc = Character->GetAbilitySystem<UAbilitySystem>();
	if (!Asc)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system component!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	
	TMap<EInputAbilities, F_ArmamentAbilityInformation> CombatAbilities;
	if (PrimaryArmament)
	{
		// Retrieve the primary weapon's combat abilities for this armament
		const TArray<F_ArmamentAbilityInformation>& Abilities = PrimaryArmament->GetMeleeAbilities();
		for (const F_ArmamentAbilityInformation& CombatAbility : Abilities)
		{
			// Always use the primary armament's combat abilities except for the offhand attack unless they're two handing (block) or dual wielding
			if (CombatAbility.InputId != EInputAbilities::SecondaryAttack && !CombatAbility.InvalidStances.Contains(GetCurrentStance()))
			{
				AddCombatAbilityIfValidStance(CombatAbilities, CombatAbility);
			}
			// Only use the secondary attack (Block or alt primary attack) for two handing or dual wielding
			else if (CombatAbility.InputId == EInputAbilities::SecondaryAttack)
			{
				if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons || CurrentStance == EArmamentStance::TwoHanding)
				{
					AddCombatAbilityIfValidStance(CombatAbilities, CombatAbility);
				}
			}
			
		}
	}

	if (SecondaryArmament)
	{
		// Retrieve the secondary weapon's combat abilities for this armament
		const TArray<F_ArmamentAbilityInformation>& Abilities = SecondaryArmament->GetMeleeAbilities();
		for (const F_ArmamentAbilityInformation& CombatAbility : Abilities)
		{
			// If we're two handing the weapon, retrieve every ability, otherwise retrieve the offhand ability
			if (CurrentStance == EArmamentStance::TwoHanding)
			{
				AddCombatAbilityIfValidStance(CombatAbilities, CombatAbility);
			}
			else if (CurrentStance == EArmamentStance::OneHanding)
			{
				if (EInputAbilities::SecondaryAttack == CombatAbility.InputId)
				{
					AddCombatAbilityIfValidStance(CombatAbilities, CombatAbility);
				}
			}
		}
	}


	// Clear the old abilities
	Asc->RemoveGameplayAbilities(CombatAbilityHandles);
	CombatAbilityHandles.Empty();

	// Add the stance's abilities
	for (auto &[AttackPattern, CombatAbility] : CombatAbilities)
	{
		// The id for each ability should be specific to the equipped armament
		FGuid SecondaryId = SecondaryArmament ? SecondaryArmament->Execute_GetId(SecondaryArmament) : FGuid();
		FGuid PrimaryId = PrimaryArmament ? PrimaryArmament->Execute_GetId(PrimaryArmament) : FGuid();
		FGuid Id = PrimaryId;

		/*
			- OneHanding
				- Primary (Primary, Strong, Special)
				- Secondary (Secondary)
				
			- TwoWeapons
				- Primary (Primary, Strong, Special)
				- Secondary (Secondary)

			- DualWielding
				- Primary (Primary, Strong, Special)
				- Secondary (Secondary)
				
			- TwoHanding
				- Secondary (Primary, Strong, Special, Secondary)

			
		*/
		if (CurrentStance == EArmamentStance::OneHanding || CurrentStance == EArmamentStance::TwoWeapons || CurrentStance == EArmamentStance::DualWielding)
		{
			if (EInputAbilities::SecondaryAttack == AttackPattern) Id = SecondaryId;
			if (EInputAbilities::PrimaryAttack == AttackPattern) Id = PrimaryId;
			if (EInputAbilities::SpecialAttack == AttackPattern) Id = PrimaryId;
			if (EInputAbilities::StrongAttack == AttackPattern) Id = PrimaryId;
		}
		else if (CurrentStance == EArmamentStance::TwoHanding && SecondaryArmament && SecondaryArmament->GetEquipStatus() == EEquipStatus::Equipped) Id = SecondaryId;
		else if (CurrentStance == EArmamentStance::TwoHanding && PrimaryArmament && PrimaryArmament->GetEquipStatus() == EEquipStatus::Equipped) Id = PrimaryId;

		// Add the ability
		CombatAbilityHandles.Add(Asc->AddAbility(FGameplayAbilityInfo(CombatAbility.Ability, CombatAbility.Level, CombatAbility.InputId, nullptr, Id)));
	}
	

	/*

		Eventually only add/remove the abilities required using these conditions

			Primary weapon
				- Every combat ability except the secondary attack

			Two Handed, Two Weapons, Or Dual Wielding
				- Every combat ability

			Offhand weapon
				- Secondary attack

			And if they go from dual wielding to two handing a weapon, they need to use the proper abilities

			if (PreviousStance == EArmamentStance::DualWielding || PreviousStance == EArmamentStance::TwoWeapons || PreviousStance == EArmamentStance::TwoHanding)
			{
				// Only primary weapon, remove the offhand ability

				// Only offhand weapon, remove all other abilities

				// If dual wielding, use each of the weapon's abilities

				// If they transition to two handing, add the abilities of the specific armament
			}
			else if (PreviousStance == EArmamentStance::OneHanding)
			{
				// Primary Weapon
				if (PrimaryArmament)
				{
					// Primary weapon
					// 	- Every combat ability except the secondary attack
					

				}

				// Offhand Weapon
				if (SecondaryArmament)
				{
					// Offhand weapon
					// 	- Secondary attack

					
				}
			}

	*/

}


void UCombatComponent::OnRep_CurrentStance()
{
	// Update client state based on the current stance
}


AArmament* UCombatComponent::GetArmament(const bool bRightHand) const
{
	if (!bRightHand) return SecondaryArmament;
	return PrimaryArmament;
}


bool UCombatComponent::IsRightHandedArmament(EEquipSlot Slot) const
{
	if (Slot == EEquipSlot::LeftHandSlotOne || Slot == EEquipSlot::LeftHandSlotTwo || Slot == EEquipSlot::LeftHandSlotThree) return false;
	return true;
}


EEquipSlot UCombatComponent::GetCurrentlyEquippedSlot(const bool bRightHand)
{
	if (!bRightHand)
	{
		if (SecondaryArmament) return SecondaryArmament->GetEquipSlot();
		return EEquipSlot::None;
	}
	
	if (PrimaryArmament) return PrimaryArmament->GetEquipSlot();
	return EEquipSlot::None;
}


EEquipSlot UCombatComponent::GetNextEquipSlot(const bool bRightHand)
{
	int32& Index = bRightHand ? ArmamentIndex : OffhandArmamentIndex;
	Index = Index + 1 > 2 ? 0 : Index + 1;
	EEquipSlot EquipSlot;

	if (!bRightHand)
	{
		if (Index == 1 && LeftHandEquipSlot_Two.IsValid()) EquipSlot = EEquipSlot::LeftHandSlotTwo;
		else if (Index == 2) EquipSlot = EEquipSlot::LeftHandSlotThree;
		else EquipSlot = EEquipSlot::LeftHandSlotOne;
	}
	else
	{
		if (Index == 1 && RightHandEquipSlot_Two.IsValid()) EquipSlot = EEquipSlot::RightHandSlotTwo;
		else if (Index == 2) EquipSlot = EEquipSlot::RightHandSlotThree;
		else EquipSlot = EEquipSlot::RightHandSlotOne;
	}
	
	return  EquipSlot;
}


F_Item UCombatComponent::GetArmamentInventoryInformation(const EEquipSlot Slot)
{
	if (Slot == EEquipSlot::LeftHandSlotOne)    return LeftHandEquipSlot_One;
	if (Slot == EEquipSlot::LeftHandSlotTwo)    return LeftHandEquipSlot_Two;
	if (Slot == EEquipSlot::LeftHandSlotThree)  return LeftHandEquipSlot_Three;
	if (Slot == EEquipSlot::RightHandSlotOne)   return RightHandEquipSlot_One;
	if (Slot == EEquipSlot::RightHandSlotTwo)   return RightHandEquipSlot_Two;
	if (Slot == EEquipSlot::RightHandSlotThree) return RightHandEquipSlot_Three;
	return F_Item();
}


F_ArmamentInformation UCombatComponent::GetArmamentInformationFromDatabase(const FName ArmamentId)
{
	if (ArmamentId.IsNone())
	{
		return F_ArmamentInformation();
	}
	
	if (ArmamentInformationTable)
	{
		const FString RowContext(TEXT("Armament Information Context"));
		if (const F_Table_ArmamentInformation* Data = ArmamentInformationTable->FindRow<F_Table_ArmamentInformation>(ArmamentId, RowContext))
		{
			return Data->ArmamentInformation;
		}
		else
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve {3} from the armament information table!",
				UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), ArmamentId);
		}
	}
	else
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2}'s Armament information table hasn't been added to the character yet!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
	}

	return F_ArmamentInformation();
}


void UCombatComponent::Server_AddArmamentToEquipSlot_Implementation(const F_Item& ArmamentData, const EEquipSlot EquipSlot)
{
	AddArmamentToEquipSlot(ArmamentData, EquipSlot);
}


void UCombatComponent::Server_RemoveArmamentFromEquipSlot_Implementation(const EEquipSlot EquipSlot)
{
	RemoveArmamentFromEquipSlot(EquipSlot);
}


void UCombatComponent::Server_CreateArmament_Implementation(const EEquipSlot EquipSlot)
{
	CreateArmament(EquipSlot);
}
#pragma endregion 




#pragma region Armors
bool UCombatComponent::UnequipArmor(EArmorSlot ArmorSlot)
{
	// Sanity checks
	if (!ArmorAbilityHandles.Contains(ArmorSlot))
	{
		return true;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while removing the armor!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}
	
	
	// Broadcast the event
	F_Item Information = GetArmorItemInformation(ArmorSlot);
	OnUnequippedArmor.Broadcast(GetArmorItemInformation(ArmorSlot), ArmorAbilities[ArmorSlot], ArmorSlot);

	// Handle abilities and attributes
	if (Character->HasAuthority())
	{
		UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
		if (!AbilitySystemComponent)
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system while remove the armor!",
				UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
			return false;
		}

		
		// Remove any of the old armor abilities
		F_Information_Armor_Handle& OldArmor = ArmorAbilityHandles[ArmorSlot];
		AbilitySystemComponent->RemoveGameplayAbilities(OldArmor.AbilityHandles);
		AbilitySystemComponent->RemoveGameplayEffect(OldArmor.ArmorStats);
		for (FActiveGameplayEffectHandle& CurrentPassive : OldArmor.PassiveHandles)
		{
			AbilitySystemComponent->RemoveGameplayEffect(CurrentPassive);
		}
	}

	// Remove the armor from the character
	Character->SetArmorMesh(ArmorSlot, nullptr);

	// Clear the old armor information
	if (EArmorSlot::Gauntlets == ArmorSlot) Gauntlets = F_Item();
	if (EArmorSlot::Leggings == ArmorSlot) Leggings = F_Item();
	if (EArmorSlot::Chest == ArmorSlot) Chest = F_Item();
	if (EArmorSlot::Helm == ArmorSlot) Helm = F_Item();
	ArmorAbilityHandles.Remove(ArmorSlot);
	ArmorAbilities.Remove(ArmorSlot);
	return true;
}


bool UCombatComponent::EquipArmor(F_Item Armor)
{
	if (!Armor.IsValid())
	{
		return false;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while creating the armor!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	F_Information_Armor ArmorInformation = GetArmorFromDatabase(Armor.ItemName);
	if (!ArmorInformation.Id.IsValid() || ArmorInformation.ArmorSlot == EArmorSlot::None)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the armor information!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}
	
	// Remove any of the old armor
	if (ArmorAbilityHandles.Contains(ArmorInformation.ArmorSlot))
	{
		UnequipArmor(ArmorInformation.ArmorSlot);
	}

	
	// Equip the armor
	F_Information_Armor_Handle ArmorHandle;
	if (Character->HasAuthority())
	{
		UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
		if (!AbilitySystemComponent)
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system while creating the armor!",
				UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
			return false;
		}
		
		ArmorHandle.ArmorStats = AbilitySystemComponent->AddGameplayEffect(ArmorInformation.ArmorStats);
		
		// Passives
		for (const FGameplayEffectInfo& Passive : ArmorInformation.Passives)
		{
			ArmorHandle.PassiveHandles.Add(AbilitySystemComponent->AddGameplayEffect(Passive));
		}
		
		// Abilities
		for (const FGameplayAbilityInfo& Ability : ArmorInformation.Abilities)
		{
			ArmorHandle.AbilityHandles.Add(AbilitySystemComponent->AddAbility(Ability));
		}
	

		// Equip the armor to the character
		Character->SetArmorMesh(ArmorInformation.ArmorSlot, ArmorInformation.ArmorMesh);
	}
	
	// Broadcast the event
	OnEquippedArmor.Broadcast(Armor, ArmorInformation, ArmorInformation.ArmorSlot);
	
	// Update the armor information
	if (EArmorSlot::Gauntlets == ArmorInformation.ArmorSlot) Gauntlets = Armor;
	if (EArmorSlot::Leggings == ArmorInformation.ArmorSlot) Leggings = Armor;
	if (EArmorSlot::Chest == ArmorInformation.ArmorSlot) Chest = Armor;
	if (EArmorSlot::Helm == ArmorInformation.ArmorSlot) Helm = Armor;
	ArmorAbilities.Add(ArmorInformation.ArmorSlot, ArmorInformation);
	ArmorAbilityHandles.Add(ArmorInformation.ArmorSlot, ArmorHandle);
	return true;
}


F_Item UCombatComponent::GetArmorItemInformation(EArmorSlot ArmorSlot)
{
	if (EArmorSlot::Gauntlets == ArmorSlot) return Gauntlets;
	if (EArmorSlot::Leggings == ArmorSlot) return Leggings;
	if (EArmorSlot::Chest == ArmorSlot) return Chest;
	if (EArmorSlot::Helm == ArmorSlot) return Helm;
	return F_Item();
}


F_Information_Armor UCombatComponent::GetArmorAbilityInformation(EArmorSlot ArmorSlot)
{
	if (ArmorAbilities.Contains(ArmorSlot))
	{
		return ArmorAbilities[ArmorSlot];
	}
	return F_Information_Armor();
}


USkeletalMeshComponent* UCombatComponent::GetArmorMesh(EArmorSlot ArmorSlot)
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while retrieving the armor mesh!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}
	
	return nullptr;
}


const F_Information_Armor UCombatComponent::GetArmorFromDatabase(const FName Id) const
{
	if (ArmorInformationTable)
	{
		const FString RowContext(TEXT("Armor Information Context"));
		if (const F_Table_Armors* Data = ArmorInformationTable->FindRow<F_Table_Armors>(Id, RowContext))
		{
			return Data->ArmorInformation;
		}
		else
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve {2} from the armor information table!",
				UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), Id);
		}
	}
	else
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} The armor information table hasn't been added to the character yet!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
	}

	return F_Information_Armor();
}


void UCombatComponent::Server_EquipArmor_Implementation(const F_Item& Armor)
{
	EquipArmor(Armor);
}
#pragma endregion 




#pragma region Combat Logic
EHitDirection UCombatComponent::GetHitReactDirection(AActor* Actor, const FVector& ActorLocation, const FVector& ImpactLocation) const
{
	// TODO: Refactor this logic
	const float DistanceToFrontBackPlane = FVector::PointPlaneDist(ImpactLocation, ActorLocation, Actor->GetActorRightVector());
	const float DistanceToRightLeftPlane = FVector::PointPlaneDist(ImpactLocation, ActorLocation, Actor->GetActorForwardVector());
	//UE_LOG(LogTemp, Warning, TEXT("DistanceToFrontBackPlane: %f, DistanceToRightLeftPlane: %f"), DistanceToFrontBackPlane, DistanceToRightLeftPlane);

	EHitDirection HitDirection;
	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		HitDirection = DistanceToRightLeftPlane >= 0 ? EHitDirection::Front : EHitDirection::Back; // Front or back
	}
	else
	{
		HitDirection = DistanceToFrontBackPlane >= 0 ? EHitDirection::Right : EHitDirection::Left; // Determine if Right or Left
	}
	return HitDirection;
}


void UCombatComponent::HandleDamageTaken(ACharacterBase* Enemy, UObject* Source, float Value, const FGameplayAttribute Attribute)
{
	OnPlayerAttacked.Broadcast(Cast<ACharacterBase>(GetOwner()), Enemy, Source, Value, Attribute);
}


void UCombatComponent::PoiseBreak(ACharacterBase* Enemy, AActor* Source, float PoiseDamage, EHitStun HitStun, EHitDirection HitDirection)
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to add a HitStun duration when the character wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UAbilitySystem* AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to add a HitStun duration when the ability system component wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	
	// Hit react
	if (HitStun != EHitStun::None)
	{
		TSubclassOf<UGameplayEffect> HitStunDurationClass = GetHitStunDurationEffect(HitStun);
		UGameplayEffect* HitStunDuration = HitStunDurationClass ? HitStunDurationClass->GetDefaultObject<UGameplayEffect>() : nullptr;
		if (HitStunDuration)
		{
			// If we need to handle removing abilities or add other extra configuration, handle it here, when we have direct access to the gameplay effect (do not edit blueprint tsubclasses, it adjusts values in the editor
			

			AActor* Instigator = Enemy && Enemy->GetAbilitySystemComponent() ? Enemy->GetAbilitySystemComponent()->GetOwnerActor() : Enemy;
			AActor* EffectCauser = Enemy && Enemy->GetAbilitySystemComponent() ? Enemy->GetAbilitySystemComponent()->GetAvatarActor() : Enemy;
			
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystem->MakeEffectContext();
			EffectContextHandle.AddInstigator(Instigator, EffectCauser);
			EffectContextHandle.AddSourceObject(Source);
			AbilitySystem->ApplyGameplayEffectToSelf(HitStunDuration, 1, EffectContextHandle);
		}
		
		// Hit react montage
		UAnimMontage* HitReactMontage = Character->GetCharacterMontages().HitReactMontage;
		if (HitReactMontage)
		{
			Character->NetMulticast_PlayMontage(HitReactMontage, Character->GetHitReactSection(HitDirection, HitStun));
		}
	}

	OnPoiseBroken.Broadcast(Character, Enemy, HitStun, HitDirection, PoiseDamage);
}


void UCombatComponent::HandleDeath(ACharacterBase* Enemy, AActor* Source, FName MontageSection)
{
	UGameplayEffect* DeathEffect = DeathEffectClass ? DeathEffectClass->GetDefaultObject<UGameplayEffect>() : nullptr;
	if (!DeathEffect)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle death when the effect information wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle death when the character wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UAbilitySystem* AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle death when the ability system component wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	// Add the GE for handling death state and information
	AActor* Instigator = Enemy && Enemy->GetAbilitySystemComponent() ? Enemy->GetAbilitySystemComponent()->GetOwnerActor() : Enemy;
	AActor* EffectCauser = Enemy && Enemy->GetAbilitySystemComponent() ? Enemy->GetAbilitySystemComponent()->GetAvatarActor() : Enemy;

	FGameplayEffectContextHandle EffectContextHandle = AbilitySystem->MakeEffectContext();
	EffectContextHandle.AddInstigator(Instigator, EffectCauser);
	EffectContextHandle.AddSourceObject(Source);
	AbilitySystem->ApplyGameplayEffectToSelf(DeathEffect, 1, EffectContextHandle);


	UAnimMontage* DeathMontage = Character->GetCharacterMontages().DeathMontage;
	if (!DeathMontage) Character->GetMesh()->SetHiddenInGame(true, true);
	else
	{
		Character->NetMulticast_PlayMontage(DeathMontage, MontageSection);
	}

	OnDeath.Broadcast(Character, Enemy);
}


void UCombatComponent::HandleRespawn(ACharacterBase* Enemy, AActor* Source)
{
	// if (!RespawnInformation)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to respawn when the character's respawn information wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle curse when the character wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	if (!Character->HasAuthority())
	{
		// UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Only respawn the character on authority!",
		// 	*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UAbilitySystem* AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle curse when the ability system component wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}


	// Respawn, reset state, abilities, and equipment
	// Retrieve the character's save information, and respawn the character


	OnRespawn.Broadcast(Character, Character->GetActorLocation());
}


void UCombatComponent::HandleClearingStatuses()
{
	// Sanity checks
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried clearing statuses when the character wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	if (!Character->HasAuthority())
	{
		// UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried clearing statuses when it wasn't on authority!",
		// 	*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UAbilitySystem* AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		// UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried clearing statuses when the ability system component wasn't valid!",
		// 	*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	const UMMOAttributeSet* Attributes = Cast<UMMOAttributeSet>(AbilitySystem->GetAttributeSet(UMMOAttributeSet::StaticClass()));
	if (!Attributes)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried clearing statuses when the attribute set wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	
	// Try to remove the status effects, and notify the player
	if (PoisonedHandle.IsValid() && Attributes->GetPoisonBuildup() == 0.0)
	{
		if (AbilitySystem->RemoveActiveGameplayEffect(PoisonedHandle))
		{
			PoisonedHandle.Invalidate();
			OnStatusCleared.Broadcast(Character, Attributes->GetPoisonBuildupAttribute(), 0.0);
		}
	}

	if (FrostbittenHandle.IsValid() && Attributes->GetFrostbiteBuildup() == 0.0)
	{
		if (AbilitySystem->RemoveActiveGameplayEffect(FrostbittenHandle))
		{
			FrostbittenHandle.Invalidate();
			OnStatusCleared.Broadcast(Character, Attributes->GetFrostbiteBuildupAttribute(), 0.0);
		}
	}

	if (MaddenedHandle.IsValid() && Attributes->GetMadnessBuildup() == 0.0)
	{
		if (AbilitySystem->RemoveActiveGameplayEffect(MaddenedHandle))
		{
			MaddenedHandle.Invalidate();
			OnStatusCleared.Broadcast(Character, Attributes->GetMadnessBuildupAttribute(), 0.0);
		}
	}

	if (CursedHandle.IsValid() && Attributes->GetCurseBuildup() == 0.0)
	{
		if (AbilitySystem->RemoveActiveGameplayEffect(CursedHandle))
		{
			CursedHandle.Invalidate();
			OnStatusCleared.Broadcast(Character, Attributes->GetCurseBuildupAttribute(), 0.0);
		}
	}

	if (SleepHandle.IsValid() && Attributes->GetSleepBuildup() == 0.0)
	{
		if (AbilitySystem->RemoveActiveGameplayEffect(SleepHandle))
		{
			SleepHandle.Invalidate();
			OnStatusCleared.Broadcast(Character, Attributes->GetSleepBuildupAttribute(), 0.0);
		}
	}
}


void UCombatComponent::StatusProc(ACharacterBase* Enemy, AActor* Source, const FGameplayAttribute& Attribute, float NewValue)
{
	if (!(Attribute == UMMOAttributeSet::GetCurseBuildupAttribute() ||
		Attribute == UMMOAttributeSet::GetBleedBuildupAttribute() ||
		Attribute == UMMOAttributeSet::GetPoisonBuildupAttribute() ||
		Attribute == UMMOAttributeSet::GetFrostbiteBuildupAttribute() ||
		Attribute == UMMOAttributeSet::GetMadnessBuildupAttribute() ||
		Attribute == UMMOAttributeSet::GetSleepBuildupAttribute()))
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle a statuc proc with an invalid attribute!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	// Yeah
	const TSubclassOf<UGameplayEffect>& StatusEffectClass = Attribute == UMMOAttributeSet::GetCurseBuildupAttribute() ? CursedStateClass :
		                                                        Attribute == UMMOAttributeSet::GetBleedBuildupAttribute() ? GE_Bled :
		                                                        Attribute == UMMOAttributeSet::GetPoisonBuildupAttribute() ? GE_Poisoned :
		                                                        Attribute == UMMOAttributeSet::GetFrostbiteBuildupAttribute() ? GE_Frostbitten :
		                                                        Attribute == UMMOAttributeSet::GetMadnessBuildupAttribute() ? GE_Maddened : GE_Slept;

	FActiveGameplayEffectHandle& EffectHandle = Attribute == UMMOAttributeSet::GetCurseBuildupAttribute() ? CursedHandle :
												Attribute == UMMOAttributeSet::GetBleedBuildupAttribute() ? BleedHandle :
												Attribute == UMMOAttributeSet::GetPoisonBuildupAttribute() ? PoisonedHandle :
												Attribute == UMMOAttributeSet::GetFrostbiteBuildupAttribute() ? FrostbittenHandle :
												Attribute == UMMOAttributeSet::GetMadnessBuildupAttribute() ? MaddenedHandle : SleepHandle;

	ECombatAttribute BP_Attribute = Attribute == UMMOAttributeSet::GetCurseBuildupAttribute() ? ECombatAttribute::Curse :
									Attribute == UMMOAttributeSet::GetBleedBuildupAttribute() ? ECombatAttribute::Bleed :
									Attribute == UMMOAttributeSet::GetPoisonBuildupAttribute() ? ECombatAttribute::Poison :
									Attribute == UMMOAttributeSet::GetFrostbiteBuildupAttribute() ? ECombatAttribute::Frostbite :
									Attribute == UMMOAttributeSet::GetMadnessBuildupAttribute() ? ECombatAttribute::Madness : ECombatAttribute::Sleep;
	
	
	UGameplayEffect* StatusEffect = StatusEffectClass ? StatusEffectClass->GetDefaultObject<UGameplayEffect>() : nullptr;
	if (!StatusEffect)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to add a status proc when the effect information wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}
	
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle curse  when the character wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	UAbilitySystem* AbilitySystem = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystem)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to handle curse when the ability system component wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	
	// Add the GE for handling death state and information
	AActor* Instigator = Enemy && Enemy->GetAbilitySystemComponent() ? Enemy->GetAbilitySystemComponent()->GetOwnerActor() : Enemy;
	AActor* EffectCauser = Enemy && Enemy->GetAbilitySystemComponent() ? Enemy->GetAbilitySystemComponent()->GetAvatarActor() : Enemy;

	FGameplayEffectContextHandle EffectContextHandle = AbilitySystem->MakeEffectContext();
	EffectContextHandle.AddInstigator(Instigator, EffectCauser);
	EffectContextHandle.AddSourceObject(Source);
	
	if (EffectHandle.IsValid()) AbilitySystem->RemoveActiveGameplayEffect(EffectHandle);
	EffectHandle = AbilitySystem->ApplyGameplayEffectToSelf(StatusEffect, 1, EffectContextHandle);
	BP_StatusProc(Enemy, Source, BP_Attribute, NewValue);
	OnStatusUpdate.Broadcast(Character, Enemy, Attribute, NewValue);
}


void UCombatComponent::HandleBleed(ACharacterBase* Enemy, AActor* Source, const FGameplayAttribute& Attribute, float NewValue)
{
	StatusProc(Enemy, Source, Attribute, NewValue);
	
	BP_HandleBleed(Enemy, Source, NewValue);
	OnBled.Broadcast(Cast<ACharacterBase>(GetOwner()), Enemy, Attribute, NewValue);
}


void UCombatComponent::HandleFrostbite(ACharacterBase* Enemy, AActor* Source, const FGameplayAttribute& Attribute, float NewValue)
{
	StatusProc(Enemy, Source, Attribute, NewValue);
	
	BP_HandleFrostbite(Enemy, Source, NewValue);
	OnFrostBitten.Broadcast(Cast<ACharacterBase>(GetOwner()), Enemy, Attribute, NewValue);
}


void UCombatComponent::HandlePoisoned(ACharacterBase* Enemy, AActor* Source, const FGameplayAttribute& Attribute, float NewValue)
{
	StatusProc(Enemy, Source, Attribute, NewValue);
	
	BP_HandlePoisoned(Enemy, Source, NewValue);
	OnPoisoned.Broadcast(Cast<ACharacterBase>(GetOwner()), Enemy, Attribute, NewValue);
}


void UCombatComponent::HandleMadness(ACharacterBase* Enemy, AActor* Source, const FGameplayAttribute& Attribute, float NewValue)
{
	StatusProc(Enemy, Source, Attribute, NewValue);
	
	BP_HandleMadness(Enemy, Source, NewValue);
	OnMaddened.Broadcast(Cast<ACharacterBase>(GetOwner()), Enemy, Attribute, NewValue);
}


void UCombatComponent::HandleCurse(ACharacterBase* Enemy, AActor* Source, const FGameplayAttribute& Attribute, float NewValue)
{
	StatusProc(Enemy, Source, Attribute, NewValue);

	BP_HandleCurse(Enemy, Source, NewValue);
	OnCursed.Broadcast(Cast<ACharacterBase>(GetOwner()), Enemy, Attribute, NewValue);
}


void UCombatComponent::HandleSleep(ACharacterBase* Enemy, AActor* Source, const FGameplayAttribute& Attribute, float NewValue)
{
	StatusProc(Enemy, Source, Attribute, NewValue);
	
	BP_HandleSleep(Enemy, Source, NewValue);
	OnSlept.Broadcast(Cast<ACharacterBase>(GetOwner()), Enemy, Attribute, NewValue);
}


bool UCombatComponent::IsImmuneToBleed_Implementation(ACharacterBase* Enemy, UObject* Source, const ECombatAttribute Attribute, float Value) { return false; }
bool UCombatComponent::IsImmuneToFrostbite_Implementation(ACharacterBase* Enemy, UObject* Source, const ECombatAttribute Attribute, float Value) { return false; }
bool UCombatComponent::IsImmuneToPoison_Implementation(ACharacterBase* Enemy, UObject* Source, const ECombatAttribute Attribute, float Value) { return false; }
bool UCombatComponent::IsImmuneToMadness_Implementation(ACharacterBase* Enemy, UObject* Source, const ECombatAttribute Attribute, float Value) { return false; }
bool UCombatComponent::IsImmuneToCurses_Implementation(ACharacterBase* Enemy, UObject* Source, const ECombatAttribute Attribute, float Value) { return false; }
bool UCombatComponent::IsImmuneToSleep_Implementation(ACharacterBase* Enemy, UObject* Source, const ECombatAttribute Attribute, float Value) { return false; }


UGameplayEffect* UCombatComponent::GetPreventAttributeAccumulationEffect(float Duration, const ECombatAttribute Attribute) const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to create a prevent regen effect when the character wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	const FGameplayTag AttributeTag = GetAttributePreventionTag(Attribute);
	if (!AttributeTag.IsValid() || Duration <= 0)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to create a prevent regen effect when the attribute or duration isn't valid! Duration: {3}, Attribute: {4}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), Duration, *UEnum::GetValueAsString(Attribute));
		return nullptr;
	}

	const FName PreventAttribute = FName(*GetNameSafe(Character) + FString("_PreventAccumulation_").Append(UEnum::GetValueAsString(Attribute)));
	UGameplayEffect* PreventRegenOrBuildup = NewObject<UGameplayEffect>(Character, PreventAttribute);
	if (PreventRegenOrBuildup)
	{
		PreventRegenOrBuildup->DurationPolicy = EGameplayEffectDurationType::HasDuration;
		PreventRegenOrBuildup->DurationMagnitude = FGameplayEffectModifierMagnitude(Duration);
		PreventRegenOrBuildup->StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;

		PreventRegenOrBuildup->InheritableGameplayEffectTags.AddTag(GetAttributePreventionTag(Attribute, false));
		PreventRegenOrBuildup->InheritableOwnedTagsContainer.AddTag(AttributeTag);

		return PreventRegenOrBuildup;
	}
	
	// FGameplayEffectSpec* StaminaCostSpec = new FGameplayEffectSpec(StaminaCost, MakeEffectContext(Handle, ActorInfo), 1);
	// FGameplayEffectSpecHandle StaminaCostHandle = FGameplayEffectSpecHandle(StaminaCostSpec);
	// ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, StaminaCostHandle);
	return nullptr;
}


TSubclassOf<UGameplayEffect> UCombatComponent::GetHitStunDurationEffect(EHitStun HitStun) const
{
	if (HitStun == EHitStun::None)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to add a hitstun duration without a valid hitstun!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	// Unreal sucks and doesn't let you add replicated custom gameplay effects because there isn't a reference to it on the client, so we're creating one for every hit react
	/*
		if (!HitStunDurations.Contains(HitStun))
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2}'s HitStun duration for {3} hasn't been setup yet!",
				*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(HitStun));
			return nullptr;
		}

		ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
		if (!Character)
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to create a poise broken effect when the character wasn't valid!",
				*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
			return nullptr;
		}

		const FName HitStunEffect = FName(*GetNameSafe(Character) + FString("_HitStun_").Append(UEnum::GetValueAsString(HitStun)));
		UGameplayEffect* HitStunDuration = NewObject<UGameplayEffect>(Character, HitStunEffect);
		if (HitStunDuration)
		{
			HitStunDuration->DurationPolicy = EGameplayEffectDurationType::HasDuration;
			HitStunDuration->DurationMagnitude = FGameplayEffectModifierMagnitude(HitStunDurations[HitStun]);
			HitStunDuration->StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
		
			HitStunDuration->InheritableGameplayEffectTags.AddTag(HitStunEffectTag);
			HitStunDuration->InheritableOwnedTagsContainer.AddTag(HitStunTag);
		
			return HitStunDuration;
		}
	*/
	
	// FGameplayEffectSpec* StaminaCostSpec = new FGameplayEffectSpec(StaminaCost, MakeEffectContext(Handle, ActorInfo), 1);
	// FGameplayEffectSpecHandle StaminaCostHandle = FGameplayEffectSpecHandle(StaminaCostSpec);
	// ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, StaminaCostHandle);
	
	if (HitStunDurations.Contains(HitStun)) return HitStunDurations[HitStun];
	return nullptr;
}


UGameplayEffect* UCombatComponent::GetPreventAttributesAccumulationEffect(float Duration, const TArray<ECombatAttribute>& Attributes) const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to create a prevent regen effect when the character wasn't valid!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}

	if (Duration <= 0)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Tried to create a prevent regen effect when the duration wasn't valid! Duration: {3}",
			*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), Duration);
		return nullptr;
	}
	
	TArray<FGameplayTag> AttributeTags = {};
	for (const ECombatAttribute Attribute : Attributes)
	{
		const FGameplayTag AttributeTag = GetAttributePreventionTag(Attribute);
		if (!AttributeTag.IsValid())
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Creating a prevent regen effect with an invalid tag reference! Attribute: {3}",
				*UEnum::GetValueAsString(Character->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Character), *UEnum::GetValueAsString(Attribute));
		}

		AttributeTags.Add(AttributeTag);
	}

	
	const FName PreventAttributes = FName(*GetNameSafe(Character) + FString("_PreventMultipleAttributeAccumulation_").Append(FString().FromInt(Attributes.Num())));
	UGameplayEffect* PreventRegenOrBuildup = NewObject<UGameplayEffect>(Character, PreventAttributes);
	if (PreventRegenOrBuildup)
	{
		PreventRegenOrBuildup->DurationPolicy = EGameplayEffectDurationType::HasDuration;
		PreventRegenOrBuildup->DurationMagnitude = FGameplayEffectModifierMagnitude(Duration);
		PreventRegenOrBuildup->StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;

		for (const ECombatAttribute Attribute : Attributes)
		{
			PreventRegenOrBuildup->InheritableGameplayEffectTags.AddTag(GetAttributePreventionTag(Attribute, false));
		}
		
		for (const FGameplayTag& AttributeTag : AttributeTags)
		{
			PreventRegenOrBuildup->InheritableOwnedTagsContainer.AddTag(AttributeTag);
		}

		return PreventRegenOrBuildup;
	}
	
	// FGameplayEffectSpec* StaminaCostSpec = new FGameplayEffectSpec(StaminaCost, MakeEffectContext(Handle, ActorInfo), 1);
	// FGameplayEffectSpecHandle StaminaCostHandle = FGameplayEffectSpecHandle(StaminaCostSpec);
	// ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, StaminaCostHandle);
	return nullptr;
}
#pragma endregion 




#pragma region Utility
const USkeletalMeshSocket* UCombatComponent::GetSkeletalSocket(const FName SocketName) const
{
	const ACharacterBase* OwningCharacter = Cast<ACharacterBase>(GetOwner());
	if (OwningCharacter && OwningCharacter->GetMesh())
	{
		return OwningCharacter->GetMesh()->GetSocketByName(SocketName);
	}

	return nullptr;
}


FName UCombatComponent::GetEquippedSocketName(EArmamentClassification Armament, EEquipSlot EquipSlot) const
{
	const bool bRightHand = IsRightHandedArmament(EquipSlot);
	if (!bRightHand)
	{
		return Socket_LeftHandEquip;
	}
	else
	{
		return Socket_RightHandEquip;
	}
}


EArmamentStance UCombatComponent::GetCurrentStance() const
{
	return CurrentStance;
}


void UCombatComponent::AddCombatAbilityIfValidStance(TMap<EInputAbilities, F_ArmamentAbilityInformation>& Map, const F_ArmamentAbilityInformation& Ability)
{
	if (!Ability.InvalidStances.Contains(GetCurrentStance()))
	{
		Map.Add(Ability.InputId, Ability);
	}
}


FGameplayTag UCombatComponent::GetAttributePreventionTag(const ECombatAttribute Attribute, const bool bStateTag) const
{
	if (Attribute == ECombatAttribute::Health) return bStateTag ? PreventHealthRegen : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::Poise) return bStateTag ? PreventPoiseRegen : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::Stamina) return bStateTag ? PreventStaminaRegen : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::Mana) return bStateTag ? PreventManaRegen : PreventHealthRegenEffect;

	if (Attribute == ECombatAttribute::CurseBuildup) return bStateTag ? PreventCurseBuildup : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::BleedBuildup) return bStateTag ? PreventBleedBuildup : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::PoisonBuildup) return bStateTag ? PreventPoisonBuildup : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::FrostbiteBuildup) return bStateTag ? PreventFrostbiteBuildup : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::MadnessBuildup) return bStateTag ? PreventMadnessBuildup : PreventHealthRegenEffect;
	if (Attribute == ECombatAttribute::SleepBuildup) return bStateTag ? PreventSleepBuildup : PreventHealthRegenEffect;
	return FGameplayTag();
}


void UCombatComponent::SetComboIndex(const int32 Index)
{
	ComboIndex = Index;
}


int32 UCombatComponent::GetComboIndex() const
{
	return ComboIndex;
}


UDataTable* UCombatComponent::GetArmamentMontageTable() const
{
	return MontageInformationTable;
}
#pragma endregion