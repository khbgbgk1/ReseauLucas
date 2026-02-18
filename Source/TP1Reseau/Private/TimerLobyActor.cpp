// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerLobyActor.h"
#include "NetworkGameInstanceSubsystem.h"
#include "LobyGameState.h"


// Sets default values
ATimerLobyActor::ATimerLobyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATimerLobyActor::BeginPlay()
{
	Super::BeginPlay();
	StartTimer();
	
}

void ATimerLobyActor::StartTimer()
{
	GetWorldTimerManager().SetTimer(
		TimerHandle_UpdateDisplay, 
		this, 
		&ATimerLobyActor::UpdateTimerLogic, 
		UpdateInterval, 
		true
	);
}

void ATimerLobyActor::UpdateTimerLogic()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 1. Récupération du temps synchronisé via le Subsystem
	if (UGameInstance* GI = World->GetGameInstance())
	{
		UNetworkGameInstanceSubsystem* NetworkSubsystem = GI->GetSubsystem<UNetworkGameInstanceSubsystem>();
		if (NetworkSubsystem)
		{
			CurrentSyncedTime = NetworkSubsystem->GetSyncedServerTime();
		}
	}

	// 2. Calcul du compte à rebours via le GameState
	if (ALobyGameState* GS = World->GetGameState<ALobyGameState>())
	{
		if (GS->LevelStartTime > 0.01f)
		{
			float TimeElapsed = CurrentSyncedTime - GS->LevelStartTime;
			float CountDown = GS->RemainingTime - TimeElapsed;

			// Clamp à 0 pour éviter les chiffres négatifs
			ServerTime = FMath::Max(0.0f, CountDown);
		}
		else
		{
			// Valeur par défaut tant que le serveur n'a pas répliqué StartTime
			ServerTime = GS->RemainingTime;
		}
	}
}

// Called every frame
void ATimerLobyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

