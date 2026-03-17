// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstanceSubsystem.h"

#include "LagRewindComponent.h"
#include "NetworkPlayerControllerComponent.h"
#include "Kismet/GameplayStatics.h"
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

bool UNetworkGameInstanceSubsystem::CheckActorsCollision(AActor* DamageInstigator,
	AActor* ActorToGetRewindShapeCollision, float HitTime, FVector HitLocation)
{
	if (!ActorToGetRewindShapeCollision) return false;

	ULagRewindComponent* RewindComp = ActorToGetRewindShapeCollision->FindComponentByClass<ULagRewindComponent>();
	if (RewindComp)
	{
		// On cherche le move le plus proche du HitTime envoyé par le client
		FSavedMove BestMove = RewindComp->GetClosestMoveToTime(HitTime);
		bool bFound = BestMove.Time != -1.0f; // Valeur par défault dans la fonction GetClosestMoveToTime si rien de trouvé

		if (bFound)
		{
			// --- VERIFICATION ---
			// On vérifie si le point d'impact du client est dans la capsule serveur à ce moment
			// Calcul simple : Distance entre le centre de la capsule rembobinée et le point d'impact
			float Distance = FVector::Dist(BestMove.Position.GetLocation(), HitLocation);
            
			// Seuil de tolérance (Rayon capsule ~42 + marge d'erreur/vitesse ~30)
			const float Tolerance = 85.0f; 

			if (Distance <= Tolerance)
			{
				//Draw la shape chez le client DU Hit
				if (DamageInstigator)
				{
					APawn* InstigatorPawn = Cast<APawn>(DamageInstigator);
					if (InstigatorPawn && InstigatorPawn->GetController())
					{
						UNetworkPlayerControllerComponent* DebugComp = InstigatorPawn->GetController()->FindComponentByClass<UNetworkPlayerControllerComponent>();
						if (DebugComp)
						{
							UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("Serveur: Envoi du debug au NetworkDebugComponent du tireur"));
							DebugComp->Client_ReceiveRewindHit(ActorToGetRewindShapeCollision, BestMove.Position);
						}
					}
				}
				
				UE_LOG(LogNetworkGameInstanceSubsystem, Log, TEXT("Rewind Success: Dist %f <= %f"), Distance, Tolerance);
				return true;
			}
			else
			{
				UE_LOG(LogNetworkGameInstanceSubsystem, Warning, TEXT("Rewind REJECTED: Dist %f trop grande !"), Distance);
				return false;
			}
		}
	}
	return false;
}
