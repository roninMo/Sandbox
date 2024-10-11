// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/MMOAttributeLogic.h"

#include "GameplayEffectExtension.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Information/SandboxTags.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/CombatComponent.h"
#include "Sandbox/Combat/Weapons/Armament.h"
#include "Sandbox/Data/Enums/AttributeTypes.h"
#include "Sandbox/Data/Enums/HitDirection.h"
#include "Sandbox/Data/Enums/HitReacts.h"


bool UMMOAttributeLogic::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	/**
	 *  This should apply 'gamewide' rules. Such as clamping Health to MaxHealth or granting +3 health for every point of strength, etc
	 *	PreAttributeModify can return false to 'throw out' this modification.
	 */
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	ClampEvaluatedAttribute(GetManaAttribute(), Data.EvaluatedData, -GetMana(), GetMaxMana() - GetMana());
	ClampEvaluatedAttribute(GetHealthAttribute(), Data.EvaluatedData, -GetHealth(), GetMaxHealth() - GetHealth());
	ClampEvaluatedAttribute(GetStaminaAttribute(), Data.EvaluatedData, -GetStamina(), GetMaxStamina() - GetStamina());

	ClampEvaluatedAttribute(GetPoisonBuildupAttribute(), Data.EvaluatedData, -GetPoisonBuildup(), GetMaxPoisonBuildup() - GetPoisonBuildup());
	ClampEvaluatedAttribute(GetBleedBuildupAttribute(), Data.EvaluatedData, -GetBleedBuildup(), GetMaxBleedBuildup() - GetBleedBuildup());
	ClampEvaluatedAttribute(GetFrostbiteBuildupAttribute(), Data.EvaluatedData, -GetFrostbiteBuildup(), GetMaxFrostbiteBuildup() - GetFrostbiteBuildup());
	ClampEvaluatedAttribute(GetSleepBuildupAttribute(), Data.EvaluatedData, -GetSleepBuildup(), GetMaxSleepBuildup() - GetSleepBuildup());
	ClampEvaluatedAttribute(GetMadnessBuildupAttribute(), Data.EvaluatedData, -GetMadnessBuildup(), GetMaxMadnessBuildup() - GetMadnessBuildup());
	ClampEvaluatedAttribute(GetCurseBuildupAttribute(), Data.EvaluatedData, -GetCurseBuildup(), GetMaxCurseBuildup() - GetCurseBuildup());
	
	return true;
}


void UMMOAttributeLogic::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Any clamping that happens here does not permanently change the modifier on the ASC. It only changes the value returned from querying the modifier.
	// This means anything that recalculates the CurrentValue from all of the modifiers like GameplayEffectExecutionCalculations and ModifierMagnitudeCalculations need to implement clamping again.
	Super::PreAttributeChange(Attribute, NewValue);
}


