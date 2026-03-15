// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "EventHandle.h"
#include "EventWorldSubsystem.generated.h"

/**
 * 
 */
 
UCLASS()
class EVENTSMANAGER_API UEventWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	//Event Gestion
    UFUNCTION(BlueprintCallable, Category = "Event Manager")
    	void TriggerEvent(FGameplayTag Tag, UObject *Interactor);

	UFUNCTION(BlueprintCallable, Category = "Event Manager")
	UEventHandle* GetEvent(const FGameplayTag& Tag);
    
    private:
    	UPROPERTY()
		TMap<FGameplayTag, TObjectPtr<UEventHandle>> TagDelegateMap;
	
};