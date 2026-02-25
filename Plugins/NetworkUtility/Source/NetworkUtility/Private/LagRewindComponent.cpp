// Fill out your copyright notice in the Description page of Project Settings.


#include "LagRewindComponent.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogLagRewindComponent, Log, All);


// Sets default values for this component's properties
ULagRewindComponent::ULagRewindComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULagRewindComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULagRewindComponent::OnRegister()
{
	Super::OnRegister();

	// On crée la shape si elle n'existe pas encore et qu'on a une classe définie
	if (RewindShapeClassForDrawDebug && !ReferenceShapeForDrawDebug)
	{
		ReferenceShapeForDrawDebug = NewObject<UShapeComponent>(GetOwner(), RewindShapeClassForDrawDebug);
		if (ReferenceShapeForDrawDebug)
		{
			ReferenceShapeForDrawDebug->SetupAttachment(GetOwner()->GetRootComponent());
			ReferenceShapeForDrawDebug->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ReferenceShapeForDrawDebug->SetRelativeTransform(DebugRelativeTransform);
			ReferenceShapeForDrawDebug->RegisterComponent();
		}
	}
}

// Called every frame
void ULagRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwner()->HasAuthority())
	{
		RecordMoves();
		CleanUpHistory();
		ShowHistory();
	}

	// ...
}

void ULagRewindComponent::RecordMoves()
{
	AActor* actor = GetOwner();
	
	if (!actor)
	{
		UE_LOG(LogLagRewindComponent, Log, TEXT("Le owner n'Est pas un actor...."));
		return;
	}
	FSavedMove NewMove;
	NewMove.Time =GetWorld()->GetTimeSeconds();
	NewMove.Position = actor->GetActorTransform();
	SavedMoves.AddHead(NewMove);
	
}

void ULagRewindComponent::CleanUpHistory()
{
	if (SavedMoves.Num() <= 1)
	{
		return;
	}
	
	float HistoryLength = SavedMoves.GetHead()->GetValue().Time - SavedMoves.GetTail()->GetValue().Time;
	while (HistoryLength > MaximumRecordTime)
	{
		SavedMoves.RemoveNode(SavedMoves.GetTail());
		HistoryLength = SavedMoves.GetHead()->GetValue().Time - SavedMoves.GetTail()->GetValue().Time;;
	}
}

void ULagRewindComponent::ShowHistory()
{
	if (!ReferenceShapeForDrawDebug) return;

	for (auto& Move : SavedMoves)
	{
		FTransform FinalVisualTransform = DebugRelativeTransform * Move.Position;
        
		FVector Location = FinalVisualTransform.GetLocation();
		FQuat Rotation = FinalVisualTransform.GetRotation();
		FVector Scale = FinalVisualTransform.GetScale3D();
		
		FColor DebugColor = FColor::Green;
		
		if (UBoxComponent* Box = Cast<UBoxComponent>(ReferenceShapeForDrawDebug))
		{
			// Note : On multiplie l'extent par le scale du transform de debug
			DrawDebugBox(GetWorld(), Location, Box->GetUnscaledBoxExtent() * Scale, Rotation, DebugColor, false, 0.1f);
		}
		else if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(ReferenceShapeForDrawDebug))
		{
			// On ajuste rayon et hauteur selon le scale (Z pour hauteur, X ou Y pour rayon)
			float ScaledHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight() * Scale.Z;
			float ScaledRadius = Capsule->GetUnscaledCapsuleRadius() * FMath::Max(Scale.X, Scale.Y);
            
			DrawDebugCapsule(GetWorld(), Location, ScaledHalfHeight, ScaledRadius, Rotation, DebugColor, false, 0.1f);
		}
		else if (USphereComponent* Sphere = Cast<USphereComponent>(ReferenceShapeForDrawDebug))
		{
			float ScaledRadius = Sphere->GetUnscaledSphereRadius() * Scale.GetMax();
			DrawDebugSphere(GetWorld(), Location, ScaledRadius, 16, DebugColor, false, 0.1f);
		}
		else 
		{
			UE_LOG(LogLagRewindComponent, Warning, 
				TEXT("LogLagRewindComponent : ShowHistory: Type de forme non supporté pour le dessin. Classe : [%s]. Veuillez ajouter un bloc 'else if' pour gérer ce type dans LagRewindComponent.cpp"), 
				*ReferenceShapeForDrawDebug->GetClass()->GetName());
            
			break; 
		}
	}
}

#if WITH_EDITOR
void ULagRewindComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	// Si on change le scale/offset, on met à jour la shape de référence immédiatement
	if (ReferenceShapeForDrawDebug)
	{
		ReferenceShapeForDrawDebug->SetRelativeTransform(DebugRelativeTransform);
	}
}
#endif

