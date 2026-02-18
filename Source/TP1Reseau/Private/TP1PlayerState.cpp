// Fill out your copyright notice in the Description page of Project Settings.


#include "TP1PlayerState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogTP1PlayerState, Log, All);


void ATP1PlayerState::PostNetInit()
{
	Super::PostNetInit();
    
	// À ce stade, les variables initiales répliquées devraient être arrivées
	UE_LOG(LogTP1PlayerState, Log, TEXT("PostNetInit: SelectedSkin reçu : %d"), SelectedSkin);
}

void ATP1PlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATP1PlayerState, SelectedSkin);
}

void ATP1PlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// On cast comme ATP1PlayerState
	ATP1PlayerState* NewPlayerState = Cast<ATP1PlayerState>(PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->SelectedSkin = this->SelectedSkin;
	}
}