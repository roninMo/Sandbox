// Fill out your copyright notice in the Description page of Project Settings.


#include "Save_Level.h"

#include "Kismet/GameplayStatics.h"
#include "Sandbox/Characters/Components/Saving/SaveComponent.h"
#include "Sandbox/World/CustomLevelScriptActor.h"

bool USave_Level::SaveData_Implementation()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	ACustomLevelScriptActor* Level = Cast<ACustomLevelScriptActor>(World->GetLevelScriptActor());
	if (!Level) return false;

	// Save the character's information pertaining to the world separately here
	

	// Save level specific information pertaining to the character's game mode / level
	return Level->SaveLevelInformation(Cast<USaveComponent>(GetOuter()), this);
}
