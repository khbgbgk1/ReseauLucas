// Fill out your copyright notice in the Description page of Project Settings.


#include "LagRewindComponent.h"

#include "NetworkGameInstanceSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogLagRewindComponent, Log, All);


// Sets default values for this component's properties
ULagRewindComponent::ULagRewindComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULagRewindComponent::Server_SetShowRewindHistory_Implementation(bool bNewValue)
{
	ShowRewindHistory = bNewValue;
}

void ULagRewindComponent::Server_UpdateMaximunRecordTime_Implementation(float NewValue)
{
	MaximumRecordTime = NewValue;
}

void ULagRewindComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// On réplique la variable à tout le monde
	DOREPLIFETIME(ULagRewindComponent, ShowRewindHistory);
	DOREPLIFETIME(ULagRewindComponent, MaximumRecordTime);
}


// Called when the game starts
void ULagRewindComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULagRewindComponent::DrawDebugMove(const FSavedMove& Move, FColor Color, float Duration)
{
	if (!ReferenceShapeForDrawDebug)
	{
		UE_LOG(LogLagRewindComponent, Warning, TEXT("DrawDebugMove: ReferenceShapeForDrawDebug est NULL sur %s"), *GetOwner()->GetName());
		return;
	}

	FTransform FinalVisualTransform = DebugRelativeTransform * Move.Position;
	FVector Location = FinalVisualTransform.GetLocation();
	FQuat Rotation = FinalVisualTransform.GetRotation();
	FVector Scale = FinalVisualTransform.GetScale3D();

	if (UBoxComponent* Box = Cast<UBoxComponent>(ReferenceShapeForDrawDebug))
	{
		DrawDebugBox(GetWorld(), Location, Box->GetUnscaledBoxExtent() * Scale, Rotation, Color, false, Duration);
	}
	else if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(ReferenceShapeForDrawDebug))
	{
		float ScaledHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight() * Scale.Z;
		float ScaledRadius = Capsule->GetUnscaledCapsuleRadius() * FMath::Max(Scale.X, Scale.Y);
		DrawDebugCapsule(GetWorld(), Location, ScaledHalfHeight, ScaledRadius, Rotation, Color, false, Duration);
	}
	else if (USphereComponent* Sphere = Cast<USphereComponent>(ReferenceShapeForDrawDebug))
	{
		float ScaledRadius = Sphere->GetUnscaledSphereRadius() * Scale.GetMax();
		DrawDebugSphere(GetWorld(), Location, ScaledRadius, 16, Color, false, Duration);
	}
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
		if (ShowRewindHistory)
		{
			ShowHistory();
		}
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
	
	//on reccupere le temps synchronisé au serveur
	if (UNetworkGameInstanceSubsystem* NetworkSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UNetworkGameInstanceSubsystem>())
	{
		NewMove.Time = NetworkSubsystem->GetSyncedServerTime();
	}
	else
	{
		NewMove.Time = GetWorld()->GetTimeSeconds();
	}
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
		DrawDebugMove(Move, HistoryShapeColor, ShapeRewindLifeDuration);
	}
}

FSavedMove ULagRewindComponent::GetClosestMoveToTime(float Time)
{
	FSavedMove BestMove;
	BestMove.Time = -1.0f;
	for(auto It = SavedMoves.GetHead(); It != nullptr; It = It->GetNextNode())
	{
		if (It->GetValue().Time <= Time)
		{
			BestMove = It->GetValue();
			break; 
		}
	}
	return BestMove;
}

void ULagRewindComponent::DrawValidationHit(FTransform ActorTransformAtTime)
{
	if (!ShowRewindHit)
	{
		UE_LOG(LogLagRewindComponent, Log, TEXT("Client_DrawValidationHit: ShowRewindHit désactivé pas de shape tracé"));
		return;
	}
	UE_LOG(LogLagRewindComponent, Log, TEXT("Client_DrawValidationHit: Dessin forcé par le serveur sur %s"), *GetOwner()->GetName());

	// On crée un SavedMove temporaire pour réutiliser ta fonction de dessin
	FSavedMove TempMove;
	TempMove.Position = ActorTransformAtTime;
	TempMove.Time = 0.0f; // Peu importe ici

	// On dessine la shape (Jaune par exemple)
	DrawDebugMove(TempMove, HitShapeColor, ShapeRewindLifeDurationForHit);
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

