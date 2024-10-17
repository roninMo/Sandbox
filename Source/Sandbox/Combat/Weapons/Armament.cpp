// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/Weapons/Armament.h"

#include "Sandbox/Data/Enums/SkeletonMappings.h"

#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Data/Enums/HitReacts.h"

DEFINE_LOG_CATEGORY(ArmamentLog);


AArmament::AArmament()
{
	// Network stuff
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	MinNetUpdateFrequency = 33.0f;
	NetUpdateFrequency = 66.0f;
	AActor::SetReplicateMovement(true);
	
	/*
	 * Here's how the collision should be, but this should be created in the subclasses for more adaptability

		// Armament
		ArmamentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmamentMesh"));
		SetRootComponent(ArmamentMesh);

		// Collision
		ArmamentMesh->SetGenerateOverlapEvents(false);
		ArmamentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ArmamentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ArmamentMesh->SetCollisionObjectType(ECC_Armament);
		
		ArmamentMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		ArmamentMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
		ArmamentMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
		ArmamentMesh->SetCollisionResponseToChannel(ECC_Armament, ECollisionResponse::ECR_Overlap);
		ArmamentMesh->SetCollisionResponseToChannel(ECC_Projectile, ECollisionResponse::ECR_Overlap);
		*/

		/*
		// Default Weapon information (The information is now being accessed through data assets!
		Item.Id = FGuid();
		Item.Name = "Potato";
		Item.Description = "This weapon's values haven't been set yet!";
		Item.InteractText = "Press E to pickup";
		Armament.bEquipped = false;
		Armament.BaseDamage = 10;
		Armament.Level = 1;
		Armament.StaminaCost = 4;
		Armament.PoiseDamage = 3;
		Armament.DamageType = EDamageAttribute::D_Normal;
		Armament.BaseAuxiliaryDamage = 0;
		Armament.ArmamentType = EArmamentType::AT_None;
		Armament.ArmamentClass = nullptr;
	*/

}


void AArmament::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AArmament, EquipSlot, COND_Custom, REPNOTIFY_OnChanged);
}


void AArmament::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void AArmament::BeginPlay()
{
	Super::BeginPlay();
	
}




#pragma region Construction and Deconstruction
bool AArmament::ConstructArmament()
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while constructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystemComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system while constructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	// Add armament state and passives
	if (ArmamentInformation.StateInformation.Effect) StateInformationHandle = AbilitySystemComponent->AddGameplayEffect(ArmamentInformation.StateInformation);
	for (const FGameplayEffectInfo& Passive : ArmamentInformation.Passives)
	{
		PassiveHandles.Add(AbilitySystemComponent->AddGameplayEffect(Passive));
	}
	
	// Add armament abilities
	for (const FGameplayAbilityInfo& ArmamentAbility : ArmamentInformation.Abilities)
	{
		AbilityHandles.Add(AbilitySystemComponent->AddAbility(FGameplayAbilityInfo(ArmamentAbility.Ability.Get(), ArmamentAbility.Level, ArmamentAbility.InputId)));
	}
	
	return true;
}


bool AArmament::DeconstructArmament()
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while deconstructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystemComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system while deconstructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}

	// Remove armament state, passives, and abilities
	AbilitySystemComponent->RemoveGameplayEffect(StateInformationHandle, 1);
	StateInformationHandle.Invalidate();

	// Remove the armament's passives
	for (const FActiveGameplayEffectHandle& PassiveHandle : PassiveHandles)
	{
		AbilitySystemComponent->RemoveGameplayEffect(PassiveHandle, 1);
	}
	PassiveHandles.Empty();

	// Remove the armament's abilities
	AbilitySystemComponent->RemoveGameplayAbilities(AbilityHandles);
	AbilityHandles.Empty();
	
	// Handle any cleanup and information specific to the armament

	return true;
}


void AArmament::OnRep_Item()
{
	Super::OnRep_Item();

	// Retrieve the armament's information
	if (!IsValidArmanent())
	{
		RetrieveArmamentInformationOnClient();
	}
}


