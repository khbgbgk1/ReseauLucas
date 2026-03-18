// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerControllerComponent.h"
#include "TimerManager.h"
#include "LagRewindComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogNetworkPlayerControllerComponent, Log, All);


// Sets default values for this component's properties
UNetworkPlayerControllerComponent::UNetworkPlayerControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UNetworkPlayerControllerComponent::Client_ReceiveRewindHit_Implementation(AActor* VictimActor,
	FTransform VictimTransform)
{
	if (!VictimActor)
	{
		UE_LOG(LogNetworkPlayerControllerComponent, Warning, TEXT("NetworkDebugComponent: VictimActor est null !"));
		return;
	}

	// On cherche le LagRewindComponent sur l'acteur que le serveur nous a désigné comme victime
	ULagRewindComponent* RewindComp = VictimActor->FindComponentByClass<ULagRewindComponent>();
	if (RewindComp)
	{
		UE_LOG(LogNetworkPlayerControllerComponent, Log, TEXT("NetworkDebugComponent: Envoi de l'ordre de dessin à la victime %s"), *VictimActor->GetName());
		RewindComp->DrawValidationHit(VictimTransform);
	}
	else
	{
		UE_LOG(LogNetworkPlayerControllerComponent, Warning, TEXT("NetworkDebugComponent: %s n'a pas de LagRewindComponent !"), *VictimActor->GetName());
	}
}


// Called when the game starts
void UNetworkPlayerControllerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningPlayerController = Cast<APlayerController>(GetOwner());
    
	if (!OwningPlayerController)
	{
		UE_LOG(LogNetworkPlayerControllerComponent, Error, TEXT("BeginPlay: Owner n'est pas un APlayerController !"));
	}
	
	//Plus opti si on change reset apres le timer une fois le jeu chargé
	if (OwningPlayerController->IsLocalController())
	{
		OwningPlayerController->GetWorldTimerManager().SetTimer(TimerHandle_Sync, this, &UNetworkPlayerControllerComponent::SyncTime, 0.1f, true);
	}
	
}

void UNetworkPlayerControllerComponent::Server_RequestServerTime_Implementation(float ClientTimestamp)
{
	// On renvoie l'heure du monde
	Client_ReportServerTime(ClientTimestamp, GetWorld()->GetTimeSeconds());
}

void UNetworkPlayerControllerComponent::Client_ReportServerTime_Implementation(float ClientTimestamp, float ServerTimestamp)
{
	UNetworkGameInstanceSubsystem* NetworkSubsystem = GetNetworkSubsystem();
            
	if (NetworkSubsystem)
	{
		NetworkSubsystem->SyncServerTime(ClientTimestamp, ServerTimestamp);
	}
}

void UNetworkPlayerControllerComponent::SyncTime()
{
	// On n'exécute ça que sur le client local
	if (OwningPlayerController && OwningPlayerController->IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

UNetworkGameInstanceSubsystem* UNetworkPlayerControllerComponent::GetNetworkSubsystem()
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


// Called every frame
void UNetworkPlayerControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