void UMMOAttributeLogic::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	/**
		Apply buildups / damages / poise damages, then the hit react, and handle any other logic after that

		Handling individual calculations isn't as complex as having everything together, there's a lot more of interesting things and calculations with multiple attributes.
		You just need to clear out the old attributes and help handle some of the basic values for handling damage

			https://fextralife.com/forums/t52683/poise-explanation-general-pvp-tips
			https://www.reddit.com/r/Eldenring/comments/pjemv1/which_poise_system_do_you_prefer_the_most_and/

			
			- Status calculations
				- status buildup
				- Status effect (Take damage / slow / poison)

			- Take damage
				- Hit reaction based on the attack
				- Handle taking damage / dying

			- Poise damage
				- Damage poise
				- Handle poise break / effect for regenerating poise
				- Handle hit reactions
			
			- Any other effects to attributes
				- stamina drain, etc.


			CombatComponent
				- Status buildup
					- Curse -> Death / Montage / etc.
					- Bleed -> Bleed damage / Hit react / gameplay cue
					- Poison -> Poise damage / drain / gameplay cue
					- Frostbite -> Frostbite damage / Hit react / Damage received debuff / gameplay cue
					- Madness -> Madness damage / Reaction / gameplay cue
					- Sleep -> Sleep debuff / Montage / gameplay cue

				- Take damage
					- Hit reactions during poise break
						- variations based on weapon and attack location
					- Dying and respawning


			- Gameplay logic for handling durations
				- Prevent characters from attacking after they've just been attacked (Check if this is something that doesn't cause network problems, or if it's something we should handle independently)
				- Things like preventing stamina regeneration for a duration after the player's attacked, sprinted, etc.
				
	*/

	
	// Retrieve the player and target's information, and handle attribute clamping during damage calculations
	FAttributeStatusInformation Statuses;
	FAttributeCombatInformation CombatInfo;
	FGAttributeSetExecutionData Props;
	
	GetExecutionData(Data, Props);
	ACharacterBase* Character = Props.TargetCharacter;
	UAbilitySystem* AbilitySystem = Props.TargetAbilitySystem;
	UCombatComponent* CombatComponent = Props.TargetCombatComponent;

	// TODO: Add attack pattern to effect context
	// TODO: Add weapons/hitstun/hitLocation to effect context
	
	

	
	//--------------------------------------------------------------------------------------//
	// Attribute Calculations																//
	//--------------------------------------------------------------------------------------//
	if (Data.EffectSpec.Def->Executions.Num() == 0)
	{
		StatusCalculations(Props, Data.EvaluatedData.Attribute, Statuses, Data.EvaluatedData.Magnitude);
		DamageCalculations(Props, Data.EvaluatedData.Attribute, CombatInfo, Data.EvaluatedData.Magnitude);
	}
	

	//--------------------------------------------------------------------------------------//
	// Combat calculations (handle everything at once)										//
	//--------------------------------------------------------------------------------------//
	else if (Data.EvaluatedData.Attribute == GetDamageCalculationAttribute())
	{
		// Retrieve the damage calculations
		for (auto &[Attribute, Value] : Data.EffectSpec.ModifiedAttributes)
		{
			StatusCalculations(Props, Attribute, Statuses, Value);
			DamageCalculations(Props, Attribute, CombatInfo, Value);
			// TODO: Any other effects to attributes - stamina drain, etc.
		}
	}
	
	
	//--------------------------------------------------------------------------------------//
	// Take Damage calculations																//
	//--------------------------------------------------------------------------------------//
	if (CombatInfo.DamageTaken > 0.0 ||
		CombatInfo.MagicDamageTaken > 0.0 ||
		CombatInfo.PoiseDamageTaken > 0.0 ||
		CombatInfo.bPoiseBroken ||
		Statuses.bCharacterBled ||
		Statuses.bWasFrostbitten)
	{
		// Physical / Magic damage
		float CurrentHealth = GetHealth() - CombatInfo.MagicDamageTaken - CombatInfo.DamageTaken;
		
		// Bleed damage
		if (Statuses.bCharacterBled)
		{
			CurrentHealth -= 100 + (GetMaxHealth() * 0.15);
			if (CombatInfo.HitStun < EHitStun::Medium)
			{
				CombatInfo.HitStun = EHitStun::Medium;
			}
		}

		// Curse damage
		if (Statuses.bWasCursed)
		{
			CombatInfo.DamageTaken += GetHealth();
		}

		// Frostbite damage
		if (Statuses.bWasFrostbitten)
		{
			CurrentHealth -= 100 + (GetMaxHealth() * 0.15);
			if (CombatInfo.HitStun < EHitStun::Medium)
			{
				CombatInfo.HitStun = EHitStun::Medium;
			}
		}
		
		// Damage multipliers for weapon stats and player equipment, and any other status effects should be handled here
		
		
		// Handle take damage
		CombatComponent->HandleDamageTaken(Props.SourceCharacter, Props.Context.GetSourceObject(), CombatInfo.DamageTaken, Data.EvaluatedData.Attribute);
		// if (CurrentHealth <= 0.0)
		// {
		// 	SetHealth(0);
		// 	CombatComponent->HandleDeath(Props.SourceCharacter, Character, bWasCursed ? Montage_Section_Curse : Montage_Section_Death);
		// }
		// else
		{	
			SetHealth(CurrentHealth);
			if (CombatInfo.bPoiseBroken)
			{
				CombatComponent->PoiseBreak(Props.SourceCharacter, Character, GetDamage_Poise(), CombatInfo.HitStun, CombatInfo.HitDirection);
			}
		}

		
		// Health/Poise: (100)(10), Damage/Poise: (-10)(10) ->  Bleed: (0) / Frostbite: (0) / Cursed: (0)
		FString BleedDamage = FString("");
		FString FrostbiteDamage = FString("");
		FString PoisonDamage = FString("");
		FString MadnessDamage = FString("");
		FString CurseDamage = FString("");
		FString SleepDamage = FString("");
		if (Statuses.bCharacterBled || Statuses.BleedBuildup > 0) BleedDamage = FString("Bleed(").Append(Statuses.bCharacterBled ? "Proc" : "").Append(FString::FromInt(Statuses.BleedBuildup)).Append(") / ");
		if (Statuses.bWasFrostbitten || Statuses.FrostbiteBuildup > 0) FrostbiteDamage = FString("Frostbite(").Append(Statuses.bCharacterBled ? "Proc" : "").Append(FString::FromInt(Statuses.FrostbiteBuildup)).Append(") / ");
		if (Statuses.bWasPoisoned || Statuses.PoisonBuildup > 0) PoisonDamage = FString("Poison(").Append(Statuses.bCharacterBled ? "Proc" : "").Append(FString::FromInt(Statuses.PoisonBuildup)).Append(") / ");
		if (Statuses.bWasMaddened || Statuses.MadnessBuildup > 0) MadnessDamage = FString("Madness(").Append(Statuses.bCharacterBled ? "Proc" : "").Append(FString::FromInt(Statuses.MadnessBuildup)).Append(") / ");
		if (Statuses.bWasCursed || Statuses.CurseBuildup > 0) CurseDamage = FString("Curse(").Append(Statuses.bCharacterBled ? "Proc" : "").Append(FString::FromInt(Statuses.CurseBuildup)).Append(") / ");
		if (Statuses.bSlept || Statuses.SleepBuildup > 0) SleepDamage = FString("Sleep(").Append(Statuses.bCharacterBled ? "Proc" : "").Append(FString::FromInt(Statuses.SleepBuildup)).Append(")");
		UE_LOGFMT(LogTemp, Warning, "{0}::AttributeLogic() {1} attacked {2} with {3}! \n"
			"Health/Poise: ({4})({5}), Damage: ({6})({7}) {8} {9} {10} {11} {12} {13} {14} \n",

			*UEnum::GetValueAsString(Props.SourceCharacter->GetLocalRole()),
			*GetNameSafe(Props.SourceCharacter), *GetNameSafe(Props.TargetCharacter), *GetNameSafe(Props.Context.GetSourceObject()),
			
			FMath::CeilToInt(GetHealth()), FMath::CeilToInt(GetPoise()),
			FMath::CeilToInt(-CombatInfo.MagicDamageTaken - CombatInfo.DamageTaken),
			FMath::CeilToInt(-CombatInfo.PoiseDamageTaken),
			
			Statuses.StatusProc() || Statuses.StatusDamage() ? FString("->  ") : FString(""),
			
			BleedDamage, FrostbiteDamage, CurseDamage, PoisonDamage, MadnessDamage, SleepDamage
		);
	}

	OnPostGameplayEffectExecute.Broadcast(Props);
}


