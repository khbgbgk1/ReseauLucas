// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API ALobyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ALobyGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual FString  InitNewPlayer(
	APlayerController* NewPlayer,
	const FUniqueNetIdRepl& UniqueId,
	const FString& Options,
	const FString& Portal
) override;
	
	UFUNCTION(BlueprintCallable, Category = "Lobby Settings")
	void StartGame();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Settings")
	TSoftObjectPtr<UWorld> GameMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Settings")
	
	float TimerBeforeStart = 20.0f;

	FTimerHandle TimerHandle_GameStart;

	bool bIsTimerStarted = false;
	
};
