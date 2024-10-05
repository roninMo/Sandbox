// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/CombatComponent.h"

#include "Sandbox/Data/Enums/EquipSlot.h"
#include "Sandbox/Data/Enums/ArmorTypes.h"
#include "Sandbox/Data/Structs/ArmamentInformation.h"

#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Asc/Attributes/AttributeLogic.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Weapons/Armament.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(CombatComponentLog);
// TODO: Custom logging to remove the extra message logic for clarification


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
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


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UE_LOGFMT(CombatComponentLog, Log, "{0}::{1}() {2}'s combo index: {2}", *UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), ComboIndex);
}


void UCombatComponent::AddArmamentToEquipSlot(const F_Item& ArmamentInventoryInformation, const EEquipSlot EquipSlot)
{
	if (!ArmamentInventoryInformation.IsValid()) return;
	
	if (EquipSlot == EEquipSlot::LeftHandSlotOne) LeftHandEquipSlot_One = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::LeftHandSlotTwo) LeftHandEquipSlot_Two = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::LeftHandSlotThree) LeftHandEquipSlot_Three = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotOne) RightHandEquipSlot_One = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotTwo) RightHandEquipSlot_Two = ArmamentInventoryInformation;
	else if (EquipSlot == EEquipSlot::RightHandSlotThree) RightHandEquipSlot_Three = ArmamentInventoryInformation;
}


void UCombatComponent::RemoveArmamentFromEquipSlot(const EEquipSlot EquipSlot)
{
	if (EquipSlot == EEquipSlot::LeftHandSlotOne) LeftHandEquipSlot_One = F_Item();
	else if (EquipSlot == EEquipSlot::LeftHandSlotTwo) LeftHandEquipSlot_Two = F_Item();
	else if (EquipSlot == EEquipSlot::LeftHandSlotThree) LeftHandEquipSlot_Three = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotOne) RightHandEquipSlot_One = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotTwo) RightHandEquipSlot_Two = F_Item();
	else if (EquipSlot == EEquipSlot::RightHandSlotThree) RightHandEquipSlot_Three = F_Item();
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
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Only create the armament on the server!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
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
	if (!ArmamentItemData.ActualClass)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve valid armament class while creating the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
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
		const TArray<F_ArmamentAbilityInformation>& Abilities = PrimaryArmament->GetCombatAbilities();
		for (const F_ArmamentAbilityInformation& CombatAbility : Abilities)
		{
			if (EInputAbilities::PrimaryAttack == CombatAbility.InputId) CombatAbilities.Add(EInputAbilities::PrimaryAttack, CombatAbility);
			if (EInputAbilities::StrongAttack == CombatAbility.InputId) CombatAbilities.Add(EInputAbilities::StrongAttack, CombatAbility);
			if (EInputAbilities::SpecialAttack == CombatAbility.InputId) CombatAbilities.Add(EInputAbilities::SpecialAttack, CombatAbility);

			// Only use the secondary attack (Block or alt primary attack) for two handing or dual wielding
			if (CurrentStance == EArmamentStance::DualWielding || CurrentStance == EArmamentStance::TwoWeapons || CurrentStance == EArmamentStance::TwoHandingPrimary)
			{
				if (EInputAbilities::SecondaryAttack == CombatAbility.InputId)
				{
					CombatAbilities.Add(CombatAbility.InputId, CombatAbility);
				}
			}
		}
	}

	if (SecondaryArmament)
	{
		// Retrieve the secondary weapon's combat abilities for this armament
		const TArray<F_ArmamentAbilityInformation>& Abilities = SecondaryArmament->GetCombatAbilities();
		for (const F_ArmamentAbilityInformation& CombatAbility : Abilities)
		{
			// If we're two handing the weapon, retrieve every ability, otherwise retrieve the offhand ability
			if (CurrentStance == EArmamentStance::TwoHandingSecondary)
			{
				CombatAbilities.Add(CombatAbility.InputId, CombatAbility);
			}
			else if (CurrentStance == EArmamentStance::OneHanding)
			{
				if (EInputAbilities::SecondaryAttack == CombatAbility.InputId)
				{
					CombatAbilities.Add(CombatAbility.InputId, CombatAbility);
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
		FGuid Id;

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
				
			- TwoHandingSecondary
				- Secondary (Primary, Strong, Special, Secondary)
			
			- TwoHandingPrimary
				- Primary (Primary, Strong, Special, Secondary)

			
		*/
		if (CurrentStance == EArmamentStance::OneHanding || CurrentStance == EArmamentStance::TwoWeapons || CurrentStance == EArmamentStance::DualWielding)
		{
			if (EInputAbilities::SecondaryAttack == AttackPattern) Id = SecondaryId;
			if (EInputAbilities::PrimaryAttack == AttackPattern) Id = PrimaryId;
			if (EInputAbilities::SpecialAttack == AttackPattern) Id = PrimaryId;
			if (EInputAbilities::StrongAttack == AttackPattern) Id = PrimaryId;
		}
		else if (CurrentStance == EArmamentStance::TwoHandingSecondary) Id = SecondaryId;
		else if (CurrentStance == EArmamentStance::TwoHandingPrimary) Id = PrimaryId;

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

	if (!Character->HasAuthority())
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Only remove the armor on the server!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

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
	
	// Remove the armor from the character
	Character->SetArmorMesh(ArmorSlot, nullptr);


	
	// Broadcast the event
	F_Item Information = GetArmorItemInformation(ArmorSlot);
	OnUnequippedArmor.Broadcast(GetArmorItemInformation(ArmorSlot), ArmorAbilities[ArmorSlot], ArmorSlot);

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

	if (!Character->HasAuthority())
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Only create the armor on the server!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystemComponent)
	{
		UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system while creating the armor!",
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

UDataTable* UCombatComponent::GetArmamentMontageTable() const
{
	return MontageInformationTable;
}


void UCombatComponent::SetComboIndex(const int32 Index)
{
	ComboIndex = Index;
}


int32 UCombatComponent::GetComboIndex() const
{
	return ComboIndex;
}