void UMMOAttributeLogic::DamageCalculations(const FGAttributeSetExecutionData& Props,
	const FGameplayAttribute& Attribute, FAttributeCombatInformation& CombatInformation, const float Value)
{
	ACharacterBase* Character = Props.TargetCharacter;
	
	// Physical damages // TODO: we need to check whether the different damage calculations are handled
	if (Attribute == GetDamage_StandardAttribute() && GetDamage_Standard() != 0.0) CombatInformation.DamageTaken += GetDamage_Standard();
	else if (Attribute == GetDamage_SlashAttribute() && GetDamage_Slash() != 0.0) CombatInformation.DamageTaken += GetDamage_Slash();
	else if (Attribute == GetDamage_PierceAttribute() && GetDamage_Pierce() != 0.0) CombatInformation.DamageTaken += GetDamage_Pierce();
	else if (Attribute == GetDamage_StrikeAttribute() && GetDamage_Strike() != 0.0) CombatInformation.DamageTaken += GetDamage_Strike();

	// Magic damages
	else if (Attribute == GetDamage_MagicAttribute() && GetDamage_Magic() != 0.0) CombatInformation.MagicDamageTaken += GetDamage_Magic();
	else if (Attribute == GetDamage_IceAttribute() && GetDamage_Ice() != 0.0) CombatInformation.MagicDamageTaken += GetDamage_Ice();
	else if (Attribute == GetDamage_FireAttribute() && GetDamage_Fire() != 0.0)
	{
		CombatInformation.MagicDamageTaken += GetDamage_Fire();
		SetFrostbiteBuildup(0.0);
	}
	else if (Attribute == GetDamage_HolyAttribute() && GetDamage_Holy() != 0.0)
	{
		CombatInformation.MagicDamageTaken += GetDamage_Holy();
		SetCurseBuildup(0.0);
	}
	else if (Attribute == GetDamage_LightningAttribute() && GetDamage_Lightning() != 0.0) CombatInformation.MagicDamageTaken += GetDamage_Lightning();

	// Poise damages
	else if (Attribute == GetDamage_PoiseAttribute())
	{
		CombatInformation.PoiseDamageTaken = GetDamage_Poise();
		
		// Check if it was from a weapon
		AArmament* Armament = Cast<AArmament>(Props.Context.GetSourceObject());
		if (Armament)
		{
			FVector WeaponLocation = Armament->GetCenterLocation(); // TODO: create a custom target data object for returning the proper information
			CombatInformation.HitDirection = Character->GetHitReactDirection(Props.SourceCharacter, Props.SourceCharacter->GetActorLocation(), Props.TargetCharacter->GetActorLocation());
			CombatInformation.HitStun = Armament->GetHitStun(EInputAbilities::None, CombatInformation.PoiseDamageTaken);
		}
		else
		{
			CombatInformation.HitDirection = EHitDirection::None;
			CombatInformation.HitStun = EHitStun::None;
		}

		// Poise damage
		float CurrentPoise = GetPoise() - CombatInformation.PoiseDamageTaken;
		if (CurrentPoise <= 0.0)
		{
			CombatInformation.bPoiseBroken = true;
			CurrentPoise = GetMaxPoise();
		}
				
		SetPoise(CurrentPoise);
	}
}


