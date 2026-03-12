// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponGeneral.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponGeneral, Log, All);


// Sets default values
AWeaponGeneral::AWeaponGeneral()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;
	
	// MuzzleLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleLocation"));
	// MuzzleLocation->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWeaponGeneral::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponGeneral::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponGeneral::StarFiringProjectile()
{
	if (GetWorld())
	{
		FireOneProjectile();

		// On lance le timer pour les tirs suivants
		GetWorldTimerManager().SetTimer(
			TimerHandle_Fire, 
			this, 
			&AWeaponGeneral::FireOneProjectile, 
			FireRate, 
			true
		);
        
		UE_LOG(LogWeaponGeneral, Log, TEXT("StarFiringProjectile: Started Firing in weapon"));
	}
}

void AWeaponGeneral::StopFiringProjectile()
{
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
		UE_LOG(LogWeaponGeneral, Log, TEXT("StopFiringProjectile: Stopped Firing in weapon"));
	}
}

void AWeaponGeneral::FireOneProjectile()
{
	if (GetWorld() && ProjectileClass)
	{
		
		if (!ProjectileArrow)
		{
			UE_LOG(LogWeaponGeneral, Warning, TEXT("FireOneProjectile: ProjectileArrow not defined in BluePrint"));
			return;
		}
		
		FVector SpawnLocation = ProjectileArrow->GetComponentLocation();
		FRotator SpawnRotation = ProjectileArrow->GetComponentRotation();

		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;            
		SpawnParams.Instigator = GetInstigator(); 
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		
		AProjectileGeneral* SpawnedProjectile = GetWorld()->SpawnActor<AProjectileGeneral>(
			ProjectileClass, 
			SpawnLocation, 
			SpawnRotation, 
			SpawnParams
		);

		if (SpawnedProjectile)
		{
			// Log de succès
			UE_LOG(LogWeaponGeneral, Log, TEXT("Projectile Spawned!"));
		}
	}
}

