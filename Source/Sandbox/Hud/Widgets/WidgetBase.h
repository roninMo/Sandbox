// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetBase.generated.h"

class UInventoryComponent;
class UAbilitySystem;
class UCombatComponent;
class ACharacterBase;


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SANDBOX_API UWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;


public:
	/**
	 * This sets the widget controller for this widget
	 * The controller is anything, and I think the premise of this is to do logic that's associated with the Asc when set (This is epic's way of doing things)
	 */
	UFUNCTION(BlueprintCallable) virtual void SetWidgetController(UObject* InWidgetController);

	/**
	 * This sets the widget controller for this widget if it isn't already set
	 * The controller is anything, and I think the premise of this is to do logic that's associated with the Asc when set (This is epic's way of doing things)
	 */
	UFUNCTION(BlueprintCallable) virtual void SetWidgetControllerIfNotAlreadySet(UObject* InWidgetController);

	/** Retrieves the widget controller */
	UFUNCTION(BlueprintCallable) virtual UObject* GetWidgetController() const;

	/** Function for handling information and adding to viewport */
	UFUNCTION(BlueprintCallable) virtual void ShowWidget(float ZOrder = 0);
	
	/** Hide this hud widget and any widgets associated to it */
	UFUNCTION(BlueprintCallable) virtual void HideWidget();
	
	

	/** Blueprint function for handling logic once the widget information is ready */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "WidgetController") void WidgetControllerSet();
	

protected:
	/** Retrieves the character */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget|Character") ACharacterBase* GetCharacter() const;
	virtual ACharacterBase* GetCharacter_Implementation() const;

	/** Retrieves the combat component */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget|Character") UCombatComponent* GetCombatComponent(ACharacterBase* Character = nullptr) const;
	virtual UCombatComponent* GetCombatComponent_Implementation(ACharacterBase* Character = nullptr) const;

	/** Retrieves the ability system */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget|Character") UAbilitySystem* GetAbilitySystem(ACharacterBase* Character = nullptr) const;
	virtual UAbilitySystem* GetAbilitySystem_Implementation(ACharacterBase* Character = nullptr) const;

	/** Retrieves the inventory */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget|Character") UInventoryComponent* GetInventory(ACharacterBase* Character = nullptr) const;
	virtual UInventoryComponent* GetInventory_Implementation(ACharacterBase* Character = nullptr) const;
	

};
