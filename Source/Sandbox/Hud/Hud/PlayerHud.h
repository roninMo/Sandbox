// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHud.generated.h"

enum class EHudState : uint8;
struct FWidgetControllerInformation;
class UWidgetController;
class UHudWidget;
class UAbilitySystem;
class UMMOAttributeSet;
class AAbilitySystemPlayerState;
class ACharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHudTransitionDelegate);


/**
 * 
 */
UCLASS()
class SANDBOX_API APlayerHud : public AHUD
{
	GENERATED_BODY()

protected:
	/** The widget controller containing references to the player stats and information, referenced in every component */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UWidgetController> WidgetController;

	/** The current hud information that is being displayed to the player */
	UPROPERTY(BlueprintReadWrite) EHudState HudToDisplay;
	
	/**** In game ****/
	/** The heads up display in game */
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UHudWidget> PlayerOverlay;

	/**** Inventory and Equipment ****/
	// Inventory
	// Equipment
	// Stats

	/**** Settings ****/
	// Pause menu
	// Settings
	// Controls
	// System Settings


	/**** Custom settings ****/
	// Attribute information and adjustments (Or recreate Asc's attribute information)
	// Movement information


	/**** Widget Classes ****/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hud") TSubclassOf<UWidgetController> WidgetControllerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hud") TSubclassOf<UHudWidget> PlayerOverlayClass;

	
protected:
	virtual void BeginPlay() override;



public:
	/** Delegate for pending widgets and other things that need to be handled once the hud transitions to another display */
	FOnHudTransitionDelegate OnHudTransition;

	/** Initializes the player's hud and links everything to it's attributes */
	UFUNCTION(BlueprintCallable) virtual void InitializeHud(ACharacterBase* Player, APlayerController* Controller, AAbilitySystemPlayerState* PS, UAbilitySystem* Asc, UMMOAttributeSet* Attributes);

	/** Retrieves or creates the widget controller for access to the player's attributes */
	UFUNCTION(BlueprintCallable) UWidgetController* GetWidgetController(const FWidgetControllerInformation& WcParams);


//------------------------------------------------------------------------------------------------------//
// Hud Functionality																					//
//------------------------------------------------------------------------------------------------------//
public:
	/** Transition between the hud, settings, pause, inventory, etc */
	UFUNCTION(BlueprintCallable) virtual void DisplayHud(EHudState HudDisplayType);

	/** Checks if the widget controller is valid, and creates it if it isn't, and then updates the attribute values */
	UFUNCTION(BlueprintCallable) virtual void RefreshHud();

	/**
	 * Creates a widget and passes the default values to the widget
	 * @note the widget controller is the dynamic "controller" for a widget that's just used to handle your widget for whatever logic that's required for the widget
	 */
	UFUNCTION(BlueprintCallable) virtual UHudWidget* CreateHudWidget(TSubclassOf<UHudWidget> WidgetClass, UObject* WidgetControllerRef);
	
	

	
};
