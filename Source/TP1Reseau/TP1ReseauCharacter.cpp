// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP1ReseauCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TP1PlayerState.h"
#include "TP1Reseau.h"

DEFINE_LOG_CATEGORY_STATIC(LogTP1ReseauCharacter, Log, All);


ATP1ReseauCharacter::ATP1ReseauCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	SkinComponent = CreateDefaultSubobject<USkinComponent>(TEXT("SkinComponent"));
    
	// Si tu as utilisé COND_OwnerOnly précédemment, assure-toi que le composant réplique
	SkinComponent->SetIsReplicated(true);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATP1ReseauCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP1ReseauCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATP1ReseauCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATP1ReseauCharacter::Look);
		
		// Skin
		if (SkinComponent)
		{
			EnhancedInputComponent->BindAction(ChangeSkinAction, ETriggerEvent::Triggered, SkinComponent, &USkinComponent::DoSkinChange);
		}
		
	}
	else
	{
		UE_LOG(LogTP1Reseau, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATP1ReseauCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATP1ReseauCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ATP1ReseauCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ATP1ReseauCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATP1ReseauCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ATP1ReseauCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ATP1ReseauCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATP1ReseauCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// C'EST ICI que le client va vraiment récupérer son skin lors d'un voyage
	if (ATP1PlayerState* PS = GetPlayerState<ATP1PlayerState>())
	{
		if (SkinComponent)
		{
			UE_LOG(LogTP1ReseauCharacter, Log, TEXT("[Client] OnRep_PlayerState : Synchronisation du skin %d"), PS->SelectedSkin);
			SkinComponent->SetSkin(PS->SelectedSkin);
		}
		
		else
		{
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("OnRep_PlayerState : No SkinComponent"));
		}
	}
	else
	{
		UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("OnRep_PlayerState : PlayerState not a ATP1PlayerState"));
	}
}
