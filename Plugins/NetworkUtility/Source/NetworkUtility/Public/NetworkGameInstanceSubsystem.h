// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetworkGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKUTILITY_API UNetworkGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	public:
	
	float ServerClientDelta = 0.0f;
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", AdvancedDisplay = "2"), Category="Game")	
	void Host(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, bool bAbsolute);
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"), Category="Game") 
	void Join(const UObject* WorldContextObject, const FString& IPAddress);
	
	UFUNCTION(BlueprintCallable, Category="Game")
	void ServerTravel(const TSoftObjectPtr<UWorld> Level);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game")
	int SelectedSkin = 0;
	
	UFUNCTION(BlueprintCallable)
	void SetSelectedSkin(int NewSelectedSkin);
	
	//Time gestion
	TArray<float> CircularBufferOutliner;
	int32 CircularBufferOutlinerIndex = 0;
	const int32 NbElementCircularBufferOutliner = 10;
	
	UFUNCTION(BlueprintCallable)
	void ConsoleLogCircularBufferOutliner(TArray<float> CircularBufferOutlinerToLog);
	
	UFUNCTION(BlueprintCallable)
	void SyncServerTime(float ClientTimestamp, float ServerTimestamp);
	
	UFUNCTION(BlueprintCallable)
	float GetSyncedServerTime() const;
	
	UFUNCTION(BlueprintCallable)
	float GetCurrentPing() const;
	
	UFUNCTION(BlueprintCallable)
	float GetServerTripTime() const;
	
	//Rewind gestion
	UFUNCTION(BlueprintCallable)
	bool CheckActorsCollision(AActor* DamageInstigator, AActor* ActorToGetRewindShapeCollision, float HitTime, FVector HitLocation, FVector StartLocation);
	
private :
	UPROPERTY()
	float CurrentPing = 0.0f;
};


