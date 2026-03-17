// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogDamageableComponent, Log, All);

// Sets default values for this component's properties
UDamageableComponent::UDamageableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void UDamageableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDamageableComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDamageableComponent::ApplyDomage(int Damages, AActor* Instigator, float HitTime , FVector HitLocation)
{
	OnDamageEvent.Broadcast(Damages,Instigator,HitTime,HitLocation);
        
	UE_LOG(LogDamageableComponent, Log, TEXT("ApplyDomage: Broadcast réussi. Dégâts: %d par %s"), Damages, *Instigator->GetName());
}

