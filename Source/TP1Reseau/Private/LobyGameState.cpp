// Fill out your copyright notice in the Description page of Project Settings.


#include "LobyGameState.h"
#include "Net/UnrealNetwork.h"

void ALobyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// On réplique RemainingTime à tout le monde
	DOREPLIFETIME(ALobyGameState, RemainingTime);
	DOREPLIFETIME(ALobyGameState, LevelStartTime);
}

// void ALobyGameState::SyncServerTime(float ClientTimestamp, float ServerTimestamp)
// {
// 	float CurrentTime = GetWorld()->GetTimeSeconds();
// 	float RTT = CurrentTime - ClientTimestamp;
// 	float AdjustedServerTime = ServerTimestamp + (RTT * 0.5f);
//     
// 	ServerClientDelta = AdjustedServerTime - CurrentTime;
// }

// float ALobyGameState::GetSyncedServerTime() const
// {
// 	return GetWorld()->GetTimeSeconds() + ServerClientDelta;
// }