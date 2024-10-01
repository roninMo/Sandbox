// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Combat/Weapons/Armament.h"

#include "Sandbox/Data/Enums/MontageMappings.h"

#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Logging/StructuredLog.h"

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
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the character while constructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return false;
	}

	UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystemComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the ability system while constructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return false;
	}

	// Add armament state and passives
	StateInformationHandle = AbilitySystemComponent->AddGameplayEffect(ArmamentInformation.StateInformation);
	for (const FGameplayEffectMapping& Passive : ArmamentInformation.Passives)
	{
		PassiveHandles.Add(AbilitySystemComponent->AddGameplayEffect(Passive));
	}
	
	// Add armament abilities
	for (const F_ArmamentAbilityInformation& ArmamentAbility : ArmamentInformation.Abilities)
	{
		AbilityHandles.Add(AbilitySystemComponent->AddAbility(FGameplayAbilityMapping(ArmamentAbility.Ability.Get(), ArmamentAbility.Level, ArmamentAbility.InputId)));
	}
	
	// Add armament montages
	UpdateArmamentMontages(Character->GetCharacterToMontageMapping());
	
	return true;
}


bool AArmament::DeconstructArmament()
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	if (!Character)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the character while deconstructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return false;
	}

	UAbilitySystem* AbilitySystemComponent = Character->GetAbilitySystem<UAbilitySystem>();
	if (!AbilitySystemComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the ability system while deconstructing the armament!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
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


bool AArmament::IsValidArmanent()
{
	// Check if montages are valid for this specific valid
	// For melee armaments, check if the overlap component is valid
	return true;
}

#pragma endregion 




#pragma region Montages
bool AArmament::UpdateArmamentMontages(const ECharacterToMontageMapping MontageMapping)
{
	UCombatComponent* CombatComponent = GetCombatComponent();
	if (!CombatComponent)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the combat component while updating the armament montages!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return nullptr;
	}
	
	
	// For melee armaments, retrieve all combo montages, otherwise just retrieve the montage (use "None" for armaments with a single montage for their animations (that aren't combo specific))
	ArmamentMontages.Empty();
	for (F_ArmamentAbilityInformation ArmamentAbility : ArmamentInformation.Abilities)
	{
		UAnimMontage* ArmamentComboMontage = CombatComponent->GetArmamentMontageFromDB(ArmamentInformation.Id, ArmamentAbility.ComboType, MontageMapping);
		if (ArmamentComboMontage)
		{
			ArmamentMontages.Add(ArmamentAbility.ComboType, ArmamentComboMontage);
		}
	}
	
	return true;
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
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the character while attaching the armament to an equip slot!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
		return false;
	}
	
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(Socket);
	if (!HandSocket)
	{
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} did not find the armament's equip socket ({2})!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()), Socket);
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


const TArray<F_ArmamentAbilityInformation>& AArmament::GetAbilities() const
{
	return ArmamentInformation.Abilities;
}


const TArray<FGameplayEffectMapping>& AArmament::GetPassives() const
{
	return ArmamentInformation.Passives;
}


FGameplayEffectMapping AArmament::GetStateInformation() const
{
	return ArmamentInformation.StateInformation;
}


const TMap<FGameplayAttribute, float>& AArmament::GetBaseDamageStats() const
{
	return ArmamentInformation.BaseDamageStats;
}


USkeletalMeshComponent* AArmament::GetArmamentMesh() const
{
	return ArmamentMesh;
}


EEquipSlot AArmament::GetEquipSlot() const
{
	return EquipSlot;
}


TArray<UPrimitiveComponent*> AArmament::GetArmamentHitboxes() const
{
	return {};
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
		UE_LOGFMT(ArmamentLog, Error, "{0}::{1} Failed to retrieve the character while retrieving the combat component!!",
			UEnum::GetValueAsString(GetOwner()->GetLocalRole()), *GetNameSafe(GetOwner()));
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



