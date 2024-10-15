// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Hud/Widgets/HudWidget.h"
#include "Sandbox/Hud/Hud/PlayerHud.h"


APlayerHud* UHudWidget::GetHud()
{
	APlayerController* Controller = GetOwningPlayer();
	return Controller ? Controller->GetHUD<APlayerHud>() : nullptr;
}
