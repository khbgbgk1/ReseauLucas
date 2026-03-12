// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Skin_GameState_Component.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKINMANAGERPLUGIN_API USkin_GameState_Component : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkin_GameState_Component();
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skin")
	int32 SelectedSkin = 0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
