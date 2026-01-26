// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSLauncherBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
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
	TargetVisualizer->SetCapsuleHalfHeight(100);
	TargetVisualizer->SetCapsuleRadius(34);
	
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

#if WITH_EDITOR

bool ABSLauncherBase::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ABSLauncherBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (GetWorld() && !GetWorld()->IsGameWorld())
	{
		ShowApproximatePath();
	}
}



void ABSLauncherBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (IsValid(this) && GetWorld() && !GetWorld()->IsGameWorld())
	{
		if (IsSelectedInEditor())
		{
			ShowApproximatePath();
		}
	}
}

static bool SolveProjectileFlightTime(
	float StartZ,
	float TargetZ,
	float InitialVz,
	float GravityZ,
	float& OutTime
)
{
	const float A = 0.5f * GravityZ;
	const float B = InitialVz;
	const float C = StartZ - TargetZ;

	const float Discriminant = B * B - 4.f * A * C;
	if (Discriminant < 0.f)
	{
		return false;
	}

	const float SqrtDisc = FMath::Sqrt(Discriminant);

	const float T1 = (-B + SqrtDisc) / (2.f * A);
	const float T2 = (-B - SqrtDisc) / (2.f * A);
	
	OutTime = FMath::Max(T1, T2);
	return OutTime > 0.f;
}

static float EstimateProjectileArcLength(
	const FVector& Start,
	const FVector& LaunchVelocity,
	const float GravityZ,
	const float FlightTime
)
{
	constexpr int32 Samples = 16;

	float Length = 0.f;
	FVector Prev = Start;

	for (int32 i = 1; i <= Samples; ++i)
	{
		const float Alpha = static_cast<float>(i) / static_cast<float>(Samples);
		const float Time = Alpha * FlightTime;

		const FVector Curr =
			Start +
			LaunchVelocity * Time +
			0.5f * FVector(0.f, 0.f, GravityZ) * Time * Time;

		Length += FVector::Distance(Prev, Curr);
		Prev = Curr;
	}

	return Length;
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
	const FVector End = TargetLocator->GetComponentLocation();

	// If velocity is downward or flat, fall back to straight line
	if (LaunchVelocity.GetSafeNormal().Dot(FVector::UpVector) <= 0.f)
	{
		TrajectorySpline->AddSplinePoint(Start, ESplineCoordinateSpace::World);
		TrajectorySpline->AddSplinePoint(End, ESplineCoordinateSpace::World);
		TrajectorySpline->UpdateSpline();
		return;
	}

	const float GravityZ = GetWorld()->GetGravityZ() * 2.f;

	float FlightTime;
	if (!SolveProjectileFlightTime(
		Start.Z,
		End.Z,
		LaunchVelocity.Z,
		GravityZ,
		FlightTime))
	{
		return;
	}

	constexpr float DesiredPointSpacing = 200; // units between spline points

	const float ArcLength = EstimateProjectileArcLength(
		Start,
		LaunchVelocity,
		GravityZ,
		FlightTime
	);

	const int32 NumPoints = FMath::Clamp(
		FMath::CeilToInt(ArcLength / DesiredPointSpacing),
		8,
		64
	);

	for (int32 i = 0; i <= NumPoints; ++i)
	{
		const float Alpha = static_cast<float>(i) / static_cast<float>(NumPoints);
		const float Time = Alpha * FlightTime;

		const FVector Point =
			Start +
			LaunchVelocity * Time +
			0.5f * FVector(0.f, 0.f, GravityZ) * Time * Time;

		TrajectorySpline->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
	}
	
	TrajectorySpline->SetLocationAtSplinePoint(
		TrajectorySpline->GetNumberOfSplinePoints() - 1,
		End,
		ESplineCoordinateSpace::World
	);

	TrajectorySpline->UpdateSpline();
}

#endif

void ABSLauncherBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!OtherActor->HasLocalNetOwner()) return;
	
	if (ACharacter* CharacterToLaunch = Cast<ACharacter>(OtherActor))
	{
		if (FVector LaunchVelocity; 
			ComputeLaunchVelocity(OtherActor, LaunchVelocity))
		{
			CharacterToLaunch->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}

bool ABSLauncherBase::ComputeLaunchVelocity(const AActor* Target, FVector& OutLaunchVelocity) const
{
	return UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		GetWorld(), 
		OutLaunchVelocity, 
		Target->GetActorLocation(), 
		TargetLocator->GetComponentLocation(),
		GetWorld()->GetGravityZ() * 2,
		ArcDistribution
		);
}



