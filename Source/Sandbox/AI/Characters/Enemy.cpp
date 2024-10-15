// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Characters/Enemy.h"

#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"
#include "Sandbox/AI/Controllers/EnemyController.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Characters/Components/Inventory/InventoryComponent.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Data/Enums/EquipSlot.h"

DEFINE_LOG_CATEGORY(EnemyLog);


void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, MaxHealth, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, MaxMana, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, MaxPoise, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, MaxStamina, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, CurrentHealth, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, CurrentMana, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, CurrentPoise, COND_Custom, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, CurrentStamina, COND_Custom, REPNOTIFY_OnChanged);
}


AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Combat
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	CombatComponent->SetIsReplicated(false);
	
	// StatsBarsWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Stats Bars"));
	// StatsBarsWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// StatsBarsWidgetComponent->SetupAttachment(GetRootComponent());
	// StatsBarsWidgetComponent->SetIsReplicated(true);
	// StatsBarsWidgetComponent->SetHiddenInGame(true);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// if (Player && StatsBarsWidgetComponent)
	// {
	// 	FVector WidgetRotationVector = Player->GetCameraLocation() - GetActorLocation();
	// 	FRotator WidgetRotation = FRotator(0, WidgetRotationVector.Rotation().Yaw, 0);
	// 	StatsBarsWidgetComponent->SetWorldRotation(WidgetRotation);
	// }
}


void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	AEnemyController* EnemyController = Cast<AEnemyController>(NewController);
	AbilitySystemComponent = EnemyController ? EnemyController->GetAbilitySystem<UAbilitySystem>() : UGameplayAbilityUtilities::GetAbilitySystem(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(NewController, this);
		OnInitAbilityActorInfo(NewController, this);
	}
}


void AEnemy::BindAttributeValuesToAscDelegates()
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(Controller);
		if (!AbilitySystemComponent)
		{
			UE_LOGFMT(EnemyLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system component while binding the enemy's attributes!",
				*UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Controller));
			return;
		}
	}

	const UMMOAttributeSet* Attributes = Cast<UMMOAttributeSet>(AbilitySystemComponent->GetAttributeSet(UMMOAttributeSet::StaticClass()));
	if (!Attributes)
	{
		UE_LOGFMT(EnemyLog, Error, "{0}::{1}() {2} Failed to retrieve the attributes while binding the enemy's attributes!",
			*UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Controller));
		return;
	}

	// TODO: Refactor this into a function that uses references to prevent DRY
	// Bind to the attribute on change delegates (these delegates don't run on the client, and we're using delegates so that we don't have to update the values every frame)

	
	FOnGameplayAttributeValueChange& HealthAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute());
	HealthAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		CurrentHealth = Data.NewValue;
		OnHealthUpdated.Broadcast(Data.NewValue); 
	});
	
	FOnGameplayAttributeValueChange& ManaAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetManaAttribute());
	ManaAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		CurrentMana = Data.NewValue;
		OnManaUpdated.Broadcast(Data.NewValue); 
	});
	
	FOnGameplayAttributeValueChange& PoiseAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetPoiseAttribute());
	PoiseAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		CurrentPoise = Data.NewValue;
		OnPoiseUpdated.Broadcast(Data.NewValue); 
	});
	
	FOnGameplayAttributeValueChange& StaminaAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetStaminaAttribute());
	StaminaAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		CurrentStamina = Data.NewValue;
		OnStaminaUpdated.Broadcast(Data.NewValue); 
	});
	
	
	FOnGameplayAttributeValueChange& MaxHealthAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetMaxHealthAttribute());
	MaxHealthAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		MaxHealth = Data.NewValue;
		OnMaxHealthUpdated.Broadcast(Data.NewValue); 
	});
	
	FOnGameplayAttributeValueChange& MaxManaAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetMaxManaAttribute());
	MaxManaAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		MaxMana = Data.NewValue;
		OnMaxManaUpdated.Broadcast(Data.NewValue); 
	});
	
	FOnGameplayAttributeValueChange& MaxPoiseAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetMaxPoiseAttribute());
	MaxPoiseAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		MaxPoise = Data.NewValue;
		OnMaxPoiseUpdated.Broadcast(Data.NewValue); 
	});
	
	FOnGameplayAttributeValueChange& MaxStaminaAttributeUpdates = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetMaxStaminaAttribute());
	MaxStaminaAttributeUpdates.AddLambda([this](const FOnAttributeChangeData& Data){ 
		MaxStamina = Data.NewValue;
		OnMaxStaminaUpdated.Broadcast(Data.NewValue); 
	});
	
	// UE_LOGFMT(LogTemp, Log, "{0}: attribute values: health: {1}/{2}, stamina: {3}/{4}, poise: {5}/{6}, mana: {7}/{8}, attributeSet: {9}, Asc: {10}",
	// 	*UEnum::GetValueAsString(GetLocalRole()),
	// 	BaseAttributeSet->GetHealth(), BaseAttributeSet->GetMaxHealth(),
	// 	BaseAttributeSet->GetStamina(), BaseAttributeSet->GetMaxStamina(),
	// 	BaseAttributeSet->GetPoise(), BaseAttributeSet->GetMaxPoise(),
	// 	BaseAttributeSet->GetMana(), BaseAttributeSet->GetMaxMana(),
	// 	*GetNameSafe(BaseAttributeSet),
	// 	*GetNameSafe(BaseAbilitySystem)
	// );
}


