// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Hud/WidgetController/StatsWidgetController.h"

#include "Sandbox/Asc/AbilitySystem.h"
#include "Sandbox/Asc/Attributes/MMOAttributeSet.h"


void UStatsWidgetController::BindCallbacksToDependencies()
{
	if (!AttributeSet || !AbilitySystemComponent)
	{
		if (!AttributeSet) UE_LOG(LogTemp, Error, TEXT("%s() %s: The widget controller did not have access to the attribute set to bind it's values!"), *FString(__FUNCTION__), *GetName());
		if (!AbilitySystemComponent) UE_LOG(LogTemp, Error, TEXT("%s() %s: The widget controller did not have access to the ability system to bind it's values!"), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Binding to attribute callbacks is easy, here's how you do it through delegates or lambdas. It takes more configuration to do it through lambdas, so that's how we'll go about it.
	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &UStatsWidgetController::HealthChanged);
	// void UStatsWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const { OnHealthChanged.Broadcast(Data.NewValue); } // There's two delegate bindings here so don't get confused, the broadcast is just to send information to the controller
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthRegenRateAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnHealthRegenRateChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnStaminaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxStaminaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaRegenRateAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnStaminaRegenRateChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetPoiseAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnPoiseChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxPoiseAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxPoiseChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetPoiseRegenRateAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnPoiseRegenRateChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaRegenRateAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnManaRegenRateChanged.Broadcast(Data.NewValue); });

	// Function to bind messages to activated effects through delegates
	// Cast<UAbilitySystem>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	// 	[this](const FGameplayTagContainer& AssetTags)
	// 	{
	// 		if (!MessageWidgetDataTable) return;
	// 		for (const FGameplayTag& Tag : AssetTags)
	// 		{
	// 			// For example, say that Tag = Message.HealthPotion
	// 			// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
	// 			FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("UI.Message"));
	// 			if (Tag.MatchesTag(MessageTag))
	// 			{
	// 				if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag))
	// 				{
	// 					UE_LOG(LogTemp, Warning, TEXT("widget message information: %s"), *Row->Message.ToString());
	// 					OnMessageWidgetRow.Broadcast(*Row);
	// 				}
	// 			}
	// 		}
	// 	}
	// );
}


void UStatsWidgetController::BroadcastInitialValues()
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() %s: The widget controller did not have access to the attribute set to broadcast it's values!"), *FString(__FUNCTION__), *GetName());
		return;
	}
	
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
	OnHealthRegenRateChanged.Broadcast(AttributeSet->GetHealthRegenRate());
	
	OnStaminaChanged.Broadcast(AttributeSet->GetStamina());
	OnMaxStaminaChanged.Broadcast(AttributeSet->GetMaxStamina());
	OnStaminaRegenRateChanged.Broadcast(AttributeSet->GetStaminaRegenRate());
	
	OnPoiseChanged.Broadcast(AttributeSet->GetPoise());
	OnMaxPoiseChanged.Broadcast(AttributeSet->GetMaxPoise());
	OnPoiseRegenRateChanged.Broadcast(AttributeSet->GetPoiseRegenRate());
	
	OnManaChanged.Broadcast(AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AttributeSet->GetMaxMana());
	OnManaRegenRateChanged.Broadcast(AttributeSet->GetManaRegenRate());
}
