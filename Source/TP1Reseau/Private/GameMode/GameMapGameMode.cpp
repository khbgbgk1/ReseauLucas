// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/GameMapGameMode.h"

#include "TP1ReseauPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameMapGameMode, Log, All);


AGameMapGameMode::AGameMapGameMode()
{
	// Désactive le voyage fluide (Seamless Travel)
	bUseSeamlessTravel = true;
}

void AGameMapGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (ATP1ReseauPlayerController* PC = Cast<ATP1ReseauPlayerController>(NewPlayer))
	{
		PC->Client_ShowLoadingScreen();
	}

	//Vérifier si tout le monde est là
	if (GetNumPlayers() >= NbPlayerExpect) 
	{
		UE_LOG(LogGameMapGameMode, Log, TEXT("Tout le monde est là, lancement imminent..."));
        
		// Petit délai pour laisser le temps aux clients de finir le rendu initial
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AGameMapGameMode::StartMatchNow, 2.0f, false);
	}
}

void AGameMapGameMode::StartMatchNow()
{

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (ATP1ReseauPlayerController* PCReseau = Cast<ATP1ReseauPlayerController>(PC))
			{
				PCReseau->Client_HideLoadingScreen();
				UE_LOG(LogGameMapGameMode, Log, TEXT("StartMatchNow: On cache"));
				
			}
		}
	}
}

void AGameMapGameMode::RequestPlayerRespawn(AController* Controller)
{
	if (!Controller)
	{
		UE_LOG(LogGameMapGameMode, Log, TEXT("RequestPlayerRespawn: controller non valide."));
		return;

	}
	//On récupère l'actuel Pawn possédé par ce contrôleur
	APawn* OldPawn = Controller->GetPawn();
	Controller->UnPossess();
	
	if (OldPawn)
	{
		// On rend invisible l<Anien corp
		OldPawn->SetActorEnableCollision(false);
		OldPawn->Destroy(); 
	}
	
	AActor* SpawnPoint = Controller->StartSpot.Get();
	if (!SpawnPoint)
	{
		SpawnPoint = FindPlayerStart(Controller);
		Controller->StartSpot = SpawnPoint;
	}
	
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (SpawnPoint)
	{
		SpawnLocation = SpawnPoint->GetActorLocation();
		SpawnRotation = SpawnPoint->GetActorRotation();
	}

	//On Restart le joueur
	//RestartPlayer(Controller);

	// Une fois le nouveau Pawn spawn et possédé, on destroy l'ancien
	UClass* PawnClass = GetDefaultPawnClassForController(Controller);
	if (PawnClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (NewPawn)
		{
			Controller->Possess(NewPawn);
			if (OldPawn)
			{
				OldPawn->Destroy();
				UE_LOG(LogGameMapGameMode, Log, TEXT("RequestPlayerRespawn: SUCCESS | Controller: [%s] | New Pawn: [%s] | At: [%s] | Loc: %s"), 
				*Controller->GetName(), 
				*GetNameSafe(NewPawn),
				*GetNameSafe(SpawnPoint),
				*SpawnLocation.ToString());
			}

		}
	}
}