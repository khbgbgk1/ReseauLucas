// Fill out your copyright notice in the Description page of Project Settings.


#include "LobyGameState.h"
#include "TP1ReseauCharacter.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogLobyGameState, Log, All);

void ALobyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// On réplique RemainingTime à tout le monde
	DOREPLIFETIME(ALobyGameState, RemainingTime);
	DOREPLIFETIME(ALobyGameState, LevelStartTime);
}

ALobyGameState::ALobyGameState()
{
	RewindNetworkManager = CreateDefaultSubobject<URewindNetworkManagerGameStateComponent>(TEXT("RewindNetworkManager"));
}