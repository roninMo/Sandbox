// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyStateBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NotifyStateLog, Log, All);

class UAbilitySystem;
class UCombatComponent;
class ACharacterBase;


/**
 * 
 */
UCLASS()
class SANDBOX_API UAnimNotifyStateBase : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UAnimNotifyStateBase();

	
protected:
	/** Retrieves the character and the combat component */
	UFUNCTION(BlueprintCallable, Category = "Notify State|Utils")
	virtual bool GetCharacterAndCombatComponent(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character, UCombatComponent*& CombatComponent) const;

	/** Retrieves the character and the ability system */
	UFUNCTION(BlueprintCallable, Category = "Notify State|Utils")
	virtual bool GetCharacterAndAbilitySystem(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character, UAbilitySystem*& AbilitySystem) const;
	
	/** Retrieves the character, combat component, and ability system */
	UFUNCTION(BlueprintCallable, Category = "Notify State|Utils")
	virtual bool GetCharacterInformation(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character, UCombatComponent*& CombatComponent, UAbilitySystem*& AbilitySystem) const;

	/** Retrieves the character */
	UFUNCTION(BlueprintCallable, Category = "Notify State|Utils")
	virtual bool GetCharacter(USkeletalMeshComponent* MeshComp, ACharacterBase*& Character) const;
	
	
};
