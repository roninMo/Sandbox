// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	/** The current save game reference for saving the game */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerState|Saving and Loading") FString SaveUrl;
	
	/** The current save index of a specific save slot */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerState|Saving and Loading") int32 SaveIndex = -1;

	/** The current save slot for a specific character's saves */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerState|Saving and Loading") int32 SaveSlot = -1;

	/** A list of save game references for this specific Save slot. Beginning from the current save game to the previous saves */


public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	ABasePlayerState(const FObjectInitializer& ObjectInitializer);

	/**
	 * Sends a notification to the game mode to save the current game state.
	 * If there's valid save logic for the game mode, it saves the current state and updates the save iteration for save information to be handled dynamically 
	 *
	 * @param Ref The specific slot we're saving to
	 * @param Index The index that we're saving to
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Player State|Saving and Loading") virtual void Server_SaveGame(const FString& Ref, const int32 Index);



	
//----------------------------------------------------------------------------------//
// Utility																			//
//----------------------------------------------------------------------------------//
public:
	/** Retrieves the current SaveGameRef */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving and Loading|Utility") virtual FString GetSaveGameRef() const;

	/** Retrieves the current SaveIndex */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving and Loading|Utility") virtual int32 GetSaveIndex() const;

	/** Retrieves the current SaveSlot */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving and Loading|Utility") virtual int32 GetSaveSlot() const;

	/** Utility for setting SaveGameRef, only call on server */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving and Loading|Utility") virtual void SetSaveUrl(FString Ref);
	
	/** Utility for setting SaveIndex, only call on server */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving and Loading|Utility") virtual void SetSaveIndex(int32 Index);
	
	/** Utility for setting Slot, only call on server */
	UFUNCTION(BlueprintCallable, Category = "Player State|Saving and Loading|Utility") virtual void SetSaveSlot(int32 Slot);

	
};