void AEnemy::OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::OnInitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(InOwnerActor);
		if (!AbilitySystemComponent) AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(InOwnerActor);
	}

	
	
	// Add the character's equipment
	InitCharacterEquipment();
	
	// Update the npc stats
	UGameplayAbilityUtilities::TryAddAbilitySet(AbilitySystemComponent, AbilitiesAndStats, AbilitiesAndStatsHandle);
	// BaseAbilitySystem->SetCharacterAbilitiesAndPassiveEffects(CharacterAbilities, CharacterInformation.Statuses);
	// for (const TSubclassOf<UGameplayEffect> Effect : InitialEffectsState) ApplyEffectToSelf(Effect);
	// if (CharacterInformation.PrimaryAttributes) ApplyEffectToSelf(CharacterInformation.PrimaryAttributes);
	// if (SecondaryAttributes) ApplyEffectToSelf(SecondaryAttributes);
	//
	// if (BaseAttributeSet) MoveSpeed = BaseAttributeSet->GetMoveSpeed();
	
	// Update the attribute delegate bindings
	BindAttributeValuesToAscDelegates();
	UpdateAttributeValues();
	
}


void AEnemy::UpdateAttributeValues()
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(Controller);
		if (!AbilitySystemComponent)
		{
			UE_LOGFMT(EnemyLog, Error, "{0}::{1}() {2} Failed to retrieve the ability system component while updating the enemy's attributes!",
				*UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Controller));
			return;
		}
	}

	const UMMOAttributeSet* Attributes = Cast<UMMOAttributeSet>(AbilitySystemComponent->GetAttributeSet(UMMOAttributeSet::StaticClass()));
	if (!Attributes)
	{
		UE_LOGFMT(EnemyLog, Error, "{0}::{1}() {2} Failed to retrieve the attributes while updating the enemy's attributes!",
			*UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Controller));
		return;
	}

	
	if (CurrentHealth != Attributes->GetHealth())
	{
		OnHealthUpdated.Broadcast(Attributes->GetHealth());
		CurrentHealth = Attributes->GetHealth();
	}
	
	if (CurrentPoise != Attributes->GetPoise())
	{
		OnPoiseUpdated.Broadcast(Attributes->GetPoise());
		CurrentPoise = Attributes->GetPoise();
	}

	if (CurrentStamina != Attributes->GetStamina())
	{
		OnStaminaUpdated.Broadcast(Attributes->GetStamina());
		CurrentStamina = Attributes->GetStamina();
	}

	if (CurrentMana != Attributes->GetMana())
	{
		OnManaUpdated.Broadcast(Attributes->GetMana());
		CurrentMana = Attributes->GetMana();
	}
	
	if (MaxHealth != Attributes->GetMaxHealth())
	{
		OnMaxHealthUpdated.Broadcast(Attributes->GetMaxHealth());
		MaxHealth = Attributes->GetMaxHealth();
	}

	if (MaxPoise != Attributes->GetMaxPoise())
	{
		OnMaxPoiseUpdated.Broadcast(Attributes->GetMaxPoise());
		MaxPoise = Attributes->GetMaxPoise();
	}

	if (MaxStamina != Attributes->GetMaxStamina())
	{
		OnMaxStaminaUpdated.Broadcast(Attributes->GetMaxStamina());
		MaxStamina = Attributes->GetMaxStamina();
	}

	if (MaxMana != Attributes->GetMaxMana())
	{
		OnMaxManaUpdated.Broadcast(Attributes->GetMaxMana());
		MaxMana = Attributes->GetMaxMana();
	}
}



