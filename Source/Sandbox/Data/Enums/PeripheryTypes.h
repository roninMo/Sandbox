// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PeripheryTypes.generated.h"


/**
 *	The periphery type based on the player and the periphery object (this helps with highlighting and other things)
 */
UENUM(BlueprintType)
enum class EPeripheryType : uint8
{
	None		 			UMETA(DisplayName = "None"),
	Passive    				UMETA(DisplayName = "Passive"),
	Enemy    				UMETA(DisplayName = "Enemy"),
	Ally		    		UMETA(DisplayName = "Ally"),
	Object    				UMETA(DisplayName = "Object")
};


/**
 *	Where should the periphery logic be handled? Server or client, or both?
 */
UENUM(BlueprintType)
enum class EHandlePeripheryLogic : uint8
{
	None		 		    UMETA(DisplayName = "None"),
	ServerAndClient    		UMETA(DisplayName = "ServerAndClient"),
	Server			    	UMETA(DisplayName = "Server"),
	Client		  			UMETA(DisplayName = "Client"),
};
