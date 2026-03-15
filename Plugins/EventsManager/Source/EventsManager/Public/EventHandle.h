#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EventHandle.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGenericEvent,UObject*, Interactor);

UCLASS(BlueprintType)
class EVENTSMANAGER_API UEventHandle : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FGenericEvent GenericEvent;
};