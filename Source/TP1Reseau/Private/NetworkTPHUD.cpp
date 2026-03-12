// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkTPHUD.h"

#include "Blueprint/UserWidget.h"

void ANetworkTPHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
			PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible); 
		}
	}
}

void ANetworkTPHUD::PrintPlayerHUD(bool bShow)
{
	PrintUI(bShow,PlayerHUDWidget);
}

void ANetworkTPHUD::PrintUI(bool bShow,UUserWidget* UserWidget)
{
	if (UserWidget)
	{
		ESlateVisibility NewVisibility = bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
		UserWidget->SetVisibility(NewVisibility);
	}
}