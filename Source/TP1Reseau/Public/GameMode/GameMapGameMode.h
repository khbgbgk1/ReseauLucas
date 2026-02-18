// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API AGameMapGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AGameMapGameMode();
	
	int NbPlayerExpect = 3 ;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	UFUNCTION()
	void StartMatchNow();
};
