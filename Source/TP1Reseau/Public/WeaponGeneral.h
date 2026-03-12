// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "ProjectileGeneral.h"
#include "WeaponGeneral.generated.h"

UCLASS()
class TP1RESEAU_API AWeaponGeneral : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponGeneral();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> DefaultRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AProjectileGeneral> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> ProjectileArrow;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	FTimerHandle TimerHandle_Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate = 0.2f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StarFiringProjectile();
	UFUNCTION(BlueprintCallable)
	void StopFiringProjectile();
	UFUNCTION(BlueprintCallable)
	void FireOneProjectile();
};
