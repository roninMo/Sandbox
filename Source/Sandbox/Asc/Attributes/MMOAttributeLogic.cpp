// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/MMOAttributeLogic.h"


void UMMOAttributeLogic::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	FGAttributeSetExecutionData Props;
	GetExecutionData(Data, Props);
	
	/**

		Status calculations
			- status buildup
				- Status effect (Take damage / slow / poison)
				- Hit reaction

		Damage calculations
			- Take damage
			- Hit reaction based on the attack
			- Handle taking damage / dying

		Poise damage
			- Damage poise
			- Handle poise break / effect for regenerating poise
			- Handle hit reactions

		Any other effects to attributes
			- stamina drain, etc.


		Apply buildups / damages / poise damages, then the hit react, and handle any other logic after that

	 */


	// Is there a way to find out when we've finished handling the adjusted attributes of a single gameplay effect? They're sent in individually, (0) -> (0, 1) -> (0, 1, 2) -> (0, 1, 2, 3)
	// I don't want to handle any of the logic outside of attribute modifications until it's finished. One solution (if they always come in the same order) is to have an attribute to denote when to handle the calculations
	

	OnPostGameplayEffectExecute.Broadcast(Props);
	

	
	
}
