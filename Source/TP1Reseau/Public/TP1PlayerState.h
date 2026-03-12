// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Skin_GameState_Component.h"
#include "TP1PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API ATP1PlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ATP1PlayerState();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkin_GameState_Component* Skin_GS_Component;
	
	virtual void PostNetInit() override;
	
	virtual void CopyProperties(APlayerState* PlayerState) override;
};
