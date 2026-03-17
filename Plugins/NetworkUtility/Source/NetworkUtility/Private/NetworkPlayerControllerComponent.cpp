// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerControllerComponent.h"

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

	// ...
	
}


// Called every frame
void UNetworkPlayerControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

