// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_GameState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogGame_GameState, Log, All);

AGame_GameState::AGame_GameState()
{
	// Création du composant
	RewindNetworkManager = CreateDefaultSubobject<URewindNetworkManagerGameStateComponent>(TEXT("RewindNetworkManager"));
}