// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponGeneral.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponGeneral, Log, All);


// Sets default values
AWeaponGeneral::AWeaponGeneral()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
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
	if (!GetWorld()) return;

	if (HasAuthority())
	{
		// On est le serveur : on gère le timer directement
		FireOneProjectile();
		UE_LOG(LogWeaponGeneral, Log, TEXT("StarFiringProjectile called - HasAuthority: %s"), HasAuthority() ? TEXT("true") : TEXT("false"));
		GetWorldTimerManager().SetTimer(
			TimerHandle_Fire,
			this,
			&AWeaponGeneral::FireOneProjectile,
			FireRate,
			true
		);
	}
	else
	{
		// On est un client : on demande au serveur de tirer
		UE_LOG(LogWeaponGeneral, Log, TEXT("StarFiringProjectile called to server"));
		FireOneProjectile();
		UE_LOG(LogWeaponGeneral, Log, TEXT("StarFiringProjectile called - HasAuthority: %s"), HasAuthority() ? TEXT("true") : TEXT("false"));
		GetWorldTimerManager().SetTimer(
			TimerHandle_Fire,
			this,
			&AWeaponGeneral::FireOneProjectile,
			FireRate,
			true
		);
		Server_StartFiring();
	}}



void AWeaponGeneral::StopFiringProjectile()
{
	if (!GetWorld()) return;

	if (HasAuthority())
	{
		UE_LOG(LogWeaponGeneral, Log, TEXT("StopFiringProjectile called"));
		GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
	}
	else
	{
		UE_LOG(LogWeaponGeneral, Log, TEXT("StopFiringProjectile called to server"));
		GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
		Server_StopFiring();
	}
}

void AWeaponGeneral::FireOneProjectile()
{
	// if (!HasAuthority())
	// {
	// 	return;
	// }
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
			APawn* MyPawn = Cast<APawn>(GetOwner());
			SpawnedProjectile->FiringPawn = MyPawn;
		}
	}
}

void AWeaponGeneral::Server_StartFiring_Implementation()
{
	StarFiringProjectile();
}

void AWeaponGeneral::Server_StopFiring_Implementation()
{
	StopFiringProjectile();
}
