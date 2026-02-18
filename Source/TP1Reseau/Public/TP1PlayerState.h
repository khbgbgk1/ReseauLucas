// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TP1PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API ATP1PlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	
	virtual void PostNetInit() override;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skin")
	int32 SelectedSkin = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void CopyProperties(APlayerState* PlayerState) override;
};
