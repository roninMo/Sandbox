// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameModeLibrary.generated.h"

enum class EGameModeType : uint8;


/**
 * 
 */
UCLASS()
class SANDBOX_API UGameModeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	
//-------------------------------------------------------------------------------------//
// Utility																			   //
//-------------------------------------------------------------------------------------//
public:
	/** Retrieves the formatted string version of EGameModeType */
	UFUNCTION(BlueprintCallable, Category = "Combat Functions|Utils") static FString GameModeTypeToString(const EGameModeType GameModeType);

	
};
