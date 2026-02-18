// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobyGameState.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API ALobyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	// Variable répliquée qui contient le temps
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
	float RemainingTime = 20.0f;
	
	UPROPERTY(Replicated)
	float LevelStartTime = 0.0f;
	

	// Configuration de la réplication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
    
	// Heure précise synchronisée (utilisable par l'UI)
	
	//float GetSyncedServerTime() const;
};
