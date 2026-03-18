// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundBase.h"
#include "ProjectileGeneral.generated.h"

UCLASS()
class TP1RESEAU_API AProjectileGeneral : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileGeneral();
	
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereColision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(ReplicatedUsing = OnRep_FiringPawn)
	APawn* FiringPawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,	 Category = "Gameplay")
	int Degats = 5;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Gameplay")
	FVector StartPosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* LaunchSound;
	
	UFUNCTION()
	void OnRep_FiringPawn();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void CheckAndHideIfOwner();
	
	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
