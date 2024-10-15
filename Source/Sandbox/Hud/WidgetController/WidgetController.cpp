// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Hud/WidgetController/WidgetController.h"


void UWidgetController::SetWidgetControllerParams(const FWidgetControllerInformation& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}


void UWidgetController::BroadcastInitialValues()
{
}

void UWidgetController::BindCallbacksToDependencies()
{
}
