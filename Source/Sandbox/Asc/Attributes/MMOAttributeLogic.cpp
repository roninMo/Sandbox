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
				- Hit reactionz

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


	

	

	OnPostGameplayEffectExecute.Broadcast(Props);
	

	
	
}
