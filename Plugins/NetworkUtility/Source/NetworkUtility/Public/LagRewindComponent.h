// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagRewindComponent.generated.h"


USTRUCT(BlueprintType)
struct FSavedMove{
	GENERATED_BODY()
	// Capsule Position
	UPROPERTY()
	FTransform Position;
	// Server World Time Pos
	UPROPERTY()
	float Time;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKUTILITY_API ULagRewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULagRewindComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UShapeComponent> RewindShapeClassForDrawDebug;
	
	UPROPERTY()
	TObjectPtr<UShapeComponent> ReferenceShapeForDrawDebug;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform DebugRelativeTransform;
	
	UFUNCTION(Blueprintable)
	virtual void OnRegister() override;
	
#if WITH_EDITOR
	// Permet de mettre à jour la forme de debug en temps réel quand on change les valeurs dans l'UI
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	TDoubleLinkedList<FSavedMove> SavedMoves;
	
	float MaximumRecordTime = 2.0f;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void RecordMoves();
	
	UFUNCTION(BlueprintCallable)
	void CleanUpHistory();
	
	UFUNCTION(BlueprintCallable)
	void ShowHistory();
	
};
