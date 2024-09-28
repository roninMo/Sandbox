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

