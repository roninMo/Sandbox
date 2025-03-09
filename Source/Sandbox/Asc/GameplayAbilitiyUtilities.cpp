// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitiyUtilities.h"

#include "AbilitySystem.h"
#include "InputAction.h"
#include "AbilitySystemGlobals.h"
#include "Characters/AbilitySystemPlayerState.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/AI/Characters/Npc.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/ArmorData.h"
#include "Sandbox/Data/EquipmentData.h"




#pragma region Attributes, Equipment, Abilities, and Armor
bool UGameplayAbilityUtilities::TryAddAttributeData(UAbilitySystemComponent* InAbilitySystemComponent, const UAttributeData* AttributeData, FAttributeDataHandle& OutAttributeDataHandle, bool bPrintErrorMessages)
{
	check(InAbilitySystemComponent);
	if (!AttributeData)
	{
		return false;
	}

	return AttributeData->AddAttributesToCharacter(InAbilitySystemComponent, OutAttributeDataHandle);
}


bool UGameplayAbilityUtilities::TryAddEquipmentData(UAbilitySystemComponent* InAbilitySystemComponent, const UEquipmentData* EquipmentData, bool bPrintErrorMessages)
{
	check(InAbilitySystemComponent);
	if (!EquipmentData)
	{
		return false;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(InAbilitySystemComponent->GetAvatarActor());
	if (!Character)
	{
		UE_LOGFMT(LogTemp, Error, "{0} tried to add equipment when the character wasn't valid! Equipment: {1}", *GetNameSafe(InAbilitySystemComponent), *GetNameSafe(EquipmentData));
		return false;
	}

	return EquipmentData->AddToCharacter(Character);
}


bool UGameplayAbilityUtilities::TryAddAbilityData(UAbilitySystemComponent* InAbilitySystemComponent, const UAbilityData* AbilityData, FAbilityDataHandle& OutAbilityDataHandle, bool bPrintErrorMessages)
{
	check(InAbilitySystemComponent);
	if (!AbilityData)
	{
		return false;
	}

	return AbilityData->AddAbilityDataToCharacter(InAbilitySystemComponent, OutAbilityDataHandle);
}


bool UGameplayAbilityUtilities::TryAddArmorData(UAbilitySystemComponent* InAbilitySystemComponent, const UArmorData* ArmorData, bool bPrintErrorMessages)
{
	check(InAbilitySystemComponent);
	if (!ArmorData)
	{
		return false;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(InAbilitySystemComponent->GetAvatarActor());
	if (!Character)
	{
		UE_LOGFMT(LogTemp, Error, "{0} tried to add armor when the character wasn't valid! Armor: {1}", *GetNameSafe(InAbilitySystemComponent), *GetNameSafe(ArmorData));
		return false;
	}

	return ArmorData->AddToCharacter(Character);
}
#pragma endregion 




#pragma region Ability System Component Functions
UAbilitySystem* UGameplayAbilityUtilities::GetAbilitySystem(const AActor* Actor)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	// Players with ability systems on the player state
	if (!ASC && Cast<APawn>(Actor))
	{
		const APawn* Pawn = Cast<APawn>(Actor);
		AAbilitySystemPlayerState* PS = Cast<AAbilitySystemPlayerState>(Pawn->GetPlayerState());
		ASC = PS ? PS->GetAbilitySystemComponent() : ASC;
	}

	// AI characters
	if (!ASC && Cast<ANpc>(Actor))
	{
		const ANpc* Npc = Cast<ANpc>(Actor);
		ASC = Npc->GetAbilitySystem<UAbilitySystem>();
	}

	if (!ASC)
	{
		const ANpc* NpcCharacter = Cast<ANpc>(Actor);
		if (NpcCharacter && !NpcCharacter->HasAuthority())
		{
			return nullptr;
		}

		UE_LOGFMT(AbilityLog, Error, "{0}::{1} Did not find {2}'s Ability System!", *UEnum::GetValueAsString(Actor->GetLocalRole()), *FString(__FUNCTION__), *GetNameSafe(Actor));
		return nullptr;
	}

	if (UAbilitySystem* Asc = Cast<UAbilitySystem>(ASC))
	{
		return Asc;
	}

	UE_LOGFMT(AbilityLog, Warning, "ASC {0} from {1} Owner is not of UAbilitySystem type.", *GetNameSafe(ASC), *GetNameSafe(Actor));
	return nullptr;
}


void UGameplayAbilityUtilities::GetAllAttributes(TSubclassOf<UAttributeSet> AttributeSetClass, TArray<FGameplayAttribute>& OutAttributes)
{
	const UClass* Class = AttributeSetClass.Get();
	for (TFieldIterator<FProperty> It(Class); It; ++It)
	{
		if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(*It))
		{
			OutAttributes.Push(FGameplayAttribute(FloatProperty));
		}
		else if (FGameplayAttribute::IsGameplayAttributeDataProperty(*It))
		{
			OutAttributes.Push(FGameplayAttribute(*It));
		}
	}
}


