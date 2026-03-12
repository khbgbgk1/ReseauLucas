// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RewindNetworkManagerGameStateComponent.h"
#include "LobyGameState.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API ALobyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	ALobyGameState();
	
	// Variable répliquée qui contient le temps
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
	float RemainingTime = 20.0f;
	
	UPROPERTY(Replicated)
	float LevelStartTime = 0.0f;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network", meta = (AllowPrivateAccess = "true"))
	URewindNetworkManagerGameStateComponent* RewindNetworkManager;
};
