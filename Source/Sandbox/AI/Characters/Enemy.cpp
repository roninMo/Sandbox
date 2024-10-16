// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/AI/Characters/Enemy.h"

#include "Components/WidgetComponent.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"
#include "Sandbox/AI/Controllers/EnemyController.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/GameplayAbilitiyUtilities.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Asc/Information/EnemyEquipmentDataSet.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Hud/Widgets/WidgetBase.h"

DEFINE_LOG_CATEGORY(EnemyLog);


void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnemy, MaxHealth, COND_Custom, REPNOTIFY_Always);
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
	
	StatsBarsWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Stats Bars"));
	StatsBarsWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	StatsBarsWidgetComponent->SetupAttachment(GetRootComponent());
	StatsBarsWidgetComponent->SetIsReplicated(true);
	StatsBarsWidgetComponent->SetHiddenInGame(true);
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}


void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (Player && StatsBarsWidgetComponent)
	{
		FVector WidgetRotationVector = Player->GetCameraLocation() - GetActorLocation();
		FRotator WidgetRotation = FRotator(0, WidgetRotationVector.Rotation().Yaw, 0);
		StatsBarsWidgetComponent->SetWorldRotation(WidgetRotation);
	}
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


void AEnemy::OnInitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::OnInitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(InOwnerActor);
		if (!AbilitySystemComponent) AbilitySystemComponent = UGameplayAbilityUtilities::GetAbilitySystem(InOwnerActor);
	}

	
	
	


	// Update the npc stats
	// UGameplayAbilityUtilities::TryAddAbilitySet(AbilitySystemComponent, AbilitiesAndStats, AbilitiesAndStatsHandle);
	// BaseAbilitySystem->SetCharacterAbilitiesAndPassiveEffects(CharacterAbilities, CharacterInformation.Statuses);
	// for (const TSubclassOf<UGameplayEffect> Effect : InitialEffectsState) ApplyEffectToSelf(Effect);
	// if (CharacterInformation.PrimaryAttributes) ApplyEffectToSelf(CharacterInformation.PrimaryAttributes);
	// if (SecondaryAttributes) ApplyEffectToSelf(SecondaryAttributes);
	//
	// if (BaseAttributeSet) MoveSpeed = BaseAttributeSet->GetMoveSpeed();
	InitCharacterEquipment();

	// Because of how ai characters are replicated, we need to use the character // TODO: Find a good way for handling stats/attribute information for all npc characters
	if (StatsBarsWidgetComponent)
	{
		UWidgetBase* StatsBars = Cast<UWidgetBase>(StatsBarsWidgetComponent->GetWidget());
		if (StatsBars)
		{
			StatsBars->SetWidgetController(this);
		}
	}

	// Update the attribute delegate bindings
	BindAttributeValuesToAscDelegates();
	UpdateAttributeValues();
	
}


void AEnemy::InitCharacterEquipment()
{
	if (!Inventory || !CombatComponent)
	{
		return;
	}

	if (EquipmentAndArmor)
	{
		EquipmentAndArmor->AddToCharacter(AbilitySystemComponent, Inventory, CombatComponent);
	}

	if (AbilitiesAndStats)
	{
		AbilitiesAndStats->AddToAbilitySystem(AbilitySystemComponent, AbilitiesAndStatsHandle);
	}

	// Other combat related things
	// AttackPatterns = CharacterInformation.AttackPatterns;
	// CombatClassification = CharacterInformation.CombatClassification;
}




#pragma region Attributes
void AEnemy::BindAttributeValuesToAscDelegates()
{
	if (!HasAuthority())
	{
		return;
	}

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


	// TODO: nothing is wrong with this, investigate why the functions aren't broadcasting, or more specifically why the replicated values OnReplicated functions aren't handling any logic
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


void AEnemy::UpdateAttributeValues()
{
	if (!HasAuthority())
	{
		return;
	}

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

	CurrentHealth = Attributes->GetHealth();
	CurrentPoise = Attributes->GetPoise();
	CurrentStamina = Attributes->GetStamina();
	CurrentMana = Attributes->GetMana();
	MaxHealth = Attributes->GetMaxHealth();
	MaxPoise = Attributes->GetMaxPoise();
	MaxStamina = Attributes->GetMaxStamina();
	MaxMana = Attributes->GetMaxMana();
	
	OnHealthUpdated.Broadcast(Attributes->GetHealth());
	OnPoiseUpdated.Broadcast(Attributes->GetPoise());
	OnStaminaUpdated.Broadcast(Attributes->GetStamina());
	OnManaUpdated.Broadcast(Attributes->GetMana());
	OnMaxHealthUpdated.Broadcast(Attributes->GetMaxHealth());
	OnMaxPoiseUpdated.Broadcast(Attributes->GetMaxPoise());
	OnMaxStaminaUpdated.Broadcast(Attributes->GetMaxStamina());
	OnMaxManaUpdated.Broadcast(Attributes->GetMaxMana());
}
#pragma endregion 




#pragma region Periphreal Interface Functions
void AEnemy::WithinPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
	Super::WithinPlayerRadiusPeriphery_Implementation(SourceCharacter, PeripheryType);
	if (!StatsBarsWidgetComponent) return;

	ACharacterBase* Character = Cast<ACharacterBase>(SourceCharacter);
	if (Character && Character->IsLocallyControlled())
	{
		Player = Character;
		StatsBarsWidgetComponent->SetHiddenInGame(false);

		// if (AIController)
		// {
		// 	AIController->SetFocus(Player);
		// }

		UWidgetBase* StatsBars = Cast<UWidgetBase>(StatsBarsWidgetComponent->GetWidget());
		if (StatsBars)
		{
			StatsBars->SetWidgetControllerIfNotAlreadySet(this);
			UE_LOGFMT(LogTemp, Log, "{0}: attribute values: health: {1}/{2}, stamina: {3}/{4}, poise: {5}/{6}, mana: {7}/{8}, Asc: {9}",
				*UEnum::GetValueAsString(GetLocalRole()),
				CurrentHealth, MaxHealth,
				CurrentStamina, MaxStamina,
				CurrentPoise, MaxPoise,
				CurrentMana, MaxMana,
				*GetNameSafe(AbilitySystemComponent)
			);
		}
	}
	
	UpdateAttributeValues();
}