void UMMOAttributeLogic::StatusCalculations(const FGAttributeSetExecutionData& Props,
	const FGameplayAttribute& Attribute, FAttributeStatusInformation& Statuses, const float Value)
{
	bool ImmuneToAttribute = IsImmuneToAttribute(Props, Attribute);
	ACharacterBase* Character = Props.TargetCharacter;
	UAbilitySystem* AbilitySystem = Props.TargetAbilitySystem;
	UCombatComponent* CombatComponent = Props.TargetCombatComponent;

	/**
		Status calculations
			- status buildup
			- Status effect (Take damage / slow / poison)
	*/
	if (!ImmuneToAttribute)
	{
		if (Attribute == GetBleedAttribute() || Attribute == GetBleedBuildupAttribute())
		{
			Statuses.BleedBuildup = Value;
			SetBleedBuildup(FMath::Clamp(GetBleedBuildup() + Statuses.BleedBuildup, 0, GetMaxBleedBuildup()));

			if (GetBleedBuildup() == GetMaxBleedBuildup())
			{
				Statuses.bCharacterBled = true;
				SetBleedBuildup(0.0);
				CombatComponent->HandleBleed(Props.SourceCharacter, Character, GetBleedBuildupAttribute(), GetBleedBuildup());
			}
		}
		else if ((Attribute == GetFrostbiteAttribute() ||
				Attribute == GetFrostbiteBuildupAttribute()) &&
				!AbilitySystem->HasMatchingGameplayTag(FrostbiteTag))
		{
			Statuses.FrostbiteBuildup = Value;
			SetFrostbiteBuildup(FMath::Clamp(GetFrostbiteBuildup() + Statuses.FrostbiteBuildup, 0, GetMaxFrostbiteBuildup()));

			if (GetFrostbiteBuildup() == GetMaxFrostbiteBuildup())
			{
				Statuses.bWasFrostbitten = true;
				CombatComponent->HandleFrostbite(Props.SourceCharacter, Character, GetFrostbiteBuildupAttribute(), GetFrostbiteBuildup());
			}
		}
		else if ((Attribute == GetPoisonAttribute() ||
				Attribute == GetPoisonBuildupAttribute()) &&
				!AbilitySystem->HasMatchingGameplayTag(PoisonedTag))
		{
			Statuses.PoisonBuildup = Value;
			SetPoisonBuildup(FMath::Clamp(GetPoisonBuildup() + Statuses.PoisonBuildup, 0, GetMaxPoisonBuildup()));

			if (GetPoisonBuildup() == GetMaxPoisonBuildup())
			{
				Statuses.bWasPoisoned = true;
				CombatComponent->HandlePoisoned(Props.SourceCharacter, Character, GetPoisonBuildupAttribute(), GetPoisonBuildup());
			}
		}
		else if ((Attribute == GetMadnessAttribute() ||
				Attribute == GetMadnessBuildupAttribute()) &&
				!AbilitySystem->HasMatchingGameplayTag(MaddenedTag))
		{
			Statuses.MadnessBuildup = Value;
			SetMadnessBuildup(FMath::Clamp(GetMadnessBuildup() + Statuses.MadnessBuildup, 0, GetMaxMadnessBuildup()));
			
			if (GetMadnessBuildup() == GetMaxMadnessBuildup())
			{
				Statuses.bWasMaddened = true;
				CombatComponent->HandleMadness(Props.SourceCharacter, Character, GetMadnessBuildupAttribute(), GetMadnessBuildup());
			}
		}
		if ((Attribute == GetCurseAttribute() ||
			Attribute == GetCurseBuildupAttribute()) &&
			!AbilitySystem->HasMatchingGameplayTag(CursedTag))
		{
			Statuses.CurseBuildup = Value;
			SetCurseBuildup(FMath::Clamp(GetCurseBuildup() + Statuses.CurseBuildup, 0, GetMaxCurseBuildup()));

			if (GetCurseBuildup() == GetMaxCurseBuildup())
			{
				Statuses.bWasCursed = true;
				SetHealth(0.0);
				CombatComponent->HandleCurse(Props.SourceCharacter, Character, GetCurseBuildupAttribute(), GetCurseBuildup());
			}
		}
		else if ((Attribute == GetSleepAttribute() ||
				Attribute == GetSleepBuildupAttribute()) &&
				!AbilitySystem->HasMatchingGameplayTag(SleepTag))
		{
			Statuses.SleepBuildup = Value;
			SetSleepBuildup(FMath::Clamp(GetSleepBuildup() + Statuses.SleepBuildup, 0, GetMaxSleepBuildup()));

			if (GetSleepBuildup() == GetMaxSleepBuildup())
			{
				Statuses.bSlept = true;
				CombatComponent->HandleSleep(Props.SourceCharacter, Character, GetSleepBuildupAttribute(), GetSleepBuildup());
			}
		}
	}
}


