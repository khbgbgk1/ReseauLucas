// Fill out your copyright notice in the Description page of Project Settings.


#include "Skin_GameState_Component.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USkin_GameState_Component::USkin_GameState_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USkin_GameState_Component::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USkin_GameState_Component::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkin_GameState_Component::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkin_GameState_Component, SelectedSkin);
}
