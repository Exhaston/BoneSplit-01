// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSLauncherBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Interfaces/BSMovementInterface.h"
#include "Kismet/GameplayStatics.h"


ABSLauncherBase::ABSLauncherBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
	TargetLocator = CreateDefaultSubobject<USceneComponent>("TargetLocator");
	TargetLocator->SetupAttachment(RootComponent);

#if WITH_EDITORONLY_DATA
	
	TargetVisualizer = CreateDefaultSubobject<UCapsuleComponent>("TargetVisualizer");
	TargetVisualizer->SetupAttachment(TargetLocator);
	TargetVisualizer->SetIsVisualizationComponent(true);
	TargetVisualizer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	TrajectorySpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrajectorySpline"));
	TrajectorySpline->SetupAttachment(RootComponent);
	TrajectorySpline->SetIsVisualizationComponent(true);
	TrajectorySpline->EditorUnselectedSplineSegmentColor = FColor::Green;
	TrajectorySpline->SetMobility(EComponentMobility::Movable);

#endif
	
#if WITH_EDITOR
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
#endif
}

bool ABSLauncherBase::ShouldTickIfViewportsOnly() const
{
	return true;
}

#if WITH_EDITOR

void ABSLauncherBase::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	
}

void ABSLauncherBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ShowApproximatePath();
}

#endif

void ABSLauncherBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
#if WITH_EDITOR
	
	if (IsValid(this) && GetWorld() && IsSelectedInEditor())
	{
		ShowApproximatePath();
	}
	
#endif
}

void ABSLauncherBase::ShowApproximatePath() const
{
	FVector LaunchVelocity;

	if (!ComputeLaunchVelocity(this, LaunchVelocity))
	{
		TrajectorySpline->ClearSplinePoints();
		return;
	}
	
	TrajectorySpline->ClearSplinePoints();
	
	const FVector Start = GetActorLocation();
	
	if (LaunchVelocity.GetSafeNormal().Dot(FVector::UpVector) <= 0) //To handle 
	{
		const FVector EndLoc = TargetLocator->GetComponentLocation();
		
		TrajectorySpline->AddSplinePoint(Start, ESplineCoordinateSpace::World, false);
		TrajectorySpline->AddSplinePoint(EndLoc, ESplineCoordinateSpace::World, false);
		TrajectorySpline->UpdateSpline();
		return;
	}
	
	const float GravityZ = GetWorld()->GetGravityZ() * 2.f;
	constexpr float TimeStep = 0.1f;
	const float MaxTime = LaunchCurveLength;
	
	int32 PointIndex = 0;

	for (float Time = 0.f; Time <= MaxTime; Time += TimeStep)
	{
		const FVector Point =
			Start +
			LaunchVelocity * Time +
			0.5f * FVector(0.f, 0.f, GravityZ) * Time * Time;

		TrajectorySpline->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
		PointIndex++;
	}

	TrajectorySpline->UpdateSpline();
}

void ABSLauncherBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!OtherActor->HasLocalNetOwner()) return;
	
	if (IBSMovementInterface* MovementInterface = Cast<IBSMovementInterface>(OtherActor))
	{
		if (FVector LaunchVelocity; 
			ComputeLaunchVelocity(OtherActor, LaunchVelocity))
		{
			MovementInterface->LaunchActor(LaunchVelocity.GetSafeNormal(), LaunchVelocity.Length());
		}
	}
}

bool ABSLauncherBase::ComputeLaunchVelocity(const AActor* Target, FVector& OutLaunchVelocity) const
{
	if (ArcDistribution == 1) //Up...
	{
		
	}
	return UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		GetWorld(), 
		OutLaunchVelocity, 
		Target->GetActorLocation(), 
		TargetLocator->GetComponentLocation(),
		GetWorld()->GetGravityZ() * 2,
		ArcDistribution
		);
}