void AEnemy::OutsideOfPlayerRadiusPeriphery_Implementation(AActor* SourceCharacter, EPeripheryType PeripheryType)
{
	Super::OutsideOfPlayerRadiusPeriphery_Implementation(SourceCharacter, PeripheryType);
	if (!StatsBarsWidgetComponent) return;

	if (Player == SourceCharacter)
	{
		StatsBarsWidgetComponent->SetHiddenInGame(true);
	}
	else
	{
		ACharacterBase* Character = Cast<ACharacterBase>(SourceCharacter);
		if (Character && Character->IsLocallyControlled())
		{
			StatsBarsWidgetComponent->SetHiddenInGame(true);
		}
	}
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




#pragma region Utility
void AEnemy::GetCombatInformationFromTable(FName EquipmentId)
{
	if (CombatInformationTable)
	{
		const F_Table_NpcCombatInformation* Information = CombatInformationTable->FindRow<F_Table_NpcCombatInformation>(EquipmentId, TEXT("Npc Combat Data Context"), false);
		if (Information)
		{
			EquipmentAndArmor = Information->Equipment;
			AbilitiesAndStats = Information->AttributeAndAbilityData;
		}
		
		// UE_LOGFMT(LogTemp, Error, "{0} did not find {1} within the Npc combat information table!", *GetNameSafe(CombatInformationTable), EquipmentId);
	}
}


UAttributeData* AEnemy::GetAttributeInformationFromTable(FName AttributeId)
{
	if (!AttributeInformationTable)
	{
		UE_LOGFMT(LogTemp, Error, "{0}::{1}() {2}'s attribute data table!", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return nullptr;
	}

	const F_Table_AttributeData* Information = CombatInformationTable->FindRow<F_Table_AttributeData>(AttributeId, TEXT("Npc Attribute Data Context"));
	if (Information)
	{
		return Information->AttributeData;
	}
	
	return nullptr;
}


UEquipmentData* AEnemy::GetEquipmentInformationFromTable(FName EquipmentId)
{
	if (!EquipmentInformationTable)
	{
		UE_LOGFMT(LogTemp, Error, "{0}::{1}() Set {2}'s equipment data table!", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return nullptr;
	}

	const F_Table_EquipmentData* Information = CombatInformationTable->FindRow<F_Table_EquipmentData>(EquipmentId, TEXT("Npc Equipment Data Context"), false);
	if (Information)
	{
		return Information->EquipmentData;
	}
	
	return nullptr;
}


UArmorData* AEnemy::GetArmorInformationFromTable(FName ArmorId)
{
	if (!ArmorInformationTable)
	{
		UE_LOGFMT(LogTemp, Error, "{0}::{1}() Set {2}'s armor data table!", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return nullptr;
	}

	const F_Table_ArmorData* Information = CombatInformationTable->FindRow<F_Table_ArmorData>(ArmorId, TEXT("Npc Armor Data Context"), false);
	if (Information)
	{
		return Information->ArmorData;
	}
	
	return nullptr;
}


UAbilityData* AEnemy::GetAbilityInformationFromTable(FName AbilityId)
{
	if (!AbilityInformationTable)
	{
		UE_LOGFMT(LogTemp, Error, "{0}::{1}() Set {2}'s ability data table!", *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__), *GetName());
		return nullptr;
	}

	const F_Table_AbilityData* Information = AbilityInformationTable->FindRow<F_Table_AbilityData>(AbilityId, TEXT("Npc Ability Data Context"), false);
	if (Information)
	{
		return Information->AbilityData;
	}
	
	return nullptr;
}
#pragma endregion 

