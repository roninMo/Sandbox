// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetController.generated.h"

class ACharacterBase;
class AAbilitySystemPlayerState;
class UAbilitySystem;
class UMMOAttributeSet;


USTRUCT(BlueprintType)
struct FWidgetControllerInformation
{
	GENERATED_BODY()
	FWidgetControllerInformation() {}
	FWidgetControllerInformation(
		ACharacterBase* C,
		UAbilitySystem* ASC,
		UMMOAttributeSet* Attrs,
		APlayerController* PC,
		AAbilitySystemPlayerState* PS
	) :
		Character(C),
		AbilitySystemComponent(ASC),
		AttributeSet(Attrs),
		PlayerController(PC),
		PlayerState(PS)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<ACharacterBase> Character;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UAbilitySystem> AbilitySystemComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UMMOAttributeSet> AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<AAbilitySystemPlayerState> PlayerState;
};


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SANDBOX_API UWidgetController : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController") TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController") TObjectPtr<AAbilitySystemPlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController") TObjectPtr<UAbilitySystem> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController") TObjectPtr<UMMOAttributeSet> AttributeSet;


public:
	UFUNCTION(BlueprintCallable) void SetWidgetControllerParams(const FWidgetControllerInformation& WCParams);
	
	/* Broadcast the initial attribute values once the widget has been created. */
	UFUNCTION(BlueprintCallable) virtual void BroadcastInitialValues();
	
	/* Bind to the ability system attribute delegates to broadcast the value changes for specific widgets.. */
	UFUNCTION(BlueprintCallable) virtual void BindCallbacksToDependencies();
	
	
};
