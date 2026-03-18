// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGeneral.h"

#include "DamageableComponent.h"
#include "NetworkGameInstanceSubsystem.h"
#include "Net/UnrealNetwork.h"

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
	
	SphereColision->SetCollisionObjectType(ECC_WorldDynamic);
	// On ignore toutes els collisions
	SphereColision->SetCollisionResponseToAllChannels(ECR_Ignore);
	//On autorise  l'Overlap avec les Pawns
	SphereColision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// On bloque sur le décor (Static)
	SphereColision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	//On active la génération d'événements d'overlap
	SphereColision->SetGenerateOverlapEvents(true);
	
	SphereColision->OnComponentBeginOverlap.AddDynamic(this, &AProjectileGeneral::OnProjectileOverlap);

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

void AProjectileGeneral::CheckAndHideIfOwner()
{
	if (GetNetMode() == NM_DedicatedServer) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn() && FiringPawn == PC->GetPawn())
	{
		UE_LOG(LogProjectileGeneral, Display, TEXT("C'est mon tir ! Cache-toi."));
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void AProjectileGeneral::OnRep_FiringPawn()
{
	UE_LOG(LogProjectileGeneral, Log, TEXT("OnRep_FiringPawn reçu !"));
	CheckAndHideIfOwner();
}

void AProjectileGeneral::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// On veut que le code s'active s'il touche bien un actor
	if (!OtherActor || OtherActor == FiringPawn) return;
	
	// On veut que seul le client qui a tiré envoie l'information de hit
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn() == FiringPawn)
	{
		// 3. On cherche le composant Damageable sur la victime
		UDamageableComponent* DamageComp = OtherActor->FindComponentByClass<UDamageableComponent>();
        
		if (DamageComp)
		{
			UE_LOG(LogProjectileGeneral, Log, TEXT("HIT DETECTED : Envoi de la demande de dégâts par le client tireur."));
			
			float SyncTime = 0.f;
			if (UNetworkGameInstanceSubsystem* TimeSub = GetGameInstance()->GetSubsystem<UNetworkGameInstanceSubsystem>())
			{
				SyncTime = TimeSub->GetSyncedServerTime();
				float STT = TimeSub->GetServerTripTime();
				SyncTime -= STT;
			}
            
			// On exécute la fonction du composant
			DamageComp->ApplyDomage(Degats, FiringPawn, SyncTime, GetActorLocation());

			//On détruit le projectile après l'impact
			Destroy();
		} else
		{
			UE_LOG(LogProjectileGeneral, Warning, TEXT("[%s] COLLISION avec [%s] : L'acteur n'a pas de DamageableComponent."), 
				   *GetName(), *OtherActor->GetName());
		}
	}
}

void AProjectileGeneral::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileGeneral, FiringPawn);
}
