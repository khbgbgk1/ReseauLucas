// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstanceSubsystem.h"

#include "LagRewindComponent.h"
#include "NetworkPlayerControllerComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogNetworkGameInstanceSubsystem, Log, All);

void UNetworkGameInstanceSubsystem::Host(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, bool bAbsolute)
{
	const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	
	FString SkinValue = FString::Printf(TEXT("?Skin=%d"), SelectedSkin);
	const FString Options = TEXT("listen") ;
	FString URL = Options + SkinValue;
	
	UGameplayStatics::OpenLevel(WorldContextObject, LevelName, bAbsolute, URL);
}

void UNetworkGameInstanceSubsystem::Join(const UObject* WorldContextObject, const FString& IPAddress)
{
	if (IPAddress.IsEmpty())
	{
		UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("Join: IP Address is empty!"));
		return;
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (PC)
	{
		FString SkinValue = FString::Printf(TEXT("?Skin=%d"), SelectedSkin);
		FString URL = IPAddress + SkinValue;
		PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
}

void UNetworkGameInstanceSubsystem::ServerTravel(const TSoftObjectPtr<UWorld> Level)
{
	if (Level.IsNull()) return;

	UWorld* World = GetWorld();
	if (World)
	{
		if (World->GetNetMode() < NM_Client)
		{
			FString LevelPath = FPackageName::ObjectPathToPackageName(Level.ToString());
			FString URL = LevelPath + TEXT("?listen");

			// 3. On lance le voyage
			World->ServerTravel(URL);
		}
		else
		{
			UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("ServerTravel: Tentative d'appel par un client ignorée."));
		}
	}
}

void UNetworkGameInstanceSubsystem::SetSelectedSkin(int NewSelectedSkin)
{
	SelectedSkin = NewSelectedSkin;
}

void UNetworkGameInstanceSubsystem::SyncServerTime(float ClientTimestamp, float ServerTimestamp)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float RTT = CurrentTime - ClientTimestamp;
	float AdjustedServerTime = ServerTimestamp + (RTT * 0.5f);
	float CurrentDelta = AdjustedServerTime - CurrentTime;
	
	//Save Pin en ms
	CurrentPing = (RTT * 0.5) * 1000.0f;
    
	if (CircularBufferOutliner.Num() <= NbElementCircularBufferOutliner)
	{
		CircularBufferOutliner.Add(CurrentDelta);
		ServerClientDelta = CurrentDelta;
	}
	else
	{
		CircularBufferOutliner[CircularBufferOutlinerIndex] = CurrentDelta;
		CircularBufferOutlinerIndex = (CircularBufferOutlinerIndex + 1) % NbElementCircularBufferOutliner;
		//ConsoleLogCircularBufferOutliner(CircularBufferOutliner);
		
		TArray<float> SortedTimeList = CircularBufferOutliner;
		SortedTimeList.Sort();
		
		float Sum = 0.f;
		for (int32 i = 1; i < NbElementCircularBufferOutliner && i < SortedTimeList.Num() - 1; ++i)
		{
			Sum += SortedTimeList[i];
		}
		
		ServerClientDelta = Sum / (SortedTimeList.Num() - 2);
	}
}

float UNetworkGameInstanceSubsystem::GetSyncedServerTime() const
{
	return GetWorld()->GetTimeSeconds() + ServerClientDelta;
}

void UNetworkGameInstanceSubsystem::ConsoleLogCircularBufferOutliner(TArray<float> CircularBufferOutlinerToLog)
{
	FString BufferContent = FString::JoinBy(CircularBufferOutlinerToLog, TEXT(" | "), [](float Val) { 
		   return FString::SanitizeFloat(Val, 4); 
	   });
	UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("Buffer Sync: [%s]"), *BufferContent);
}

float UNetworkGameInstanceSubsystem::GetCurrentPing() const
{
	return CurrentPing;
}

float UNetworkGameInstanceSubsystem::GetServerTripTime() const
{
	return CurrentPing/1000.0f; //time en seconds
}

