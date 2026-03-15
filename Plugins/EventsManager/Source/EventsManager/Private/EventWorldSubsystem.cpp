// Fill out your copyright notice in the Description page of Project Settings.


#include "EventWorldSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogTempleWorldSubSystem, Log, All);

void UEventWorldSubsystem::TriggerEvent(FGameplayTag Tag, UObject *Interactor)
{
	if (TObjectPtr<UEventHandle>* HandlePtr = TagDelegateMap.Find(Tag))
	{
		FString InteractorName = Interactor ? Interactor->GetName() : TEXT("None");
		UE_LOG(LogTempleWorldSubSystem, Log, 
			TEXT("TriggerEvent: Événement trouvé et déclenché pour Tag: %s, Interactor: %s"), 
			*Tag.ToString(), 
			*InteractorName
	   );
		(*HandlePtr)->GenericEvent.Broadcast(Interactor);
	}
	else 
	{
		UE_LOG(LogTempleWorldSubSystem, Warning, TEXT("TriggerEvent: Aucun gestionnaire d'événement (Handle) trouvé pour le Tag: %s. Vérifiez les abonnements."), *Tag.ToString());
	}
}

UEventHandle* UEventWorldSubsystem::GetEvent(const FGameplayTag& Tag)
{
	if (!TagDelegateMap.Contains(Tag))
	{
		UEventHandle* NewHandle = NewObject<UEventHandle>(this);
		TagDelegateMap.Add(Tag, NewHandle);
	}
	return TagDelegateMap[Tag];
}