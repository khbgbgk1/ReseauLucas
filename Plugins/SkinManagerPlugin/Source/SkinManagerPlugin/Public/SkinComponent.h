// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkinComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKINMANAGERPLUGIN_API USkinComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkinComponent();
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_UpdateSkin)
	int SkinValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin")
	UMaterialInterface* SkinMaterial1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin")
	UMaterialInterface* SkinMaterial0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	UFUNCTION( BlueprintCallable )
	void SetSkin(int NewSkin);
	
	UFUNCTION( BlueprintCallable )
	void OnRep_UpdateSkin();
	
	UFUNCTION( Server, Reliable )
	void Server_UpdateSkin(int NewSkin);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	 void DoSkinChange();
	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
