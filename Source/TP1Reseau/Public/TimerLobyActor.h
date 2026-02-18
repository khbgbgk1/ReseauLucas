// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerLobyActor.generated.h"

UCLASS()
class TP1RESEAU_API ATimerLobyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimerLobyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void UpdateTimerLogic();
	
	UFUNCTION(BlueprintCallable)
	void StartTimer();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ServerTime ;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CurrentSyncedTime ;
	
	FTimerHandle TimerHandle_UpdateDisplay;
    
	const float UpdateInterval = 0.05f;
};