void AArmament::OnRep_CreatedArmament()
{
	// Retrieve the armament's information
	if (!IsValidArmanent())
	{
		RetrieveArmamentInformationOnClient();
	}

	// Client notification for when the armament has been created
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} tried to add the armament information on the client after creation and failed!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}
	
	CombatComponent->OnEquippedArmament.Broadcast(this, GetEquipSlot());
}

void AArmament::RetrieveArmamentInformationOnClient()
{
	if (!GetOwner())
	{
		// Initial replication happens before the component retrieves the owner, and causes problems. // TODO: add logging that checks against GetOwner() being valid, and handles the extra arguments
		return;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while retrieving the combat component!!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}
	
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} tried to add the armament information on the client after creation and failed!",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return;
	}

	// Item information is universal, just retrieve the inventory id
	FGuid Id = CombatComponent->GetArmamentInventoryInformation(EquipSlot).Id;
	if (Id.IsValid())
	{
		// Execute_SetItem(this, CombatComponent->GetArmamentInventoryInformation(EquipSlot));
		Execute_SetId(this, Id);
	}

	// We just need the combat information, nothing else really needs to be replicated, and we should already be able to retrieve that from the item information
	if (!Item.ItemName.IsNone())
	{
		SetArmamentInformation(CombatComponent->GetArmamentInformationFromDatabase(Item.ItemName));
		SetArmamentMontagesFromDB(CombatComponent->GetArmamentMontageTable(), Character->GetCharacterSkeletonMapping());
		
		if (!ArmamentInformation.IsValid())
		{
			UE_LOGFMT(CombatComponentLog, Error, "{0}::{1}() {2}  tried to add the armament information on the client after creation and something happened!",
				UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		}
	}
}


bool AArmament::IsValidArmanent()
{
	// Check if montages are valid for this specific valid
	// For melee armaments, check if the overlap component is valid
	return !ArmamentInformation.IsValid();
}

#pragma endregion 




#pragma region Montages
void AArmament::SetArmamentMontagesFromDB(UDataTable* ArmamentMontageDB, ECharacterSkeletonMapping Link)
{
	if (!ArmamentMontageDB || ArmamentInformation.Id.IsNone()) return;
	
	const FString RowContext(TEXT("Armament Montage Information Context"));
	if (const F_Table_ArmamentMontages* Data = ArmamentMontageDB->FindRow<F_Table_ArmamentMontages>(ArmamentInformation.Id, RowContext))
	{
		const F_ArmamentMeleeMontages& MeleeMontages = Data->ArmamentMontages.MeleeMontages;

		// Montages
		Montages.Empty();
		for (auto &[Name, MontageMap] : Data->ArmamentMontages.Montages)
		{
			if (MontageMap.MontageMappings.Contains(Link)) Montages.Add(Name, MontageMap.MontageMappings[Link]);
		}

		// One hand montages
		MeleeMontages_OneHand.Empty();
		for (auto &[AttackPattern, MontageMap] : MeleeMontages.OneHandMontages)
		{
			if (MontageMap.Montage.MontageMappings.Contains(Link))
			{
				// Attack pattern
				F_ArmamentComboInformation MeleeMontageInfo;
				MeleeMontageInfo.Montage = MontageMap.Montage.MontageMappings[Link];
				MeleeMontageInfo.Combo = MontageMap.Combo;
				MeleeMontages_OneHand.Add(AttackPattern, MeleeMontageInfo);
			}
		}

		// Two hand montages
		MeleeMontages_TwoHand.Empty();
		for (auto &[AttackPattern, MontageMap] : MeleeMontages.TwoHandMontages)
		{
			if (MontageMap.Montage.MontageMappings.Contains(Link))
			{
				// Attack pattern
				F_ArmamentComboInformation MeleeMontageInfo;
				MeleeMontageInfo.Montage = MontageMap.Montage.MontageMappings[Link];
				MeleeMontageInfo.Combo = MontageMap.Combo;
				MeleeMontages_TwoHand.Add(AttackPattern, MeleeMontageInfo);
			}
		}
		
		// Dual wielding montages
		MeleeMontages_DualWield.Empty();
		for (auto &[AttackPattern, MontageMap] : MeleeMontages.DualWieldMontages)
		{
			if (MontageMap.Montage.MontageMappings.Contains(Link))
			{
				// Attack pattern
				F_ArmamentComboInformation MeleeMontageInfo;
				MeleeMontageInfo.Montage = MontageMap.Montage.MontageMappings[Link];
				MeleeMontageInfo.Combo = MontageMap.Combo;
				MeleeMontages_DualWield.Add(AttackPattern, MeleeMontageInfo);
			}
		}
		
	}
	else
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} did not find the armament montages for {3}",
			*UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), ArmamentInformation.Id);
	}
}


