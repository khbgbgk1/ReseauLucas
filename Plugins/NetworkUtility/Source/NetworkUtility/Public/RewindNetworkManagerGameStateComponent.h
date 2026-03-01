// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewindNetworkManagerGameStateComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKUTILITY_API URewindNetworkManagerGameStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URewindNetworkManagerGameStateComponent();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ShowRewindHistory)
	bool ShowRewindHistory = false;
	
	UFUNCTION()
	void OnRep_ShowRewindHistory();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetShowRewindHistory(bool bNewValue);
	
	//RewindTime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateMaximunRecordTime)
	float MaximumRecordTime = 2.0f;
	
	UFUNCTION()
	void OnRep_UpdateMaximunRecordTime();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_UpdateMaximunRecordTime(float NewValue);

	// Configuration de la réplication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	
	UFUNCTION(BlueprintCallable)
	ULagRewindComponent* GetLagRewindComponentOnPlayer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
