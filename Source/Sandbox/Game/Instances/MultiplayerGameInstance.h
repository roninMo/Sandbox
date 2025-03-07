// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MultiplayerGameInstance.generated.h"

class USave;


/**
 * 
 */
UCLASS()
class SANDBOX_API UMultiplayerGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	/** The current save information, stored on the game instance for retrieving save information when traveling between levels */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Instance|Save State") USave* CurrentSave;


public:
	/** Retrieves the current save from the game instance */
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual USave* GetCurrentSave();

	/** Sets the current save on the game instance */
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual void SetCurrentSave(USave* Save);

};