UAnimMontage* AArmament::GetCombatMontage(const EInputAbilities AttackPattern)
{
	const UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while retrieving the combat montage!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return nullptr;
	}
	
	const TMap<EInputAbilities, F_ArmamentComboInformation>& CombatMontages = CombatComponent->GetCurrentStance() == EArmamentStance::OneHanding
		|| CombatComponent->GetCurrentStance() == EArmamentStance::TwoWeapons ? MeleeMontages_OneHand
		: CombatComponent->GetCurrentStance() == EArmamentStance::DualWielding ? MeleeMontages_DualWield
		: MeleeMontages_TwoHand;
	
	if (CombatMontages.Contains(AttackPattern))
	{
		return CombatMontages[AttackPattern].Montage;
	}

	return nullptr;
}


const F_ComboAttacks& AArmament::GetComboAttacks(const EInputAbilities AttackPattern) const
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the combat component while retrieving the combo attacks!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return DummyMeleeComboInformation;
	}

	EArmamentStance Stance = CombatComponent->GetCurrentStance();
	if (Stance == EArmamentStance::OneHanding && MeleeMontages_OneHand.Contains(AttackPattern)) return MeleeMontages_OneHand[AttackPattern].Combo;
	if (Stance == EArmamentStance::TwoHanding && MeleeMontages_TwoHand.Contains(AttackPattern)) return MeleeMontages_TwoHand[AttackPattern].Combo;
	if (Stance == EArmamentStance::TwoWeapons && MeleeMontages_OneHand.Contains(AttackPattern)) return MeleeMontages_OneHand[AttackPattern].Combo;
	if (Stance == EArmamentStance::DualWielding && MeleeMontages_DualWield.Contains(AttackPattern)) return MeleeMontages_DualWield[AttackPattern].Combo;
	return DummyMeleeComboInformation;
}


UAnimMontage* AArmament::GetMontage(FName Montage)
{
	if (Montages.Contains(Montage))
	{
		return Montages[Montage];
	}

	return nullptr;
}
#pragma endregion 




#pragma region Equipping
bool AArmament::SheatheArmament()
{
	return true;
}


bool AArmament::UnsheatheArmament()
{
	return true;
}


bool AArmament::AttachArmamentToSocket(const FName Socket)
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character || !Character->GetMesh())
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while attaching the armament to an equip slot!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		return false;
	}
	
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(Socket);
	if (!HandSocket)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} did not find the armament's equip socket ({2})!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()), Socket);
		return false;
	}
	
	return HandSocket->AttachActor(this, Character->GetMesh());
}


FName AArmament::GetHolsterSocketName() const
{
	const UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent) return FName();
	
	const bool bRightHand = CombatComponent->IsRightHandedArmament(EquipSlot);
	if (!bRightHand) return LeftHandHolsterSocket;
	return RightHandHolsterSocket;
}


FName AArmament::GetSheathedSocketName() const
{
	const UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent) return FName();
	
	const bool bRightHand = CombatComponent->IsRightHandedArmament(EquipSlot);
	if (!bRightHand) return LeftHandSheatheSocket;
	return RightHandSheatheSocket;
}
#pragma endregion 




