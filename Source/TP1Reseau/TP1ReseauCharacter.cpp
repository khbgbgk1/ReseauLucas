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
#include "NetworkTPHUD.h"
#include "TP1PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "TP1Reseau.h"
#include "TP1ReseauPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/GameMapGameMode.h"
#include "Net/UnrealNetwork.h"

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
	
	//Composant pour pouvoir recevoir des dégats
	DamageableComponent = CreateDefaultSubobject<UDamageableComponent>(TEXT("DamageableComponent"));
	
	//Composant pour pouvoir rollback sur le serveur
	RewindComponent = CreateDefaultSubobject<ULagRewindComponent>(TEXT("RewindComponent"));
	
	
	CurrentHealth = MaxHealth;
    
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
		if (!SkinComponent)
		{
			SkinComponent = FindComponentByClass<USkinComponent>();
			if (SkinComponent)
			{
				UE_LOG(LogTP1ReseauCharacter, Log, TEXT("SetupPlayerInputComponent: RÉCUPÉRATION RÉUSSIE : SkinComponent trouvé via FindComponentByClass. Nom : %s"), *SkinComponent->GetName());
			}
			else
			{
				UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("ÉCHEC TOTAL : Aucun USkinComponent n'est attaché à cet acteur !"));
			}
		}
		if (SkinComponent)
		{
			EnhancedInputComponent->BindAction(ChangeSkinAction, ETriggerEvent::Triggered, SkinComponent, &USkinComponent::DoSkinChange);
		} else {
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("SetupPlayerInputComponent: SkinComponent manquant."));
            			
		}
		
		//ShowMouse
		if (ShowMouseAction)
		{
			EnhancedInputComponent->BindAction(ShowMouseAction, ETriggerEvent::Started, this, &ATP1ReseauCharacter::ShowMouse);
			//EnhancedInputComponent->BindAction(ShowMouseAction, ETriggerEvent::Completed, this, &ATP1ReseauCharacter::HideMouse);
		}else {
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("SetupPlayerInputComponent: ShowMouseAction manquant."));
            			
		}
		
		// Fire with weapon
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATP1ReseauCharacter::DoFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATP1ReseauCharacter::StopFiring);
		}else {
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("SetupPlayerInputComponent: ShowMouseAction manquant."));
            			
		}
		
		if (PauseMenuAction)
		{
			EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Started, this, &ATP1ReseauCharacter::OpenClosePauseMenu);
		}else {
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("SetupPlayerInputComponent: PauseMenuAction manquant."));
            			
		}
		
		if (WinAchievementAction)
		{
			EnhancedInputComponent->BindAction(WinAchievementAction, ETriggerEvent::Started, this, &ATP1ReseauCharacter::RequestWinSuccess);
		}else {
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("SetupPlayerInputComponent: WinAchievementAction manquant."));
            			
		}
		
		if (LoseAchievementAction)
		{
			EnhancedInputComponent->BindAction(LoseAchievementAction, ETriggerEvent::Started, this, &ATP1ReseauCharacter::RequestLoseSuccess);
		}else {
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("SetupPlayerInputComponent: LoseAchievementAction manquant."));
            			
		}
	}
	else
	{
		UE_LOG(LogTP1ReseauCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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

void ATP1ReseauCharacter::ShowMouse()
{
	bValueShowNouse = !bValueShowNouse;
	if (bValueShowNouse)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);
        
			PC->SetInputMode(InputMode);
		}
	} else
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;

			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
	}
	
}

void ATP1ReseauCharacter::HideMouse()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
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

void ATP1ReseauCharacter::DoFire()
{
	UE_LOG(LogTP1ReseauCharacter, Log, TEXT("DoFire: Start Fire !"));
	if (CurrentWeapon)
	{
		CurrentWeapon->StarFiringProjectile();
	} else
	{
		UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("DoFire:CurrentWeapon nullptr"));
	}
}

void ATP1ReseauCharacter::StopFiring()
{
	UE_LOG(LogTP1ReseauCharacter, Log, TEXT("StopFiring: EndFiring !"));
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiringProjectile();
	}else
	{
		UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("StopFiring:CurrentWeapon nullptr"));
	}
}

void ATP1ReseauCharacter::OpenClosePauseMenu()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	// 2. Récupérer le HUD et le caster vers ta classe spécifique
	ANetworkTPHUD* MyHUD = Cast<ANetworkTPHUD>(PC->GetHUD());
    
	if (MyHUD)
	{
		bool bIsMenuVisible = MyHUD->PauseMenuWidget && MyHUD->PauseMenuWidget->IsVisible();

		if (bIsMenuVisible)
		{
			// Fermer le menu pause et réafficher le HUD de jeu
			MyHUD->PrintPauseMenu(false);
			MyHUD->PrintPlayerHUD(true);
			PC->bShowMouseCursor = false;
		}
		else
		{
			// Ouvrir le menu pause et masquer le HUD de jeu
			MyHUD->PrintPlayerHUD(false);
			MyHUD->PrintPauseMenu(true);
			PC->bShowMouseCursor = true;
		}
	}
}

