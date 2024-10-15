// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WidgetComponentBase.generated.h"

class UWidgetComponentBase;


// This is not a good idea for initialization because it generally happens before the values get initialized
// With things like rendering widgets from a widget component I'm not able to use onReps, and I think the good way to go about this would be through polling multiple pending values with onReps,
// and once they're all ready then run a function to initialize everything
// It's probably okay to do this on the Asc onrep though
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnWidgetComponentInitializedSignature, UWidgetComponentBase, OnWidgetComponentInitialized, UUserWidget*, Widget);


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SANDBOX_API UWidgetComponentBase : public UWidgetComponent
{
	GENERATED_BODY()

public:
	/** Called when the widget component has initialized it's widget */
	// UPROPERTY(BlueprintAssignable, Category="Initialization")
	// FOnWidgetComponentInitializedSignature OnWidgetComponentInitialized;

	/**  
	 *  Sets the widget to use directly. This function will keep track of the widget till the next time it's called
	 *	with either a newer widget or a nullptr
	 */ 
	virtual void SetWidget(UUserWidget* Widget) override;
	
	
};
