// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NetworkTPHUD.generated.h"

/**
 * 
 */
UCLASS()
class TP1RESEAU_API ANetworkTPHUD : public AHUD
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHUDClass;
	
	UPROPERTY()
	UUserWidget* PlayerHUDWidget;

	// Fonction pour afficher/cacher le menu
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PrintPlayerHUD(bool bShow);
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PrintUI(bool bShow,UUserWidget* UserWidget);
	
};