void UGameplayAbilityUtilities::TryAddAbility(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayAbilityInfo& InAbilityMapping, FGameplayAbilitySpecHandle& OutAbilityHandle,
	FGameplayAbilitySpec& OutAbilitySpec)
{
	check(AbilitySystemComponent);
	
	if (!InAbilityMapping.Ability)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}'s Failed to Add Ability \"{2}\" because class is null",
			*UEnum::GetValueAsString(AbilitySystemComponent->GetOwnerActor()->GetLocalRole()),
			*GetNameSafe(AbilitySystemComponent->GetOwnerActor()), *InAbilityMapping.Ability->GetName()
		);
		return;
	}
	
	const TSubclassOf<UGameplayAbility> Ability = InAbilityMapping.Ability;
	check(Ability);
	
	UAbilitySystem* ASC = Cast<UAbilitySystem>(AbilitySystemComponent);
	if (!ASC)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}'s Failed to Add Ability \"{2}\" because ASC \"{3}\" is not a valid UAbilitySystem",
			*UEnum::GetValueAsString(AbilitySystemComponent->GetOwnerActor()->GetLocalRole()),
			*GetNameSafe(AbilitySystemComponent->GetOwnerActor()), *GetNameSafe(Ability), *GetNameSafe(AbilitySystemComponent)
		);
		return;
	}

	// Add the ability 
	OutAbilityHandle = ASC->AddAbility(InAbilityMapping);

	// Only use this function if you need a reference to an ability's specifications
	OutAbilitySpec = ASC->GetAbilitySpec(Ability);

	
	/** Ability system behavior for different states when the abilities are granted
	 * 
		OutAbilitySpec = ASC->BuildAbilitySpecFromClass(Ability, InAbilityMapping.Level, static_cast<int32>(InAbilityMapping.InputId));
		
		// Try to grant the ability first
		if (ASC->IsOwnerActorAuthoritative())
		{
			// Only Grant abilities on authority, and only if we should (ability not granted yet or wants reset on spawn)
			if (!IsAbilityGranted(ASC, Ability, InAbilityMapping.Level))
			{
				UE_LOGFMT(AbilityLog, Verbose, "Authority, Grant Ability ({0})", *Ability->GetName());
				OutAbilityHandle = ASC->GiveAbility(OutAbilitySpec);
			}
			else
			{
				// In case granting is prevented because of ability already existing, return the existing handle
				const FGameplayAbilitySpec* ExistingAbilitySpec = ASC->FindAbilitySpecFromClass(Ability);
				if (ExistingAbilitySpec)
				{
					OutAbilityHandle = ExistingAbilitySpec->Handle;
				}
			}
		}
		else
		{
			// For clients, try to get ability spec and update handle used later on for input binding
			const FGameplayAbilitySpec* ExistingAbilitySpec = ASC->FindAbilitySpecFromClass(Ability);
			if (ExistingAbilitySpec)
			{
				OutAbilityHandle = ExistingAbilitySpec->Handle;
			}
			
			UE_LOGFMT(AbilityLog, Verbose, "{0}::{1}'s AddActorAbilities: Not Authority, try to find ability handle from spec: {2}",
				*UEnum::GetValueAsString(AbilitySystemComponent->GetOwnerActor()->GetLocalRole()), *GetNameSafe(AbilitySystemComponent->GetOwnerActor()), *OutAbilityHandle.ToString()
			);
		}
	*/
}


