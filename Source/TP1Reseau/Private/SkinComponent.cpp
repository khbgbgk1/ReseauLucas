// Fill out your copyright notice in the Description page of Project Settings.


#include "SkinComponent.h"
#include "TP1PlayerState.h"
#include "GameFramework/Character.h"

class ATP1PlayerState;
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
			if (ATP1PlayerState* PS =OwnerChar->GetPlayerState<ATP1PlayerState>())
			{
				PS->SelectedSkin = NewSkin;
				UE_LOG(LogSkinComponemt, Log, TEXT("Server_UpdateSkin_Implementation: NewSkin %d updates"),NewSkin);
				
			}
		}
		OnRep_UpdateSkin();
	} else
	{
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

