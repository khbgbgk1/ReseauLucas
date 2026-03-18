// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnDamageable, int, Damages, AActor*, Instigator, float, HitTime , FVector, HitLocation, FVector, StartLocation);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TP1RESEAU_API UDamageableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDamageableComponent();
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnDamageable OnDamageEvent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void ApplyDomage(int Damages, AActor* Instigator, float HitTime , FVector HitLocation, FVector StartLocation);
};