void UGameplayAbilityUtilities::TryAddAttributes(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayAttributeInfo& InAttributeSetMapping, UAttributeSet*& OutAttributeSet)
{
	check(AbilitySystemComponent);
	
	AActor* OwnerActor = AbilitySystemComponent->GetOwnerActor();
	if (!IsValid(OwnerActor))
	{
		UE_LOGFMT(AbilityLog, Error, "{0}() {1}'s Ability System Component owner actor is not valid", *FString(__FUNCTION__), *GetNameSafe(AbilitySystemComponent));
		return;
	}
	
	const TSubclassOf<UAttributeSet> AttributeSetType = InAttributeSetMapping.AttributeSet.LoadSynchronous();
	if (!AttributeSetType)
	{
		UE_LOGFMT(AbilityLog, Error, "{0}::{1}'s AttributeSet class is invalid",
			*UEnum::GetValueAsString(OwnerActor->GetLocalRole()), *GetNameSafe(OwnerActor)
		);
		return;
	}
	
	// Prevent adding the same attribute set multiple times (if already registered by another GF or on Actor ASC directly)
	if (UAttributeSet* AttributeSet = GetAttributeSet(AbilitySystemComponent, AttributeSetType))
	{
		OutAttributeSet = AttributeSet;
		// UE_LOGFMT(AbilityLog, Warning, "{0}::{1}'s AttributeSet has already been created for {2}",
		// 	*UEnum::GetValueAsString(OwnerActor->GetLocalRole()), *AttributeSetType->GetName(), *OwnerActor->GetName()
		// );
	}
	
	OutAttributeSet = NewObject<UAttributeSet>(OwnerActor, AttributeSetType);
	if (!InAttributeSetMapping.InitializationData.IsNull())
	{
		const UDataTable* InitData = InAttributeSetMapping.InitializationData.LoadSynchronous();
		if (InitData)
		{
			OutAttributeSet->InitFromMetaDataTable(InitData);
		}
	}

	
	AbilitySystemComponent->AddAttributeSetSubobject(OutAttributeSet);
}

void UGameplayAbilityUtilities::TryAddAttributes(UAbilitySystemComponent* AbilitySystemComponent,
	const TSubclassOf<UGameplayEffect> InEffectType, const float InLevel,
	TArray<FActiveGameplayEffectHandle>& OutEffectHandles)
{
	TryAddGameplayEffect(AbilitySystemComponent, InEffectType, InLevel, OutEffectHandles);
}


void UGameplayAbilityUtilities::TryAddGameplayEffect(UAbilitySystemComponent* AbilitySystemComponent,
                                                     const TSubclassOf<UGameplayEffect> InEffectType, const float InLevel,
                                                     TArray<FActiveGameplayEffectHandle>& OutEffectHandles)
{
	check(AbilitySystemComponent);
	
	if (!AbilitySystemComponent->IsOwnerActorAuthoritative())
	{
		return;
	}
	
	if (!InEffectType)
	{
		UE_LOGFMT(AbilityLog, Warning, "{0}::{1}'s Trying to apply an effect from an invalid class",
			*UEnum::GetValueAsString(AbilitySystemComponent->GetOwnerActor()->GetLocalRole()), *AbilitySystemComponent->GetOwnerActor()->GetName()
		);
		return;
	}
	
	// Don't apply if ASC already has it
	TArray<FActiveGameplayEffectHandle> ActiveEffects;
	if (HasGameplayEffectApplied(AbilitySystemComponent, InEffectType, ActiveEffects))
	{
		// Return the list of found effects already applied as part of this ability set handle (would be removed when set is removed)
		OutEffectHandles = MoveTemp(ActiveEffects);
		return;
	}
	
	const FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(InEffectType, InLevel, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
		OutEffectHandles.Add(MoveTemp(EffectHandle));
	}
}


UAttributeSet* UGameplayAbilityUtilities::GetAttributeSet(const UAbilitySystemComponent* InASC, const TSubclassOf<UAttributeSet> InAttributeSet)
{
	check(InASC);
	
	for (UAttributeSet* SpawnedAttribute : InASC->GetSpawnedAttributes())
	{
		if (SpawnedAttribute && SpawnedAttribute->IsA(InAttributeSet))
		{
			return SpawnedAttribute;
		}
	}

	return nullptr;
}

bool UGameplayAbilityUtilities::HasGameplayEffectApplied(const UAbilitySystemComponent* InASC,
	const TSubclassOf<UGameplayEffect>& InEffectType, TArray<FActiveGameplayEffectHandle>& OutEffectHandles)
{
	check(InASC);

	// Note: Now thinking about the case of stacking GEs, we might want to always allow a GE to apply
	
	// For now, don't allow sets to add multiple instance of the same effect, even if applied from different ability sets or subsequent add calls of the same set
	// This is mostly to stay consistent with the behavior of abilities / attributes and tags, where if it were already applied, those are not applied or granted again

	FGameplayEffectQuery Query;
	Query.EffectDefinition = InEffectType;
	OutEffectHandles = InASC->GetActiveEffects(Query);
	return !OutEffectHandles.IsEmpty();
}

