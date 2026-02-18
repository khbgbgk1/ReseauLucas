// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobyGameMode.h"

#include "GenericTeamAgentInterface.h"
#include "LobyGameState.h"
#include "TP1PlayerState.h"
#include "TP1ReseauCharacter.h"
#include "NetworkGameInstanceSubsystem.h"
#include "TP1ReseauPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogLobyGameMode, Log, All);


ALobyGameMode::ALobyGameMode()
{
	// Désactive le voyage fluide (Seamless Travel)
	bUseSeamlessTravel = true;
	PlayerStateClass = ATP1PlayerState::StaticClass();
}

void ALobyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// On récupère le nombre actuel de joueurs
	int NombreDeJoueurs = GetNumPlayers();
	
	//Event pour l'UI

	UE_LOG(LogLobyGameMode, Log, TEXT("Joueur connecté ! Nombre actuel : %d"), NombreDeJoueurs);
	
	if (NombreDeJoueurs >= 2 && !bIsTimerStarted)
	{
		bIsTimerStarted = true;
		if (ALobyGameState* GS = GetGameState<ALobyGameState>())
		{
			GS->LevelStartTime = GetWorld()->GetTimeSeconds();
			GS->RemainingTime = TimerBeforeStart; 
          
			UE_LOG(LogLobyGameMode, Log, TEXT("Chrono déclenché sur le GameState à %f"), GS->LevelStartTime);
		}
        
		// On lance un timer qui appelle UpdateCountdown toutes les 0.01 secondes
		GetWorldTimerManager().SetTimer(TimerHandle_GameStart, this, &ALobyGameMode::StartGame, TimerBeforeStart, false);
        
	}
}

FString ALobyGameMode::InitNewPlayer(
	APlayerController* NewPlayer,
	const FUniqueNetIdRepl& UniqueId,
	const FString& Options,
	const FString& Portal
)
{
	// IMPORTANT : récupérer le retour de la classe mère
	const FString Error = Super::InitNewPlayer(NewPlayer, UniqueId, Options, Portal);

	// 🔹 Lecture de l'option Skin
	const FString SkinOption = UGameplayStatics::ParseOption(Options, TEXT("Skin"));

	if (!SkinOption.IsEmpty())
	{
		const int SkinIndex = FCString::Atoi(*SkinOption);

		UE_LOG(LogLobyGameMode, Log, TEXT("Skin reçu via URL : %d"), SkinIndex);

		
		ATP1PlayerState* PS = NewPlayer	? Cast<ATP1PlayerState>(NewPlayer->PlayerState)	: nullptr;

		if (PS)
		{
			PS->SelectedSkin = SkinIndex;
		}
		
	}

	return Error; 
}



void ALobyGameMode::StartGame()
{
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ATP1ReseauPlayerController* PC = Cast<ATP1ReseauPlayerController>(It->Get()))
		{
			PC->Client_ShowLoadingScreen();
		}
	}
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
	
		if (!GameMap.IsNull())
		{
			if (UWorld* World = GetWorld())
			{
				if (UGameInstance* GI = World->GetGameInstance())
				{
					UNetworkGameInstanceSubsystem* NetworkSubsystem = GI->GetSubsystem<UNetworkGameInstanceSubsystem>();
	            
					if (NetworkSubsystem)
					{
						NetworkSubsystem->ServerTravel(GameMap);
					}
				}
			}
			
		}
		else
		{
			UE_LOG(LogLobyGameMode, Error, TEXT("GameMap n'est pas configurée dans le GameMode !"));
		}
	}, 0.2f, false);
}