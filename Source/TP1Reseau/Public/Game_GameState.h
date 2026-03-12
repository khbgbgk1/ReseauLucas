// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RewindNetworkManagerGameStateComponent.h"
#include "Game_GameState.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API AGame_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AGame_GameState();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network", meta = (AllowPrivateAccess = "true"))
	URewindNetworkManagerGameStateComponent* RewindNetworkManager;
	
};
