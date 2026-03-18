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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool ShowRewindHistory = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float ShapeRewindLifeDuration = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FColor HistoryShapeColor = FColor::Green;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool ShowRewindHit = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float ShapeRewindLifeDurationForHit = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FColor HitShapeColor = FColor::Yellow;
	
	
	UFUNCTION(Blueprintable)
	virtual void OnRegister() override;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetShowRewindHistory(bool bNewValue);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_UpdateMaximunRecordTime(float NewValue);
	
#if WITH_EDITOR
	// Permet de mettre à jour la forme de debug en temps réel quand on change les valeurs dans l'UI
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void DrawDebugMove(const FSavedMove& Move, FColor Color, float Duration);
	
	TDoubleLinkedList<FSavedMove>::TDoubleLinkedListNode* GetNodeBeforeTime(float Time);

public:
	// Called every frame
	TDoubleLinkedList<FSavedMove> SavedMoves;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float MaximumRecordTime = 2.0f;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void RecordMoves();
	
	UFUNCTION(BlueprintCallable)
	void CleanUpHistory();
	
	UFUNCTION(BlueprintCallable)
	void ShowHistory();
	
	// UFUNCTION(BlueprintCallable)
	// FSavedMove GetClosestMoveToTime(float Time);
	
	UFUNCTION(BlueprintCallable)
	FSavedMove GetInterpolatedMoveToTime(float Time);
	
	UFUNCTION(BlueprintCallable)
	void DrawValidationHit(FTransform ActorTransformAtTime);
	
};