bool UGameplayAbilityUtilities::IsAbilityGranted(const UAbilitySystemComponent* InASC,
	TSubclassOf<UGameplayAbility> InAbility, const int32 InLevel)
{
	check(InASC);
	
	// Check for activatable abilities, if one is matching the given Ability type, prevent re adding again
	TArray<FGameplayAbilitySpec> AbilitySpecs = InASC->GetActivatableAbilities();
	for (const FGameplayAbilitySpec& ActivatableAbility : AbilitySpecs)
	{
		if (!ActivatableAbility.Ability)
		{
			continue;
		}

		if (ActivatableAbility.Ability->GetClass() == InAbility && ActivatableAbility.Level == InLevel)
		{
			return true;
		}
	}

	return false;
}
#pragma endregion 




#pragma region Gameplay Cues
void UGameplayAbilityUtilities::ExecuteGameplayCueForActor(AActor* Actor, FGameplayTag GameplayCueTag, FGameplayEffectContextHandle Context)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ExecuteGameplayCue(GameplayCueTag, Context);
	}
}


void UGameplayAbilityUtilities::ExecuteGameplayCueWithParams(AActor* Actor, FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ExecuteGameplayCue(GameplayCueTag, GameplayCueParameters);
	}
}


void UGameplayAbilityUtilities::AddGameplayCue(AActor* Actor, FGameplayTag GameplayCueTag, FGameplayEffectContextHandle Context)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddGameplayCue(GameplayCueTag, Context);
	}
}


void UGameplayAbilityUtilities::AddGameplayCueWithParams(AActor* Actor, FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameter)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddGameplayCue(GameplayCueTag, GameplayCueParameter);
	}
}


void UGameplayAbilityUtilities::RemoveGameplayCue(AActor* Actor, FGameplayTag GameplayCueTag)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveGameplayCue(GameplayCueTag);
	}
}


void UGameplayAbilityUtilities::RemoveAllGameplayCues(AActor* Actor)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveAllGameplayCues();
	}
}
#pragma endregion




#pragma region Utility
void UGameplayAbilityUtilities::AddLooseGameplayTagsUnique(UAbilitySystemComponent* InASC,
	const FGameplayTagContainer& InTags, const bool bReplicated)
{
	check(InASC);

	FGameplayTagContainer ExistingTags;
	InASC->GetOwnedGameplayTags(ExistingTags);

	TArray<FGameplayTag> TagsToCheck;
	InTags.GetGameplayTagArray(TagsToCheck);

	FGameplayTagContainer TagsToAdd;
	
	// Check existing tags and build the container to add
	for (const FGameplayTag& Tag : TagsToCheck)
	{
		if (ExistingTags.HasTagExact(Tag))
		{
			// Tag already present
			continue;
		}

		TagsToAdd.AddTag(Tag);
	}

	// Add the resulting tags container, with all tags that were not owned by the ASC yet
	InASC->AddLooseGameplayTags(TagsToAdd);
	if (bReplicated)
	{
		InASC->AddReplicatedLooseGameplayTags(TagsToAdd);
	}
}


void UGameplayAbilityUtilities::RemoveLooseGameplayTagsUnique(UAbilitySystemComponent* InASC,
	const FGameplayTagContainer& InTags, const bool bReplicated)
{
	check(InASC);

	FGameplayTagContainer ExistingTags;
	InASC->GetOwnedGameplayTags(ExistingTags);

	TArray<FGameplayTag> TagsToCheck;
	InTags.GetGameplayTagArray(TagsToCheck);

	FGameplayTagContainer TagsToRemove;
	
	// Check existing tags and build the container to add
	for (const FGameplayTag& Tag : TagsToCheck)
	{
		if (ExistingTags.HasTagExact(Tag))
		{
			// Tag is present
			TagsToRemove.AddTag(Tag);
		}
	}

	// Add the resulting tags container, with all tags that were not owned by the ASC yet
	InASC->RemoveLooseGameplayTags(TagsToRemove);
	if (bReplicated)
	{
		InASC->RemoveReplicatedLooseGameplayTags(TagsToRemove);
	}
}


void UGameplayAbilityUtilities::HandleOnGiveAbility(FGameplayAbilitySpec& InAbilitySpec,
	TWeakObjectPtr<UInputComponent> InInputComponent, TWeakObjectPtr<UInputAction> InInputAction,
	const ETriggerEvent InTriggerEvent, FGameplayAbilitySpec InNewAbilitySpec)
{
	UE_LOGFMT(
		AbilityLog,
		Verbose,
		"Handle: {0}, Ability: {1}, Input: {2} (TriggerEvent: {3}) - (InputComponent: {4})",
		*InAbilitySpec.Handle.ToString(),
		*GetNameSafe(InAbilitySpec.Ability),
		*GetNameSafe(InInputAction.Get()),
		*UEnum::GetValueAsName(InTriggerEvent).ToString(),
		*GetNameSafe(InInputComponent.Get())
	);
}
#pragma endregion

