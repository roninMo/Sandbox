// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "HudWidget.generated.h"

class APlayerHud;
class UCombatComponent;
class UInventoryComponent;


/**
 * 
 */
UCLASS()
class SANDBOX_API UHudWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) virtual APlayerHud* GetHud();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetInventoryHud();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetDetailsPanelHud();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetEquipmentHud();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetCharacterStatsHud();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetScoreboard();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetPauseMenu();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetSettingsMenu();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetContainerInventoryWidget();
	// UFUNCTION(BlueprintCallable) virtual UHudWidget* GetItemStatsWidget();

	
};