bool UMMOAttributeLogic::IsImmuneToAttribute(const FGAttributeSetExecutionData& Props, const FGameplayAttribute& Attribute) const
{
	UCombatComponent* Character = Props.TargetCombatComponent;

	if (Attribute == GetBleedBuildupAttribute() || Attribute == GetBleedAttribute())
	{
		if (Character && Character->IsImmuneToBleed(Props.SourceCharacter, Props.Context.GetSourceObject(), ECombatAttribute::Bleed, Props.DeltaValue)) 
		{
			return true;
		}
		
		return GetMaxBleedBuildup() <= 0;
	}
	if (Attribute == GetFrostbiteBuildupAttribute() || Attribute == GetFrostbiteAttribute())
	{
		if (Character && Character->IsImmuneToFrostbite(Props.SourceCharacter, Props.Context.GetSourceObject(), ECombatAttribute::Frostbite, Props.DeltaValue)) 
		{
			return true;
		}
		
		return GetMaxFrostbiteBuildup() <= 0;
	}
	if (Attribute == GetPoisonBuildupAttribute() || Attribute == GetPoisonAttribute())
	{
		if (Character && Character->IsImmuneToPoison(Props.SourceCharacter, Props.Context.GetSourceObject(), ECombatAttribute::Poison, Props.DeltaValue)) 
		{
			return true;
		}
		
		return GetMaxPoisonBuildup() <= 0;
	}
	if (Attribute == GetMadnessBuildupAttribute() || Attribute == GetMadnessAttribute())
	{
		if (Character && Character->IsImmuneToMadness(Props.SourceCharacter, Props.Context.GetSourceObject(), ECombatAttribute::Madness, Props.DeltaValue)) 
		{
			return true;
		}
		
		return GetMaxMadnessBuildup() <= 0;
	}
	if (Attribute == GetCurseBuildupAttribute() || Attribute == GetCurseAttribute())
	{
		if (Character && Character->IsImmuneToCurses(Props.SourceCharacter, Props.Context.GetSourceObject(), ECombatAttribute::Curse, Props.DeltaValue)) 
		{
			return true;
		}
		
		return GetMaxCurseBuildup() <= 0;
	}
	if (Attribute == GetSleepBuildupAttribute() || Attribute == GetSleepAttribute())
	{
		if (Character && Character->IsImmuneToSleep(Props.SourceCharacter, Props.Context.GetSourceObject(), ECombatAttribute::Sleep, Props.DeltaValue)) 
		{
			return true;
		}
		
		return GetMaxSleepBuildup() <= 0;
	}

	return false;
}


void UMMOAttributeLogic::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}


void UMMOAttributeLogic::ClampEvaluatedAttribute(const FGameplayAttribute& AttributeToClamp, FGameplayModifierEvaluatedData& EvaluatedAttribute, const float MinValue, const float MaxValue)
{
	if (EvaluatedAttribute.Attribute == AttributeToClamp)
	{
		EvaluatedAttribute.Magnitude = FMath::Clamp(EvaluatedAttribute.Magnitude, MinValue, MaxValue);
	}
}


UMMOAttributeLogic::UMMOAttributeLogic()
{
	PoisonedTag = FGameplayTag::RequestGameplayTag(Tag_Status_Poison);
	FrostbiteTag = FGameplayTag::RequestGameplayTag(Tag_Status_Frostbite);
	MaddenedTag = FGameplayTag::RequestGameplayTag(Tag_Status_Madness);
	CursedTag = FGameplayTag::RequestGameplayTag(Tag_Status_Curse);
	SleepTag = FGameplayTag::RequestGameplayTag(Tag_Status_Sleep);
}
