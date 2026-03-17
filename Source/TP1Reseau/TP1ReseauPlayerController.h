// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NetworkGameInstanceSubsystem.h"
#include "NetworkPlayerControllerComponent.h"
#include "TP1ReseauCharacter.h"
#include "GameFramework/PlayerController.h"
#include "TP1ReseauPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class ATP1ReseauPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	ATP1ReseauPlayerController();
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	UPROPERTY()
	UUserWidget* LoadingWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UNetworkPlayerControllerComponent* NetworkComponent;
	
	UFUNCTION(Client, Reliable)
	void Client_ShowLoadingScreen();

	UFUNCTION(Client, Reliable)
	void Client_HideLoadingScreen();
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyDamage(int Damages, AActor* DamageInstigator,ATP1ReseauCharacter* CharacterToKill,float HitTime , FVector HitLocation);
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
	
	UFUNCTION()
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(Server, Unreliable)
	void Server_RequestServerTime(float ClientTimestamp);

	UFUNCTION(Client, Unreliable)
	void Client_ReportServerTime(float ClientTimestamp, float ServerTimestamp);
	
	FTimerHandle TimerHandle_Sync;
	
	UFUNCTION()
	void SyncTime();
	
	UFUNCTION(BlueprintCallable)
	UNetworkGameInstanceSubsystem* GetNetworkSubsystem();

};
