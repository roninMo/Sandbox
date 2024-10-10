// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/MMOAttributeLogic.h"

#include "GameplayEffectExtension.h"
#include "Logging/StructuredLog.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Combat/Weapons/Armament.h"


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
	FGAttributeSetExecutionData Props;
	/**
		Apply buildups / damages / poise damages, then the hit react, and handle any other logic after that

		Handling individual calculations isn't as complex as having everything together, there's a lot more of interesting things and calculations with multiple attributes.
		You just need to clear out the old attributes and help handle some of the basic values for handling damage

			https://fextralife.com/forums/t52683/poise-explanation-general-pvp-tips
			https://www.reddit.com/r/Eldenring/comments/pjemv1/which_poise_system_do_you_prefer_the_most_and/
			

	*/

	// Retrieve the player and target's information, and handle attribute clamping during damage calculations
	GetExecutionData(Data, Props);


	/**** Combat calculations ****/
	if (Data.EvaluatedData.Attribute == GetDamageCalculationAttribute())
	{
		float MagicDamageTaken = 0.0;
		float DamageTaken = 0.0;
		
		// Retrieve the damage calculations
		for (auto &[Attribute, Value] : Data.EffectSpec.ModifiedAttributes)
		{
			/**
				Status calculations
					- status buildup
					- Status effect (Take damage / slow / poison)
			*/
			if (Attribute == GetCurseAttribute()) SetCurseBuildup(FMath::Clamp(GetCurseBuildup() + Value, 0, GetMaxCurseBuildup()));
			if (Attribute == GetBleedAttribute()) SetBleedBuildup(FMath::Clamp(GetBleedBuildup() + Value, 0, GetMaxBleedBuildup()));
			if (Attribute == GetPoisonAttribute()) SetPoisonBuildup(FMath::Clamp(GetPoisonBuildup() + Value, 0, GetMaxPoisonBuildup()));
			if (Attribute == GetFrostbiteAttribute()) SetFrostbiteBuildup(FMath::Clamp(GetFrostbiteBuildup() + Value, 0, GetMaxFrostbiteBuildup()));
			if (Attribute == GetMadnessAttribute()) SetMadnessBuildup(FMath::Clamp(GetMadnessBuildup() + Value, 0, GetMaxMadnessBuildup()));
			if (Attribute == GetSleepAttribute()) SetSleepBuildup(FMath::Clamp(GetSleepBuildup() + Value, 0, GetMaxSleepBuildup()));

			
			/**
				Damage calculations
					- Take damage
					- Hit reaction based on the attack
					- Handle taking damage / dying
			*/
			if (Attribute == GetDamage_StandardAttribute()) DamageTaken += GetDamage_Standard();
			if (Attribute == GetDamage_SlashAttribute()) DamageTaken += GetDamage_Slash();
			if (Attribute == GetDamage_PierceAttribute()) DamageTaken += GetDamage_Pierce();
			if (Attribute == GetDamage_StrikeAttribute()) DamageTaken += GetDamage_Strike();

			if (Attribute == GetDamage_MagicAttribute()) MagicDamageTaken += GetDamage_Magic();
			if (Attribute == GetDamage_IceAttribute()) MagicDamageTaken += GetDamage_Ice();
			if (Attribute == GetDamage_FireAttribute()) MagicDamageTaken += GetDamage_Fire();
			if (Attribute == GetDamage_HolyAttribute()) MagicDamageTaken += GetDamage_Holy();
			if (Attribute == GetDamage_LightningAttribute()) MagicDamageTaken += GetDamage_Lightning();

			
			/**
				Poise damage
					- Damage poise
					- Handle poise break / effect for regenerating poise
					- Handle hit reactions
			*/

			
			/**
				Any other effects to attributes
					- stamina drain, etc.
			*/

			
		}

		
		// Damage multipliers for weapon stats and player equipment, and any other status effects should be handled here



		
		// Take damage
		float CurrentHealth = GetHealth() - MagicDamageTaken - DamageTaken;
		if (CurrentHealth <= 0.0)
		{
			SetHealth(0);

			// Handle death
		}
		else
		{	
			SetHealth(CurrentHealth);

			// Handle any other take damage logic
		}


		/**


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
			


		
		// Player reactions / other handling
		ACharacterBase* Character = Cast<ACharacterBase>(Props.TargetActor);
		AArmament* Armament = Cast<AArmament>(Props.Context.GetSourceObject());
		if (Character)
		{
			UAnimMontage* Montage = Character->GetHitReactMontage();
			FVector WeaponLocation = Armament ? Armament->GetCenterLocation() : Props.SourceActor.Get()->GetActorLocation(); // TODO: create a custom target data object for returning the proper information
			FName HitReact = Character->GetHitReactSection(Character, Character->GetActorLocation(), WeaponLocation);
			
			Character->NetMulticast_PlayMontage(Montage, HitReact);
		}

		
		UE_LOGFMT(LogTemp, Warning, "{0}::{1}() {2} attacked {3} with {4}! Remaining health: {5}({6})", *UEnum::GetValueAsString(Props.SourceActor->GetLocalRole()), *FString(__FUNCTION__),
			*GetNameSafe(Props.SourceActor), *GetNameSafe(Props.TargetActor), *GetNameSafe(Props.Context.GetSourceObject()), GetHealth(), -(MagicDamageTaken + DamageTaken));
		
	}
	

	OnPostGameplayEffectExecute.Broadcast(Props);
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