void ATP1ReseauCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	
	if (SkinComponent == nullptr)
	{
		UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("BeginPlay: SkinComponent est NULL dès le BeginPlay !"));
		// Tentative de récupération si le constructeur a échoué à lier le composant
		SkinComponent = FindComponentByClass<USkinComponent>();
		if (SkinComponent)
		{
			UE_LOG(LogTP1ReseauCharacter, Log, TEXT("BeginPlay: RÉCUPÉRATION RÉUSSIE : SkinComponent trouvé via FindComponentByClass. Nom : %s"), *SkinComponent->GetName());
		}
		else
		{
			UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("BeginPlay: ÉCHEC TOTAL : Aucun USkinComponent n'est attaché à cet acteur !"));
		}
	}
	else
	{
		UE_LOG(LogTP1ReseauCharacter, Log, TEXT("BeginPlay : SkinComponent valide : %s"), *SkinComponent->GetName());
	}
	
	if (DamageableComponent)
	{
		
		DamageableComponent->OnDamageEvent.AddDynamic(this, &ATP1ReseauCharacter::ApplyDamageOnPlayer);
        
		UE_LOG(LogTP1ReseauCharacter, Log, TEXT("BeginPlay: Liaison DamageableComponent réussie !"));
	}
	else
	{
		UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("BeginPlay: DamageableComponent est manquant !"));
	}
}

void ATP1ReseauCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// C'EST ICI que le client va vraiment récupérer son skin lors d'un voyage
	if (ATP1PlayerState* PS = GetPlayerState<ATP1PlayerState>())
	{
		if (SkinComponent)
		{
			USkin_GameState_Component* PS_SkinComp = PS->FindComponentByClass<USkin_GameState_Component>();
       
			if (PS_SkinComp && SkinComponent)
			{
				UE_LOG(LogTP1ReseauCharacter, Log, TEXT("[Client] OnRep_PlayerState : Synchronisation via Component, Skin %d"), PS_SkinComp->SelectedSkin);
          
				SkinComponent->SetSkin(PS_SkinComp->SelectedSkin);
			}
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
void ATP1ReseauCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATP1ReseauCharacter, CurrentWeapon);
	DOREPLIFETIME(ATP1ReseauCharacter, CurrentHealth);
}

void ATP1ReseauCharacter::RequestWinSuccess()
{
	RequestUpdateSuccess(100.0f);
}

void ATP1ReseauCharacter::RequestLoseSuccess()
{
	RequestUpdateSuccess(0.0f);
}

void ATP1ReseauCharacter::RequestUpdateSuccess(float Progress)
{
	if(HasAuthority())
	{
		// Le serveur demande au Subsystem de mettre à jour pour ce joueur
		if (UNetworkGameInstanceSubsystem* NetSub = GetGameInstance()->GetSubsystem<UNetworkGameInstanceSubsystem>())
		{
			NetSub->UpdateSteamAchievement(AchievementName, Progress);
		}
	} else
	{
		Server_UpdateAchievement(Progress);
	}
}

void ATP1ReseauCharacter::Server_UpdateAchievement_Implementation(float Progress)
{
	RequestUpdateSuccess(Progress);
}

void ATP1ReseauCharacter::OnRep_CurrentWeapon()
{
	// CurrentWeapon est maintenant valide sur le client
	UE_LOG(LogTP1ReseauCharacter, Log, TEXT("OnRep_CurrentWeapon: arme reçue par réplication"));
}

void ATP1ReseauCharacter::OnRep_Health()
{
	// Lien a faire avec UI pottentiel
	UE_LOG(LogTP1ReseauCharacter, Log, TEXT("Client: Santé mise à jour : %d"), CurrentHealth);
}

void ATP1ReseauCharacter::Server_RequestRespawn_Implementation()
{
	Die();
}

void ATP1ReseauCharacter::Die()
{
	if (!HasAuthority())
	{
		Server_RequestRespawn();
		return;
	}
	else
	{
		UE_LOG(LogTP1ReseauCharacter, Log, TEXT("CHARACTER DEAD: %s"), *GetName());
		
		if (AGameMapGameMode* GM = GetWorld()->GetAuthGameMode<AGameMapGameMode>())
		{
			UE_LOG(LogTP1ReseauCharacter, Log, TEXT("Die : On appelle RequestPlayerRespawn sur le GameMode de type AGameMapGameMode"));
			GM->RequestPlayerRespawn(GetController());
		}
	}
}

void ATP1ReseauCharacter::ApplyDamageOnPlayer(int32 Damages, AActor* DamageInstigator, float HitTime , FVector HitLocation, FVector StartLocation)
{
	if (!HasAuthority())
	{
		APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
		ATP1ReseauPlayerController* OwnerSessionPC = Cast<ATP1ReseauPlayerController>(LocalPC);
		if (OwnerSessionPC)
		{
			UE_LOG(LogTP1ReseauCharacter, Log, TEXT("ApplyDamageOnPlayer : On appelle la fonction sur le PC du Client en cours pour que le serveur puisse accepter la demande"));
			OwnerSessionPC->Server_ApplyDamage(Damages, DamageInstigator, this, HitTime, HitLocation,StartLocation);
		}
	}
	else {

		if (CurrentHealth <= 0)
		{
		UE_LOG(LogTP1ReseauCharacter, Warning, TEXT("Santé déjà à 0 ou moins, de %s : %d"), *GetName(), CurrentHealth);
			return;
		}

		CurrentHealth = FMath::Max(0, CurrentHealth - Damages);
		UE_LOG(LogTP1ReseauCharacter, Log, TEXT("Santé de %s : %d"), *GetName(), CurrentHealth);

		OnRep_Health(); //Pour le serveur
		
		if (CurrentHealth <= 0)
		{
			Die();
		}
	}
}