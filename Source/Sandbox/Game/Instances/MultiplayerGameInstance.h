// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MultiplayerGameInstance.generated.h"

enum class EGameModeType : uint8;
class USaved_Level;
class USave;


/**
 * 
 */
UCLASS()
class SANDBOX_API UMultiplayerGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	/** The  */
	/** The Game Mode's classification */
	UPROPERTY(BlueprintReadWrite, Category="Game Instance|Save State") EGameModeType GameModeType;

	/** The current save information, stored on the game instance for retrieving save information when traveling between levels */
	UPROPERTY(BlueprintReadWrite, Category = "Game Instance|Save State") USave* CurrentSave;

	/** A stored reference to the save game slot for the current level */
	UPROPERTY(BlueprintReadWrite, Category= "Game Instance|Save State") TObjectPtr<USaved_Level> CurrentLevelSave;

public:
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual EGameModeType GetGameModeType();
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual USave* GetCurrentSave();
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual USaved_Level* GetCurrentLevelSave();
	
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual void SetGameModeType(EGameModeType Type);
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual void SetCurrentSave(USave* Save);
	UFUNCTION(BlueprintCallable, Category = "Game Instance|Save State") virtual void SetCurrentLevelSave(USaved_Level* LevelSave);

};
