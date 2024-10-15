// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Hud/Hud/PlayerHud.h"

#include "Blueprint/UserWidget.h"
#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"
#include "Sandbox/Asc/Characters/AbilitySystemPlayerState.h"
#include "Sandbox/Characters/CharacterBase.h"
#include "Sandbox/Data/Enums/HudState.h"
#include "Sandbox/Hud/WidgetController/StatsWidgetController.h"
#include "Sandbox/Hud/Widgets/HudWidget.h"


void APlayerHud::BeginPlay()
{
	Super::BeginPlay();
}


void APlayerHud::InitializeHud(ACharacterBase* Player, APlayerController* Controller, AAbilitySystemPlayerState* PS, UAbilitySystem* Asc, UMMOAttributeSet* Attributes)
{
	checkf(WidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out the BaseHUD"));
	checkf(PlayerOverlayClass, TEXT("Player Overlay Class uninitialized, please fill out the BaseHUD"));

	// Delete the old widget references 
	if (PlayerOverlay) PlayerOverlay->HideWidget();
	// if (CharacterInventory) CharacterInventory->HideWidget();
	// if (CharacterEquipment) CharacterEquipment->HideWidget();
	// if (CharacterStats) CharacterStats->HideWidget();
	// if (CharacterScoreboard) CharacterScoreboard->HideWidget();
	// if (PauseMenu) PauseMenu->HideWidget();
	// if (SettingsMenu) SettingsMenu->HideWidget();
	
	// Initialize the overlay widget controller to broadcast the Asc attributes to other widgets
	RefreshHud();
	
	// Player overlay (hud, stats, health, and character information)
	if (!PlayerOverlay) PlayerOverlay = CreateHudWidget(PlayerOverlayClass, WidgetController);
	
	// Bind the values and broadcast the current asc information
	WidgetController->BroadcastInitialValues();
	DisplayHud(EHudState::Hud);
}



UWidgetController* APlayerHud::GetWidgetController(const FWidgetControllerInformation& WcParams)
{
	if (WidgetController) return WidgetController;
	
	WidgetController = NewObject<UStatsWidgetController>(this, WidgetControllerClass);
	WidgetController->SetWidgetControllerParams(WcParams);
	WidgetController->BindCallbacksToDependencies();
	
	return WidgetController;
}


void APlayerHud::DisplayHud(EHudState HudDisplayType)
{
	// Notify pending widgets the hud is about to transition
	OnHudTransition.Broadcast();

	// Hide the current widget
	if (EHudState::Hud == HudToDisplay && PlayerOverlay) PlayerOverlay->RemoveFromParent();

	// Transition to the current hud to display
	HudToDisplay = HudDisplayType;
	if (EHudState::Hud == HudToDisplay && PlayerOverlay) PlayerOverlay->AddToViewport();
}


void APlayerHud::RefreshHud()
{
	if (!WidgetController)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(GetOwningPawn());
		APlayerController* Controller = GetOwningPlayerController();
		UAbilitySystem* AbilitySystem = Character ? Character->GetAbilitySystem<UAbilitySystem>() : nullptr;
		AAbilitySystemPlayerState* PlayerState = Cast<AAbilitySystemPlayerState>(Character->GetPlayerState());
		UMMOAttributeSet* Attributes = PlayerState ? PlayerState->GetAttributeSet<UMMOAttributeSet>() : nullptr;

		const FWidgetControllerInformation WidgetControllerInformation(Character, AbilitySystem, Attributes, Controller, PlayerState);
		WidgetController = GetWidgetController(WidgetControllerInformation);
	}

	if (WidgetController)
	{
		WidgetController->BroadcastInitialValues();
	}
}


UHudWidget* APlayerHud::CreateHudWidget(TSubclassOf<UHudWidget> WidgetClass, UObject* WidgetControllerRef)
{
	UHudWidget* HudWidget = CreateWidget<UHudWidget>(GetWorld(), WidgetClass);
	HudWidget->SetWidgetController(WidgetControllerRef);
	return HudWidget;
}
