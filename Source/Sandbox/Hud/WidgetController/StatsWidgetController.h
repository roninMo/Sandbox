// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "StatsWidgetController.generated.h"

class UDataTable;
class UBaseWidget;
struct FOnAttributeChangeData;

// USTRUCT(BlueprintType)
// struct FUIWidgetRow : public FTableRowBase
// {
// 	GENERATED_BODY()
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag MessageTag = FGameplayTag();
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Message = FText();
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UBaseWidget> MessageWidget;
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) UTexture2D* Image = nullptr;
// };

// Attribute Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

// Gameplay tag widget message Delegates
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);


/**
 * This gives widgets quick access to the Asc, and delegate bindings of the attributes without having to recreate this for every widget
 */
UCLASS()
class SANDBOX_API UStatsWidgetController : public UWidgetController
{
	GENERATED_BODY()

protected:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Widget Data") TObjectPtr<UDataTable> MessageWidgetDataTable;

	
public:
	/* Bind to the ability system attribute delegates to broadcast the value changes for specific widgets.. */
	virtual void BindCallbacksToDependencies() override;
	
	/* Broadcast the initial attribute values once the widget has been created. */
	virtual void BroadcastInitialValues() override;

	/**
	 * Gets the widget message information based on the corresponding gameplay tag.
	 * @note: This is intended for the Widget Message Information data table.
	*/
	// template<typename T>
	// T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	
	// Delegate events
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnHealthRegenRateChanged; 
	
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnStaminaChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnMaxStaminaChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnStaminaRegenRateChanged; 
	
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnPoiseChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnMaxPoiseChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnPoiseRegenRateChanged; 
	
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnMaxManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "Attributes") FOnAttributeChangedSignature OnManaRegenRateChanged; 

	// UPROPERTY(BlueprintAssignable, Category = "Messages") FMessageWidgetRowSignature OnMessageWidgetRow;

	
};


// template <typename T>
// T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
// {
// 	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT("Widget Information"));
// }
