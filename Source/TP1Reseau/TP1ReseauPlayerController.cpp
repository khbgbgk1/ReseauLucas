// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP1ReseauPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "TP1PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "TP1Reseau.h"
#include "TP1ReseauCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Input/SVirtualJoystick.h"

DEFINE_LOG_CATEGORY_STATIC(LogTP1ReseauPlayerController, Log, All);

ATP1ReseauPlayerController::ATP1ReseauPlayerController()
{
	NetworkComponent = CreateDefaultSubobject<UNetworkPlayerControllerComponent>(TEXT("NetworkComponent"));
}

void ATP1ReseauPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogTP1Reseau, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
	
	//Plus opti si on change reset apres le timer une fois le jeu chargé
	GetWorldTimerManager().SetTimer(TimerHandle_Sync, this, &ATP1ReseauPlayerController::SyncTime, 0.1f, true);
}

void ATP1ReseauPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ATP1ReseauPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ATP1ReseauCharacter* CurrentPlayer = Cast<ATP1ReseauCharacter>(InPawn);
	if (!CurrentPlayer) return;

	ATP1PlayerState* PS = GetPlayerState<ATP1PlayerState>();
	if (CurrentPlayer && PS && CurrentPlayer->SkinComponent)
	{
		// On cherche les composants sur les deux entités
		USkinComponent* CharSkinComp = CurrentPlayer->SkinComponent;
		USkin_GameState_Component* PSSkinComp = PS->FindComponentByClass<USkin_GameState_Component>();

		if (CharSkinComp && PSSkinComp)
		{
			// Le serveur définit le skin du personnage en fonction de la donnée stockée dans le PS
			CharSkinComp->SetSkin(PSSkinComp->SelectedSkin);
			UE_LOG(LogTP1ReseauPlayerController, Log, TEXT("OnPossess: Skin synchronisé depuis le PlayerState Component"));
		}
		
	}

}

void ATP1ReseauPlayerController::Server_RequestServerTime_Implementation(float ClientTimestamp)
{
	// On renvoie l'heure du monde
	Client_ReportServerTime(ClientTimestamp, GetWorld()->GetTimeSeconds());
}

void ATP1ReseauPlayerController::Client_ReportServerTime_Implementation(float ClientTimestamp, float ServerTimestamp)
{
	UNetworkGameInstanceSubsystem* NetworkSubsystem = GetNetworkSubsystem();
            
	if (NetworkSubsystem)
	{
		NetworkSubsystem->SyncServerTime(ClientTimestamp, ServerTimestamp);
	}
}

void ATP1ReseauPlayerController::SyncTime()
{
	// On n'exécute ça que sur le client local
	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

UNetworkGameInstanceSubsystem* ATP1ReseauPlayerController::GetNetworkSubsystem()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<UNetworkGameInstanceSubsystem>();
		}
	}
	return nullptr;
}

void ATP1ReseauPlayerController::Client_ShowLoadingScreen_Implementation()
{
	if (!LoadingWidgetInstance && LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
		if (LoadingWidgetInstance)
		{
			// ZOrder élevé (ex: 100) pour être sûr qu'il soit au-dessus de tout
			LoadingWidgetInstance->AddToViewport(100);
            
			// On fige les inputs pour éviter que le joueur ne bouge pendant le sas
			// FInputModeUIOnly InputMode;
			// InputMode.SetWidgetToFocus(LoadingWidgetInstance->TakeWidget());
			// SetInputMode(InputMode);
			// bShowMouseCursor = true;
		}
	}
}

void ATP1ReseauPlayerController::Client_HideLoadingScreen_Implementation()
{
	if (LoadingWidgetInstance)
	{
		LoadingWidgetInstance->RemoveFromParent();
		LoadingWidgetInstance = nullptr;

		// On rend les contrôles au joueur
		// FInputModeGameOnly InputMode;
		// SetInputMode(InputMode);
		// bShowMouseCursor = false;
	}
}

void ATP1ReseauPlayerController::Server_ApplyDamage_Implementation(int32 Damages, AActor* DamageInstigator,ATP1ReseauCharacter* CharacterToKill, float HitTime , FVector HitLocation)
{
	if (!CharacterToKill)
	{
			UE_LOG(LogTP1ReseauPlayerController, Log, TEXT("Server_ApplyDamage_Implementation: CharacterToKill non valide"));
		return;
	}
	//VERIFIER SI la scene est bonne avant de valider le kill
	UNetworkGameInstanceSubsystem* NetworkSubsystem = GetNetworkSubsystem();
            
	if (NetworkSubsystem)
	{
		bool CharacterIsTouched = NetworkSubsystem->CheckActorsCollision(DamageInstigator, CharacterToKill, HitTime, HitLocation);
		
		if (CharacterIsTouched)
		{
			CharacterToKill->ApplyDamageOnPlayer(Damages, DamageInstigator, HitTime, HitLocation);
		}
	}
	
	
}