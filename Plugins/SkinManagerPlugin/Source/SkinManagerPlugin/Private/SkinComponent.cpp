// Fill out your copyright notice in the Description page of Project Settings.


#include "SkinComponent.h"
#include "Skin_GameState_Component.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogSkinComponemt, Log, All);


// Sets default values for this component's properties
USkinComponent::USkinComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	//bReplicates = true;
	SetIsReplicatedByDefault(true);
	
	SkinValue = 0;

	// ...
}


// Called when the game starts
void USkinComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USkinComponent::DoSkinChange()
{
	UE_LOG(LogSkinComponemt, Log, TEXT("DoSkinChange: DoSkinChange trigger"));
	if (SkinValue == 0)
	{
		SetSkin(1);
	} else if (SkinValue == 1)
	{
		SetSkin(0);
	}
}

void USkinComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkinComponent, SkinValue);
}


void USkinComponent::SetSkin(int NewSkin)
{
	
	if (GetOwner()->HasAuthority())
	{
		SkinValue = NewSkin;

		ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
		if (OwnerChar)
		{
			APlayerState* PS = OwnerChar->GetPlayerState();
			if (PS)
			{
				UActorComponent* FoundComp = PS->FindComponentByClass(USkin_GameState_Component::StaticClass());
				if (USkin_GameState_Component* SkinDataComp = Cast<USkin_GameState_Component>(FoundComp))
				{
					SkinDataComp->SelectedSkin = NewSkin;
					UE_LOG(LogSkinComponemt, Log, TEXT("SetSkin: SkinValue mis à jour sur le PlayerState Component : %d"), NewSkin);
				}
				else
				{
					UE_LOG(LogSkinComponemt, Warning, TEXT("SetSkin: Le PlayerState n'a pas de SkinComponent !"));
				}
			}
		}
        
		// On déclenche le OnRep manuellement sur le serveur pour mettre à jour son propre visuel
		OnRep_UpdateSkin();
	} 
	else
	{
		// Si on est sur un client, on demande au serveur de le faire
		Server_UpdateSkin(NewSkin);
	}
}

void USkinComponent::OnRep_UpdateSkin()
{
	UE_LOG(LogSkinComponemt, Log, TEXT("Skin appliqué : %d"), SkinValue);
	ACharacter* MyCharacter = Cast<ACharacter>(GetOwner());
	if (MyCharacter && MyCharacter->GetMesh())
	{
		if (SkinValue == 1)
		{
			MyCharacter->GetMesh()->SetMaterial(0, SkinMaterial1);
		} else if (SkinValue == 0)
		{
			MyCharacter->GetMesh()->SetMaterial(0, SkinMaterial0);
		}
		
		
		UE_LOG(LogSkinComponemt, Log, TEXT("Couleur mise à jour sur le Mesh !"));
	}
}

void USkinComponent::Server_UpdateSkin_Implementation(int NewSkin)
{
	SetSkin(NewSkin);
}

// Called every frame
void USkinComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

