// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Saved_Attributes.generated.h"

class UAttributeSet;
/**
 * Saved attributes pertaining to the player's current health, stamina, mana, experience, etc.
 * TODO: Add a statuses save, to save any of the buffs/debuffs (and durations) from when the player previously saved
 */
UCLASS()
class SANDBOX_API USaved_Attributes : public USaveGame
{
	GENERATED_BODY()

public:
	// Default
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Stamina;

	// MMO
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Experience;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Mana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Poise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BleedBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FrostbiteBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PoisonBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MadnessBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CurseBuildup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SleepBuildup;


public:
	/** Save information from player's current attribute set */
	UFUNCTION(BlueprintCallable, Category = "Saving and Loading")
	virtual void SaveFromAttributeSet(UAttributeSet* AttributeSet);

	
};
