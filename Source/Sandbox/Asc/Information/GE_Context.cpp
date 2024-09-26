// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_Context.h"

bool FGE_Context::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint8 RepBits = 0;
	if (Ar.IsSaving())
	{
		// These are bitwise conversions that map a value to binary digits
		// That and these operations are conglomerations based on the saved information and what you're adding
		// The number on the right denotes the place that the value is going to be set on a byte, which is combined with the saved byte that's going to be sent across the server
		// So say we have a, b, c, and d and they each represent a 0 or 1 -> 0000, then if a=1 and d=1 we add that to the saved 0000, it would be 0000 + 1001, which every true value equals 1 when it combines
		// This probably gets way more complicated for non boolean values, but this is like the domino effect of what happens when you're building through net serialization
		// the FArchive has functions to make these conversions easy for you but this is how it's done
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0; // 0001
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1; // 0010
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2; // etc...
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}

		// Sandbox context values
		if (PoiseDamage < 0.0f)
		{
			RepBits |= 1 << 7;
		}
		if (PhysicalArmor < 0.0f)
		{
			RepBits |= 1 << 8;
		}
		if (MagicalArmor < 0.0f)
		{
			RepBits |= 1 << 9;
		}
		if (KnockbackType != 0.0f)
		{
			RepBits |= 1 << 10;
		}
		if (!KnockbackForce.IsZero())
		{
			RepBits |= 1 << 11;
		}
		if (KnockbackDirection != 0.0f)
		{
			RepBits |= 1 << 12;
		}
		if (bBlockedAttack)
		{
			RepBits |= 1 << 13;
		}
		if (bPerfectParriedAttack)
		{
			RepBits |= 1 << 14;
		}
		if (bParriedAttack)
		{
			RepBits |= 1 << 15;
		}
	}

	
	
	Ar.SerializeBits(&RepBits, 16);


	
	// These are true if both values are 1
	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	
	// Sandbox context values
	if (RepBits & (1 << 7))
	{
		Ar << PoiseDamage;
	}
	if (RepBits & (1 << 8))
	{
		Ar << PhysicalArmor;
	}
	if (RepBits & (1 << 9))
	{
		Ar << MagicalArmor;
	}
	if (RepBits & (1 << 10))
	{
		Ar << KnockbackType;
	}
	if (RepBits & (1 << 11))
	{
		KnockbackForce.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 12))
	{
		Ar << KnockbackDirection;
	}
	if (RepBits & (1 << 13))
	{
		Ar << bBlockedAttack;
	}
	if (RepBits & (1 << 14))
	{
		Ar << bPerfectParriedAttack;
	}
	if (RepBits & (1 << 15))
	{
		Ar << bParriedAttack;
	}



	
	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}
	
	bOutSuccess = true;
	return true;
}
