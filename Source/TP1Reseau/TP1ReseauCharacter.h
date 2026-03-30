// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SkinComponent.h"
#include "WeaponGeneral.h"
#include "LagRewindComponent.h"
#include "DamageableComponent.h"
#include "TP1ReseauCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ATP1ReseauCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ChangeSkinAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ShowMouseAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* PauseMenuAction;
	
	//Achievement
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* WinAchievementAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LoseAchievementAction;

	

public:

	/** Constructor */
	ATP1ReseauCharacter();	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDamageableComponent* DamageableComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ULagRewindComponent* RewindComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Health, Category = "Gameplay")
	int CurrentHealth = 5;

	/** Santé Max */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int MaxHealth = 5;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ApplyDamageOnPlayer(int Damages, AActor* DamageInstigator,float HitTime , FVector HitLocation, FVector StartLocation);

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void ShowMouse();
	
	void HideMouse();
	
	UPROPERTY()
	bool bValueShowNouse = false;
	
	virtual void BeginPlay() override;
	
	
	//LifeCycle
	UFUNCTION(BlueprintCallable)
	void Die();
	
	UFUNCTION()
	void OnRep_Health();
	
	UFUNCTION(Server, Reliable)
	void Server_RequestRespawn();

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkinComponent* SkinComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentWeapon, Category="Weapon")
	AWeaponGeneral* CurrentWeapon;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoFire();
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void StopFiring();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void OpenClosePauseMenu();
	
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintCallable)
	void OnRep_CurrentWeapon();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void RequestWinSuccess();
	
	UFUNCTION(BlueprintCallable)
	void RequestLoseSuccess();
	
	UFUNCTION(BlueprintCallable)
	void RequestUpdateSuccess(float Progress);
	
	UFUNCTION(Server, Reliable)
	void Server_UpdateAchievement(float Progress);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FName AchievementName = "ACH_WIN_ONE_GAME";

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

