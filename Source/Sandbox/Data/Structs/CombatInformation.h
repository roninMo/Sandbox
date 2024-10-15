#pragma once


#include "CoreMinimal.h"
#include "AbilityInformation.h" // TODO: This might cause dependency errors
#include "AttributeSet.h"
#include "Sandbox/Data/Enums/SkeletonMappings.h"
#include "CombatInformation.generated.h"

class AArmament;
class UGameplayAbility;
class UGameplayEffect;
struct FGameplayAttribute;
enum class EEquipRestrictions : uint8;
enum class EArmamentStance : uint8;
enum class EArmamentClassification : uint8;
enum class EDamageInformationSource : uint8;
enum class EInputAbilities : uint8;

// Montage information
#define Montage_ComboSections TArray<FName> {FName("1"), FName("2"), FName("3"), FName("4"), FName("5"), FName("6"), FName("7"), FName("8"), FName("9"), FName("10"), FName("1")}
#define Montage_Section_Charge FString("_C")
#define Montage_Section_CrouchAttack FName("RunningAttack")
#define Montage_Section_RunningAttack FName("CrouchingAttack")

// Montages
#define Montage_Equip FName("Equip")
#define Montage_Unequip FName("Unequip")


// Death montages
#define Montage_Section_Death FName("Death")
#define Montage_Section_Curse FName("Curse")

// HitStun sections
#define Montage_Section_HitStun_VS FName("VS")
#define Montage_Section_HitStun_V FName("V")
#define Montage_Section_HitStun_M FName("M")
#define Montage_Section_HitStun_L FName("L")
#define Montage_Section_HitStun_FP FName("FP")
#define Montage_Section_HitStun_FF FName("FF")

#define Montage_Section_HitReact_Front FName("_F")
#define Montage_Section_HitReact_Back FName("_B")
#define Montage_Section_HitReact_Left FName("_L")
#define Montage_Section_HitReact_Right FName("_R")

// Equip Montage sections
#define Montage_EquipSection FName("Equip")
#define Montage_UnequipSection FName("Unequip")

// Roll Montage sections
#define Montage_Section_Roll_Forward FName("Forward")
#define Montage_Section_Roll_Backward FName("Backward")
#define Montage_Section_Roll_Left FName("Left")
#define Montage_Section_Roll_Right FName("Right")
#define Montage_Section_Roll_ForwardLeft FName("ForwardLeft")
#define Montage_Section_Roll_ForwardRight FName("ForwardRight")
#define Montage_Section_Roll_BackwardLeft FName("BackwardLeft")
#define Montage_Section_Roll_BackwardRight FName("BackwardRight")



//--------------------------------------------------------------------------------------//
// Combat Logic																			//
//--------------------------------------------------------------------------------------//

/**
 *  Attack information for an armament or an individual combo attack. These are the objects created after retrieving the weapons current information from different armament stances
 */
// USTRUCT(BlueprintType)
// struct F_AttackInformation
// {
// 	GENERATED_USTRUCT_BODY()
// 	F_AttackInformation() = default;
//
// 	/** The base damages or damage multipliers for of the combo attack. @note You can use this for multipliers for different attacks, or actual damage for attacks that don't factor in the armament's based damage  */
// 	// UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGameplayAttribute, float> BaseDamagesOrMultipliers;
// 	
// 	/** The motion value of the current attack  */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MotionValue = 1;
// 	
// 	/** The motion value for statuses of the current attack  */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StatusMotionValue = 1;
// };


/**
 * This is all the information for a specific combo attack
 */
USTRUCT(BlueprintType)
struct F_ComboAttack
{
	GENERATED_USTRUCT_BODY()
	F_ComboAttack() = default;
	
	/** The motion value of the current attack  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MotionValue = 100;

	/** The motion value for statuses of the current attack  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StatusMotionValue = 100;

	/** The stamina cost of the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StaminaCost;

	/** The motion value for poise of the current attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PoiseDamage;
	
	/** If there's a custom montage section for this combo attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName CustomMontageSection;
	
};


/**
 * This holds combos and damage stats for a specific combo 
 */
USTRUCT(BlueprintType)
struct F_ComboAttacks
{
	GENERATED_USTRUCT_BODY()
	F_ComboAttacks() = default;

	/** The  class that handles the damage effect calculations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	/** The combos of a specific attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="MotionValues: ({MotionValue}/{StatusMotionValue}) -> StaminaCost: ({StaminaCost}, PoiseDamage: ({PoiseDamage})"))
	TArray<F_ComboAttack> ComboAttacks;
	
};






//--------------------------------------------------------------------------------------//
// Armament Objects																		//
//--------------------------------------------------------------------------------------//

/**
 * The ability information specific to an armament
 *
 *		For influencing weapon damage and costs during one handing, two handing, or dual wielding, there's base damage stats and attribute modifications for each attack during each stance in combat
 *		Sometimes this can be really confusing if you want to adjust each combo attack, and have it tied to attributes, at the end of the day you're still creating adjustments during each combat attack
 *		So this is the base damage for reference, you'll probably have to create a way to add a combo attack and it's combat details together, especially if you want combat for different armament stances
 */
USTRUCT(BlueprintType)
struct F_ArmamentAbilityInformation
{
	GENERATED_USTRUCT_BODY()
	F_ArmamentAbilityInformation() = default;

	/** A stored reference to the weapon's ability */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UGameplayAbility> Ability;
	