bool UNetworkGameInstanceSubsystem::CheckActorsCollision(AActor* DamageInstigator,
                                                         AActor* ActorToGetRewindShapeCollision, float HitTime, FVector HitLocation, FVector StartLocation)
{
	if (!ActorToGetRewindShapeCollision) return false;

	ULagRewindComponent* RewindComp = ActorToGetRewindShapeCollision->FindComponentByClass<ULagRewindComponent>();
	if (RewindComp)
	{
		// On cherche le move le plus proche du HitTime envoyé par le client
		FSavedMove BestMove = RewindComp->GetInterpolatedMoveToTime(HitTime);
		bool bFound = BestMove.Time != -1.0f; // Valeur par défault dans la fonction GetClosestMoveToTime si rien de trouvé

		if (bFound)
		{
			UShapeComponent* RewindShape = RewindComp->ReferenceShapeForDrawDebug; 
			if (!RewindShape) return false;
			
			FTransform OriginalTransform = RewindShape->GetComponentTransform();
    
			RewindShape->SetWorldTransform(BestMove.Position);
			RewindShape->UpdateComponentToWorld(); // Force la mise à jour physique

			// RAYCAST pour verifier l'impact
			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(DamageInstigator); // On n'ignore pas la victime car on veut la toucher !
			
			FVector ShootDir = (HitLocation - StartLocation).GetSafeNormal();

			// On Allonge le tir
			FVector ExtendedEnd = HitLocation + (ShootDir * 100.0f);

			// On trace uniquement contre ce composant spécifique
			bool bHit = RewindShape->LineTraceComponent(
				HitResult, 
				StartLocation, 
				ExtendedEnd,
				Params
			);

			RewindShape->SetWorldTransform(OriginalTransform);
			RewindShape->UpdateComponentToWorld();

			// 5. Validation finale
			if (bHit)
			{
				if (DamageInstigator)
				{
					APawn* InstigatorPawn = Cast<APawn>(DamageInstigator);
					if (InstigatorPawn && InstigatorPawn->GetController())
					{
						UNetworkPlayerControllerComponent* NetComp = InstigatorPawn->GetController()->FindComponentByClass<UNetworkPlayerControllerComponent>();
						if (NetComp)
						{
							UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("Serveur: Envoi du debug au NetworkDebugComponent du tireur"));
							NetComp->Client_ReceiveRewindHit(ActorToGetRewindShapeCollision, BestMove.Position);
						}
					}
				}
				UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("Rewind VALIDÉ par Raycast local sur %s"), *ActorToGetRewindShapeCollision->GetName());
				return true;
			}
			else
			{
				UE_LOG(LogNetworkGameInstanceSubsystem, Warning, 
					TEXT("Rewind ÉCHOUÉ sur %s : Rayon [Start: %s -> Extend End: %s] | Shape au passé: %s"), 
					*ActorToGetRewindShapeCollision->GetName(),
					*StartLocation.ToString(), 
					*ExtendedEnd.ToString(), 
					*BestMove.Position.GetLocation().ToString()
				);				return false;
			}
		}
	}
	return false;
}

void UNetworkGameInstanceSubsystem::HostSessionBySteam(const TSoftObjectPtr<UWorld> Level)
{
	PendingStreamLevelName = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("Steam")); // Penser a mettre dans le build en shipping TonDossierDeBuild/Windows/TonNomDeProjet/Binaries/Win64/ le fichier steam_appid.txt avec le ID dev pour nous 480
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsDedicated= false;
			SessionSettings.bIsLANMatch = false; 
			
			SessionSettings.NumPublicConnections = 4;
			
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bAllowJoinViaPresence = true; //Tester false
			SessionSettings.bUseLobbiesIfAvailable= true; 
			SessionSettings.bAllowInvites= true;
			
			SessionSettings.bUsesPresence = true;
			SessionSettings.bShouldAdvertise = true;
            
			SessionSettings.Set(FName("MatchType"), FString("PadreLudos_Pigeon_2003_07"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

			// Liaison du delegate et création
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);
			SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings); // [cite: 243]
		}
	}else
	{
		UE_LOG(LogNetworkGameInstanceSubsystem, Error, TEXT("STEAM SUBSYSTEM INTROUVABLE ! Vérifie que Steam est lancé."));
	}
}

void UNetworkGameInstanceSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// Voyage vers le lobby en mode listen [cite: 246]
		if (PendingStreamLevelName.IsValid())
		{
			UGameplayStatics::OpenLevel(GetWorld(), PendingStreamLevelName, true, "listen");
		} else
		{
				UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("OnCreateSessionComplete: PendingStreamLevelName non valide"));
		}
	}
}