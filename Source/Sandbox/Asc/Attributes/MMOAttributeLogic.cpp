// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandbox/Asc/Attributes/MMOAttributeLogic.h"

#include "GameplayEffectExtension.h"
#include "Logging/StructuredLog.h"


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

		Handling individual calculations isn't as complex as having everything together, there's a lot more of interesting things and calculations with multiple attributes.
		You just need to clear out the old attributes and help handle some of the basic values for handling damage


	 */


	if (Data.EvaluatedData.Attribute == GetDamage_StandardAttribute())
	{
		UE_LOGFMT(LogTemp, Log, "{0}::{1}() {2} dealt {3} damage to {4}!", *UEnum::GetValueAsString(Props.SourceActor->GetLocalRole()), *FString(__FUNCTION__),
			*GetNameSafe(Props.SourceActor), GetDamage_Standard(), *GetNameSafe(Props.TargetActor));
	}

	

	

	OnPostGameplayEffectExecute.Broadcast(Props);
	

	
	
}
