// Fill out your copyright notice in the Description page of Project Settings.


#include "RewindNetworkManagerGameStateComponent.h"
#include "LagRewindComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogRewindNetworkManagerGameStateComponent, Log, All);


// Sets default values for this component's properties
URewindNetworkManagerGameStateComponent::URewindNetworkManagerGameStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void URewindNetworkManagerGameStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void URewindNetworkManagerGameStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URewindNetworkManagerGameStateComponent::Server_SetShowRewindHistory_Implementation(bool bNewValue)
{
	ShowRewindHistory = bNewValue;
	OnRep_ShowRewindHistory();
}


void URewindNetworkManagerGameStateComponent::OnRep_ShowRewindHistory()
{
	UE_LOG(LogRewindNetworkManagerGameStateComponent, Log, TEXT("OnRep_ShowRewindHistory: Valeur reçue = %s"), ShowRewindHistory ? TEXT("Vrai") : TEXT("Faux"));
    
	
	ULagRewindComponent* RewindComp = GetLagRewindComponentOnPlayer();
	
	if (RewindComp)
		RewindComp->Server_SetShowRewindHistory(ShowRewindHistory);
	
}

void URewindNetworkManagerGameStateComponent::Server_UpdateMaximunRecordTime_Implementation(float NewValue)
{
	MaximumRecordTime = NewValue;
	OnRep_UpdateMaximunRecordTime();
}

void URewindNetworkManagerGameStateComponent::OnRep_UpdateMaximunRecordTime()
{
    
	ULagRewindComponent* RewindComp = GetLagRewindComponentOnPlayer();
	
	if (RewindComp)
		RewindComp->  Server_UpdateMaximunRecordTime(MaximumRecordTime);
}

void URewindNetworkManagerGameStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// On réplique RemainingTime à tout le monde
	DOREPLIFETIME(URewindNetworkManagerGameStateComponent, ShowRewindHistory);
	DOREPLIFETIME(URewindNetworkManagerGameStateComponent, MaximumRecordTime);
}


ULagRewindComponent* URewindNetworkManagerGameStateComponent::GetLagRewindComponentOnPlayer()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogRewindNetworkManagerGameStateComponent, Warning, TEXT("GetLagRewindComponentOnPlayer: Impossible de récupérer le PlayerController local !"));
		return nullptr;
	}

	//Récupération du Pawn possédé par le PlayerController
	APawn* ControlledPawn = PC->GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogRewindNetworkManagerGameStateComponent, Warning, TEXT("GetLagRewindComponentOnPlayer: Aucun Pawn possédé pour le PlayerController [%s]"), *PC->GetName());
		return nullptr;
	}

	//Recherche si le Pawn a un RewindComp
	ULagRewindComponent* RewindComp = ControlledPawn->FindComponentByClass<ULagRewindComponent>();
	if (!RewindComp)
	{
		UE_LOG(LogRewindNetworkManagerGameStateComponent, Log, TEXT("GetLagRewindComponentOnPlayer: ULagRewindComponent introuvable sur le Pawn [%s]"), *ControlledPawn->GetName());
		return nullptr;
	}
	
	UE_LOG(LogRewindNetworkManagerGameStateComponent, Log, TEXT("GetLagRewindComponentOnPlayer: Composant trouvé avec succès sur [%s]."), *ControlledPawn->GetName());
	return RewindComp;
}


