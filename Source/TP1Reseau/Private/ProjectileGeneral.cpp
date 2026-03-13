// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGeneral.h"

DEFINE_LOG_CATEGORY_STATIC(LogProjectileGeneral, Log, All);


AProjectileGeneral::AProjectileGeneral()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SphereColision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	RootComponent = SphereColision;

	// Create StaticMeshComponent and Attach to BoxComponent
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SphereColision);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	InitialLifeSpan = 5.0f;
	
	bReplicates = true;

}

// Called when the game starts or when spawned
void AProjectileGeneral::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileGeneral::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

