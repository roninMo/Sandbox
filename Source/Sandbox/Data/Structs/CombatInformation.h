#pragma once


#include "CoreMinimal.h"
#include "AbilityInformation.h" // TODO: This might cause dependency errors
#include "AttributeSet.h"
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

// Montages
#define Montage_Equip FName("Equip")
#define Montage_Unequip FName("Unequip")

// Equip Montage sections
#define Montage_EquipSection FName("Equip")
#define Montage_UnequipSection FName("Unequip")




/**
 *  Attack information for an armament or an individual combo attack. These are the objects created after retrieving the weapons current information from different armament stances
 */
USTRUCT(BlueprintType)
struct F_AttackInformation
{
	GENERATED_USTRUCT_BODY()
	F_AttackInformation() = default;

	/** The base damages or damage multipliers for of the combo attack. @note You can use this for multipliers for different attacks, or actual damage for attacks that don't factor in the armament's based damage  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FGameplayAttribute, float> BaseDamagesOrMultipliers;
};


/**
 * This is all the information for a specific combo attack
 */
USTRUCT(BlueprintType)
struct F_ComboAttack
{
	GENERATED_USTRUCT_BODY()
	F_ComboAttack() = default;
	
	/** The attack information specific to this combo attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_AttackInformation AttackInformation;
	
	/** The stamina cost of the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StaminaCost;
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	/** The combos of a specific attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<F_ComboAttack> ComboAttacks;
};



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
	
	/** The level of the ability */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 Level = 1;
	
	/** The attack pattern and input id for the current ability. This helps determine the specific attack montage we're granting to the character for the ability */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EInputAbilities InputId;
	
	/** The combos of a specific attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) F_ComboAttacks ComboInformation;
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
	
	/** The armament's passives */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FGameplayEffectInfo> Passives;
	
	/** The armament's state information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayEffectInfo StateInformation;

	/** The armament's abilities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<F_ArmamentAbilityInformation> CombatAbilities;
	
	/** The armament's abilities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FGameplayAbilityInfo> Abilities;

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


/*

	Weapon
		- Ranged Information / Damage
			- Weapon / Attachment config
			- Ranged damage information
			
		- Melee Information / Damage
			- Combo retrieval
			- Melee damage information

	->
	-> Damage information (varying attribute set modifiers)

	
	Weapon
		- Equip slot mapping
		- Anim montage mapping
		- Ability mapping
		- Character to Weapon information mapping
		- 







	Combat Component
		- Attribute damage calculation



	Combat
		- Weapon retrieves it's attack information
			- Branching logic is okay here, it ends up adjusting attributes which can easily be added to both
		- Weapon creates a damage calculation and sends it to the attribute logic
		- AttributeLogic handles adjusting attributes

 
*/