#pragma region Utility
const F_ArmamentInformation& AArmament::GetArmamentInformation() const
{
	return ArmamentInformation;
}


FName AArmament::GetArmamentId() const
{
	return ArmamentInformation.Id; // Should be the same as the Item Id
}


EArmamentClassification AArmament::GetClassification() const
{
	return ArmamentInformation.Classification;
}


EDamageInformationSource AArmament::GetDamageCalculation() const
{
	return ArmamentInformation.DamageCalculations;
}


EEquipRestrictions AArmament::GetEquipRestrictions() const
{
	return ArmamentInformation.EquipRestrictions;
}


const TArray<F_ArmamentAbilityInformation>& AArmament::GetMeleeAbilities() const
{
	return ArmamentInformation.MeleeAbilities;
}


const TArray<FGameplayAbilityInfo>& AArmament::GetAbilities() const
{
	return ArmamentInformation.Abilities;
}


const TArray<FGameplayEffectInfo>& AArmament::GetPassives() const
{
	return ArmamentInformation.Passives;
}


FGameplayEffectInfo AArmament::GetStateInformation() const
{
	return ArmamentInformation.StateInformation;
}


const TMap<FGameplayAttribute, float>& AArmament::GetBaseDamageStats() const
{
	return ArmamentInformation.BaseDamageStats;
}

EHitStun AArmament::GetHitStun(EInputAbilities AttackPattern, float PoiseDamage) const
{
	if (PoiseDamage <= 0.0) return EHitStun::None;
	if (PoiseDamage <= 4) return EHitStun::VeryShort;
	if (PoiseDamage <= 10) return EHitStun::Short;
	if (PoiseDamage <= 20) return EHitStun::Medium;
	if (PoiseDamage <= 30) return EHitStun::Long;
	return EHitStun::FacePlant;
}


USkeletalMeshComponent* AArmament::GetArmamentMesh() const
{
	return ArmamentMesh;
}


EEquipSlot AArmament::GetEquipSlot() const
{
	return EquipSlot;
}


EEquipStatus AArmament::GetEquipStatus() const
{
	return EquipStatus;
}


void AArmament::SetEquipStatus(const EEquipStatus Status)
{
	EquipStatus = Status;
}


TArray<UPrimitiveComponent*> AArmament::GetArmamentHitboxes() const
{
	return {};
}


FVector AArmament::GetCenterLocation() const
{
	if (!ArmamentMesh)
	{
		return GetActorLocation();
	}

	return ArmamentMesh->GetSocketLocation(WeaponCenterLocationSocket);
}


void AArmament::SetOwnerNoSee(const bool bHide)
{
	if (ArmamentMesh)
	{
		ArmamentMesh->SetOwnerNoSee(bHide);
	}
}


void AArmament::SetArmamentInformation(const F_ArmamentInformation& Information)
{
	if (!Information.IsValid()) return;
	ArmamentInformation = Information;
}


void AArmament::SetArmamentEquipSlot(EEquipSlot Slot)
{
	EquipSlot = Slot;
}


UCombatComponent* AArmament::GetCombatComponent(ACharacterBase* Character) const
{
	Character = Character ? Character : Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		// Construction script sanity check
		if (GetOwner())
		{
			UE_LOGFMT(ArmamentLog, Error, "{0}::{1}() {2} Failed to retrieve the character while retrieving the combat component!!",
				UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		}
		else UE_LOGFMT(ArmamentLog, Error, "{0}() {1} Failed to retrieve the character while retrieving the combat component!!", *FString(__FUNCTION__), *GetNameSafe(GetOwner()));
		
		return nullptr;
	}

	return Character->GetCombatComponent();
}


const TArray<FGameplayAbilitySpecHandle>& AArmament::GetAbilityHandles() const
{
	return AbilityHandles;
}


const TArray<FActiveGameplayEffectHandle>& AArmament::GetPassiveHandles() const
{
	return PassiveHandles;
}


const FActiveGameplayEffectHandle& AArmament::GetStateInformationHandle() const
{
	return StateInformationHandle;
}
#pragma endregion 