void AEnemy::InitCharacterEquipment()
{
	if (!Inventory || !CombatComponent)
	{
		return;
	}

	// Weapons
	if (CharacterInformation.LeftHandWeapons.Num() > 0)
	{
		F_Item LeftHandWeaponSlotOne;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.LeftHandWeapons[0].ItemName, LeftHandWeaponSlotOne);
		CombatComponent->AddArmamentToEquipSlot(LeftHandWeaponSlotOne, EEquipSlot::LeftHandSlotOne);
	}
	if (CharacterInformation.LeftHandWeapons.Num() > 1)
	{
		F_Item LeftHandWeaponSlotTwo;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.LeftHandWeapons[1].ItemName, LeftHandWeaponSlotTwo);
		CombatComponent->AddArmamentToEquipSlot(LeftHandWeaponSlotTwo, EEquipSlot::LeftHandSlotTwo);
	}
	if (CharacterInformation.LeftHandWeapons.Num() > 2)
	{
		F_Item LeftHandWeaponSlotThree;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.LeftHandWeapons[2].ItemName, LeftHandWeaponSlotThree);
		CombatComponent->AddArmamentToEquipSlot(LeftHandWeaponSlotThree, EEquipSlot::LeftHandSlotThree);
	}
	if (CharacterInformation.RightHandWeapons.Num() > 0)
	{
		F_Item RightHandWeaponSlotOne;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.RightHandWeapons[0].ItemName, RightHandWeaponSlotOne);
		CombatComponent->AddArmamentToEquipSlot(RightHandWeaponSlotOne, EEquipSlot::RightHandSlotOne);
	}
	if (CharacterInformation.RightHandWeapons.Num() > 1)
	{
		F_Item RightHandWeaponSlotTwo;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.RightHandWeapons[1].ItemName, RightHandWeaponSlotTwo);
		CombatComponent->AddArmamentToEquipSlot(RightHandWeaponSlotTwo, EEquipSlot::RightHandSlotTwo);
	}
	if (CharacterInformation.RightHandWeapons.Num() > 2)
	{
		F_Item RightHandWeaponSlotThree;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.RightHandWeapons[2].ItemName, RightHandWeaponSlotThree);
		CombatComponent->AddArmamentToEquipSlot(RightHandWeaponSlotThree, EEquipSlot::RightHandSlotThree);
	}

	// Equipped weapon
	if (CharacterInformation.CurrentlyEquippedActiveWeapon != EEquipSlot::None)
	{
		CombatComponent->CreateArmament(CharacterInformation.CurrentlyEquippedActiveWeapon);
	}

	// TODO: Add safeguards in place for handling adding inventory items
	// Add these items to the inventory also @note this is only being handled here because it's save information
	// TMap<FGuid, F_Item>& InventoryArmaments = Inventory->GetInventory(EItemType::IT_Armament);
	// for (FS_Item WeaponInformation : CharacterInformation.LeftHandWeapons)
	// {
	// 	F_InventoryItem Weapon;
	// 	Inventory->GetItemFromDataTable(Weapon, WeaponInformation.RowName);
	// 	InventoryArmaments.Add(Weapon.Id, Weapon);
	// }

	
	// Armor
	if (CharacterInformation.Gauntlets.IsValid())
	{
		F_Item GauntletsInformation;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.Gauntlets.ItemName, GauntletsInformation);
		CombatComponent->EquipArmor(GauntletsInformation);
	}
	if (CharacterInformation.Leggings.IsValid())
	{
		F_Item LeggingsInformation;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.Leggings.ItemName, LeggingsInformation);
		CombatComponent->EquipArmor(LeggingsInformation);
	}
	if (CharacterInformation.Helm.IsValid())
	{
		F_Item HelmInformation;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.Helm.ItemName, HelmInformation);
		CombatComponent->EquipArmor(HelmInformation);
	}
	if (CharacterInformation.Chest.IsValid())
	{
		F_Item ChestInformation;
		Inventory->Execute_GetDataBaseItem(Inventory, CharacterInformation.Chest.ItemName, ChestInformation);
		CombatComponent->EquipArmor(ChestInformation);
	}
	

	// Other combat related things
	AttackPatterns = CharacterInformation.AttackPatterns;
	// CombatClassification = CharacterInformation.CombatClassification;
}



void AEnemy::RetrieveAttackPatterns()
{
}


#pragma region Utility
TArray<AActor*> AEnemy::GetActorsToIgnore()
{
	return ActorsToIgnore;
}


void AEnemy::SetActorsToIgnore(const TArray<AActor*>& Actors)
{
	ActorsToIgnore = Actors;
}
#pragma endregion 




#pragma region OnRep Values
void AEnemy::OnRep_CurrentHealthUpdated() { OnHealthUpdated.Broadcast(CurrentHealth); }
void AEnemy::OnRep_CurrentManaUpdated() { OnManaUpdated.Broadcast(CurrentMana); }
void AEnemy::OnRep_CurrentPoiseUpdated() { OnPoiseUpdated.Broadcast(CurrentPoise); }
void AEnemy::OnRep_CurrentStaminaUpdated() { OnStaminaUpdated.Broadcast(CurrentStamina); }
void AEnemy::OnRep_MaxHealthUpdated() { OnMaxHealthUpdated.Broadcast(MaxHealth); }
void AEnemy::OnRep_MaxManaUpdated() { OnMaxManaUpdated.Broadcast(MaxMana); }
void AEnemy::OnRep_MaxPoiseUpdated() { OnMaxPoiseUpdated.Broadcast(MaxPoise); }
void AEnemy::OnRep_MaxStaminaUpdated() { OnMaxStaminaUpdated.Broadcast(MaxStamina); }
#pragma endregion 

