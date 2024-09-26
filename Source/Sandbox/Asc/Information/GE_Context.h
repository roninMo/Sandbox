// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GE_Context.generated.h"

/**
 * Data structure that stores an instigator and related data, such as positions and targets
 * It is passed throughout effect execution so it is a great place to track transient information about an execution
 * TODO: Refactor
 */
USTRUCT(BlueprintType)
struct FGE_Context : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	/**	FGameplayEffectContext base values
	 * TWeakObjectPtr<AActor> Instigator;
	 * TWeakObjectPtr<AActor> EffectCauser; // The physical actor that actually did the damage, can be a weapon or projectile
	 * TWeakObjectPtr<UGameplayAbility> AbilityCDO; // The ability CDO that is responsible for this effect context (replicated)
	 * TWeakObjectPtr<UGameplayAbility> AbilityInstanceNotReplicated; // The ability instance that is responsible for this effect context (NOT replicated)
	 * int32 AbilityLevel; // The level this was executed at
	 * TWeakObjectPtr<UObject> SourceObject; // Object this effect was created from, can be an actor or static object. Useful to bind an effect to a gameplay object
	 * TWeakObjectPtr<UAbilitySystemComponent> InstigatorAbilitySystemComponent; // The ability system component that's bound to instigator
	 * TArray<TWeakObjectPtr<AActor>> Actors; // Actors referenced by this context
	 * TSharedPtr<FHitResult>	HitResult; // Trace information - may be nullptr in many cases
	 * FVector	WorldOrigin; // Stored origin, may be invalid if bHasWorldOrigin is false
	 * uint8 bHasWorldOrigin:1;
	 * uint8 bReplicateSourceObject:1; // True if the SourceObject can be replicated. This bool is not replicated itself.
	 * uint8 bReplicateInstigator:1; // True if the Instigator can be replicated. This bool is not replicated itself.
	 * uint8 bReplicateEffectCauser:1; // True if the Instigator can be replicated. This bool is not replicated itself.
	 */

	void SetPoiseDamage(const float Damage) { PoiseDamage = Damage; }
	void SetPhysicalArmor(const float Armor) { PhysicalArmor = Armor; }
	void SetMagicalArmor(const float Armor) { MagicalArmor = Armor; }
	void SetKnockbackType(const float Type) { KnockbackType = Type; }
	void SetKnockbackForce(const FVector& Force) { KnockbackForce = Force; }
	void SetKnockbackDirection(const float Direction) { KnockbackDirection = Direction; }
	void SetBlockedAttack(const bool bBlockHit) { bBlockedAttack = bBlockHit; }
	void SetPerfectParriedAttack(const bool bPerfectParry) { bPerfectParriedAttack = bPerfectParry; }
	void SetParriedAttack(const bool bParry) { bParriedAttack = bParry; }
	
	float GetPoiseDamage() const { return PoiseDamage; }
	float GetPhysicalArmor() const { return PhysicalArmor; }
	float GetMagicalArmor() const { return MagicalArmor; }
	float GetKnockbackType() const { return KnockbackType; }
	FVector GetKnockbackForce() const { return KnockbackForce; }
	float GetKnockbackDirection() const { return KnockbackDirection; }
	bool GetBlockedAttack() const { return bBlockedAttack; }
	bool GetPerfectParriedAttack() const { return bPerfectParriedAttack; }
	bool GetParriedAttack() const { return bParriedAttack; }

	
	
	//-----------------------------------------------------------------------------------//
	// Boiler plate code																 //
	//-----------------------------------------------------------------------------------//
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
        return StaticStruct();
	}

    /** Creates a copy of this context, used to duplicate for later modifications */
    virtual FGE_Context* Duplicate() const override
    {
        FGE_Context* NewContext = new FGE_Context();
        *NewContext = *this;
        NewContext->AddActors(Actors);
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
	
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;


	

protected:
	//-----------------------------------------------------------------------------------//
	// Combat specific values															 //
	//-----------------------------------------------------------------------------------//
	// The object pointers here have to be weak because contexts aren't necessarily tracked by GC in all cases
	/** Combat specific values  */
	UPROPERTY() float PoiseDamage = 0;
	UPROPERTY() float PhysicalArmor = 0;
	UPROPERTY() float MagicalArmor = 0;
	
	UPROPERTY() float KnockbackType = 0;
	UPROPERTY() FVector KnockbackForce = FVector::ZeroVector;
	UPROPERTY() float KnockbackDirection = 0;
	
	// These are captured during the effect, but I want to find out if there's scenarios where the exec calc that's not predicted
	UPROPERTY() bool bBlockedAttack = false;
	UPROPERTY() bool bPerfectParriedAttack = false;
	UPROPERTY() bool bParriedAttack = false;
	
	
	
	
};


template <>
struct TStructOpsTypeTraits<FGE_Context> : public TStructOpsTypeTraitsBase2<FGE_Context>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
