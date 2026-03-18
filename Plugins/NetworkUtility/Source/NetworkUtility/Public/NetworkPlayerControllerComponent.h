// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkGameInstanceSubsystem.h"
#include "Components/ActorComponent.h"
#include "NetworkPlayerControllerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKUTILITY_API UNetworkPlayerControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNetworkPlayerControllerComponent();
	
	UFUNCTION(Client, Reliable)
	void Client_ReceiveRewindHit(AActor* VictimActor, FTransform VictimTransform);
	
	UFUNCTION(BlueprintCallable)
	UNetworkGameInstanceSubsystem* GetNetworkSubsystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(visibleAnywhere,BlueprintReadOnly)
	TObjectPtr<APlayerController> OwningPlayerController;
	
	UFUNCTION(Server, Unreliable)
	void Server_RequestServerTime(float ClientTimestamp);
	
	UFUNCTION(Client, Unreliable)
	void Client_ReportServerTime(float ClientTimestamp, float ServerTimestamp);
	
	FTimerHandle TimerHandle_Sync;
	
	UFUNCTION()
	void SyncTime();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
