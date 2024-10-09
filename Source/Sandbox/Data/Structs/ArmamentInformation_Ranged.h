// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ArmamentInformation_Ranged.generated.h"


enum class EBulletType : uint8;
enum class EFireRateType : uint8;
enum class EAttachment : uint8;

//----------------------------------------------------------------------------------------------//
// Ranged Armament Information  																//
//----------------------------------------------------------------------------------------------//


/**

	Stock
		- MovementSpeed while aiming bonus
		
	Muzzle
		- Silenced weapons

	Barrel
		- Weapon damage / range

	Clip
		- Ammo Amount 
		
	Sights
		- first person weapon scope
		- third person zoom range

	UnderBarrel
		- laser sights
		- flash lights
		- grenade launchers
		- shotgun attachments
		- smoke grenade launchers
		- tripods
		- attachments that help with accuracy




	Weapon Stats adjustments
		- Damage / Range
		- Ammo Amount
		- Accuracy
		- attachments

	Things that are adjusted from attachments
		- MovementSpeed
		- Third person Camera Zoom range
		- First Person Scope
		- Input actions for alternative attacks (grenade launchers and other attachments)


*/


/**
 * An object containing the information for a specific attachment
 */
USTRUCT(BlueprintType)
struct F_Attachment
{
	GENERATED_USTRUCT_BODY()
	F_Attachment() = default;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EAttachment AttachmentType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* AttachmentMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AdjustedAimMoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AdjustedClipAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AdjustedDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AdjustedDamageRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RecoilMultiplier;
};

// TODO: Instead of having the adjustments in the object, have logic for each attachment while retrieving the information


/**
 * An object containing all the ranged weapon information
 */
USTRUCT(BlueprintType)
struct F_RangedArmamentStats
{
	GENERATED_USTRUCT_BODY()
	F_RangedArmamentStats() = default;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DamageRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 AmmoCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EBulletType BulletType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EFireRateType FireRateType;
};




//--------------------------------------------------------------------------------------//
// Armament Montages																	//
//--------------------------------------------------------------------------------------//
/**
 * This object holds all the montages that a melee armament needs
 */
USTRUCT(BlueprintType)
struct FM_RangedArmamentMontages
{
	GENERATED_USTRUCT_BODY()
		FM_RangedArmamentMontages(
			UAnimMontage* FireWeapon = nullptr,
			UAnimMontage* Reload = nullptr,
			UAnimMontage* Aim = nullptr,
			UAnimMontage* Holster = nullptr,
			UAnimMontage* Equip = nullptr,
			
			UAnimMontage* FP_FireWeapon = nullptr,
			UAnimMontage* FP_Reload = nullptr,
			UAnimMontage* FP_Aim = nullptr,
			UAnimMontage* FP_Holster = nullptr,
			UAnimMontage* FP_Equip = nullptr
		) :

		FireWeapon(FireWeapon),
		Reload(Reload),
		Aim(Aim),
		Holster(Holster),
		Equip(Equip),

		FP_FireWeapon(FP_FireWeapon),
		FP_Reload(FP_Reload),
		FP_Aim(FP_Aim),
		FP_Holster(FP_Holster),
		FP_Equip(FP_Equip)
	{}

public:
	// Main
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* FireWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* Reload;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* Aim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* Holster;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)                     UAnimMontage* Equip;
	
	// First Person
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 					UAnimMontage* FP_FireWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 					UAnimMontage* FP_Reload;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 					UAnimMontage* FP_Aim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 					UAnimMontage* FP_Holster;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 					UAnimMontage* FP_Equip;
};
