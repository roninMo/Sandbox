// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Hud/WidgetComponents/WidgetComponentBase.h"


void UWidgetComponentBase::SetWidget(UUserWidget* InWidget)
{
	Super::SetWidget(InWidget);
	
	// if (InWidget == nullptr) return;
	// OnWidgetComponentInitialized.Broadcast(InWidget);
}
