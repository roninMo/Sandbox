// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Sandbox/World/Props/Items/Item.h"
#include "EffectActor.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};


/**
 * 
 */
UCLASS()
class SANDBOX_API AEffectActor : public AItem
{
	GENERATED_BODY()

protected:
	AEffectActor();
	UFUNCTION(BlueprintCallable) void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);
	UFUNCTION(BlueprintCallable) void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable) void OnEndOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable) void ApplyApplicationPolicyEffects(AActor* TargetActor, EEffectApplicationPolicy ApplicationPolicy);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") bool bDestroyOnEffectRemoval = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects") EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects") FActiveGameplayEffectHandle InfiniteEffectHandle;

	
};
