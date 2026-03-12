// Fill out your copyright notice in the Description page of Project Settings.


#include "TP1PlayerState.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogTP1PlayerState, Log, All);

ATP1PlayerState::ATP1PlayerState()
{
	// Instanciation du composant du plugin
	// Le nom "SkinComponent" sera celui visible dans l'éditeur Unreal
	Skin_GS_Component = CreateDefaultSubobject<USkin_GameState_Component>(TEXT("Skin_GS_Component"));

	// Optionnel : s'assurer que le composant est bien configuré pour répliquer
	// (Même si c'est déjà fait dans le constructeur du composant lui-même)
	if (Skin_GS_Component)
	{
		Skin_GS_Component->SetIsReplicated(true);
	}
}

void ATP1PlayerState::PostNetInit()
{
	Super::PostNetInit();
    
	// À ce stade, les variables initiales répliquées devraient être arrivées
	UE_LOG(LogTP1PlayerState, Log, TEXT("PostNetInit: SelectedSkin reçu : %d"), Skin_GS_Component->SelectedSkin);
}

void ATP1PlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// On cast comme ATP1PlayerState
	ATP1PlayerState* NewPlayerState = Cast<ATP1PlayerState>(PlayerState);
	if (NewPlayerState)
	{
		UE_LOG(LogTP1PlayerState, Log, TEXT("CopyProperties: SelectedSkin du PlayerState actuel : %d"), this->Skin_GS_Component->SelectedSkin);
		NewPlayerState->Skin_GS_Component->SelectedSkin = this->Skin_GS_Component->SelectedSkin;
		UE_LOG(LogTP1PlayerState, Log, TEXT("CopyProperties: SelectedSkin du NewPlayerState  : %d"), NewPlayerState->Skin_GS_Component->SelectedSkin);
		
	}
}