	/** The attack pattern and input id for the current ability. This helps determine the specific attack montage we're granting to the character for the ability */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EInputAbilities InputId;

	/** The stances the player isn't able to use this combat ability in. For example, two handing shouldn't be able to use both block and the offhand attack which both use the same input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<EArmamentStance> InvalidStances;
	
	/** The level of the ability */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 Level = 1;
	
};









//--------------------------------------------------------------------------------------//
// Armament Montages																	//
//--------------------------------------------------------------------------------------//
/**
 * An object containing the different attack patterns and combo information with a montage map reference to each character, use this for each weapon stance. It is still super tangled, good luck
 */
USTRUCT(BlueprintType)
struct F_ArmamentMeleeMontage
{
	GENERATED_USTRUCT_BODY()
	F_ArmamentMeleeMontage() = default;

	/** The one hand montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_CharacterToMontage Montage;

	/** The combo attacks for this attack pattern */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ComboAttacks Combo;
	
};


/**
 * An object containing the different attack patterns and combo information with a montage map reference to each character.
 * Once the combat component retrieves the armament information, the character to montage reference is removed
 */
USTRUCT(BlueprintType)
struct F_ArmamentComboInformation
{
	GENERATED_USTRUCT_BODY()
	F_ArmamentComboInformation() = default;

	/** The one hand montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* Montage;

	/** The combo attacks for this attack pattern */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ComboAttacks Combo;
	
};


/**
 * Melee armament montages for each weapon stance. This is useful for building up armament logic
 */
USTRUCT(BlueprintType)
struct F_ArmamentMeleeMontages
{
	GENERATED_USTRUCT_BODY()
	F_ArmamentMeleeMontages() = default;

	/** The one hand montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EInputAbilities, F_ArmamentMeleeMontage> OneHandMontages;

	/** The two hand montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EInputAbilities, F_ArmamentMeleeMontage> TwoHandMontages;

	/** The dual wield montages for the different attack patterns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EInputAbilities, F_ArmamentMeleeMontage> DualWieldMontages;
	
};


/**
 * An object containing attack montages for an armament
 */
USTRUCT(BlueprintType)
struct F_ArmamentMontages
{
	GENERATED_USTRUCT_BODY()
	F_ArmamentMontages() = default;

	/** The armament's different melee montages for every character. Use @ref EArmamentStance to map each attack  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ArmamentMeleeMontages MeleeMontages;
	
	/** The armament's general montages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, F_CharacterToMontage> Montages;
};


/**
 * This is the data table to hold armament montages for every character
 */
USTRUCT(BlueprintType)
struct F_Table_ArmamentMontages : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ArmamentMontages ArmamentMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};







/**
 * The combat information specific to an armament 
 */
USTRUCT(BlueprintType)
struct F_ArmamentInformation
{
	GENERATED_USTRUCT_BODY()
	virtual ~F_ArmamentInformation() = default;
	F_ArmamentInformation() = default;

	/** The database id of the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;

	/** The armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AArmament> Armament;

	/** The armament classification */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EArmamentClassification Classification;
	
	/** The armament's equip restrictions */ // This is only here because of ranged weapons
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEquipRestrictions EquipRestrictions;
	
	/** Are the damage calculations from the armament, combo, or both? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EDamageInformationSource DamageCalculations;
	
	/** The base attack information of the armament */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGameplayAttribute, float> BaseDamageStats;

	/**
		For influencing weapon damage and costs during one handing, two handing, or dual wielding, there's base damage stats and attribute modifications for each attack during each stance in combat
		Sometimes this can be really confusing if you want to adjust each combo attack, and have it tied to attributes, at the end of the day you're still creating adjustments during each combat attack
		So this is the base damage for reference, you'll probably have to create a way to add a combo attack and it's combat details together, especially if you want combat for different armament stances

	 */

	// TODO: Add attribute damage scaling
	
	/** The armament's passives */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Effect)) TArray<FGameplayEffectInfo> Passives;
	
	/** The armament's state information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayEffectInfo StateInformation;

	/** The armament's abilities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="{InputId} ->  {Ability}")) TArray<F_ArmamentAbilityInformation> MeleeAbilities;
	
	/** The armament's abilities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Ability)) TArray<FGameplayAbilityInfo> Abilities;

	/** There needs to be a valid reference to retrieve the armament/item */
	virtual bool IsValid() const
	{
		return Id.IsValid();
	}
};


/**
 * This is the data table to hold all the armament information, data, and attacks
 */
USTRUCT(BlueprintType)
struct F_Table_ArmamentInformation : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ArmamentInformation ArmamentInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};














//--------------------------------------------------------------------------------------//
// Combat Npc Information																//
//--------------------------------------------------------------------------------------//
/**
 * An object containing the different attack patterns and combo information with a montage map reference to each character, use this for each weapon stance. It is still super tangled, good luck
 */
USTRUCT(BlueprintType)
struct F_EnemyAttackPattern
{
	GENERATED_USTRUCT_BODY()
	F_EnemyAttackPattern() = default;

	/** The name of the attack pattern */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Name;

	/** The montage for the attack pattern */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_CharacterToMontage Montage;

	/** The combo attacks for this attack pattern */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ComboAttacks Combo;
	
};



/**
 * This is the data table to hold certain attack patterns for multiple enemies within the game.
 */
USTRUCT(BlueprintType)
struct F_Table_EnemyAttackPatterns : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_EnemyAttackPattern AttackPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString DevDescription;
};








