// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Patrol/BSPatrolPath.h"

#include "NavigationSystem.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/SplineComponent.h"

ABSPatrolPath::ABSPatrolPath(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PatrolPathSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolPathComponent"));
	SetRootComponent(PatrolPathSplineComponent);
	
	BSConsoleVariables::CVarBSDebugPatrolPaths->OnChangedDelegate().AddWeakLambda(this, [this] (IConsoleVariable* Variable)
	{
		if (GetWorld())
		{
			if (Variable->GetBool())
			{
				DrawDebugPatrolPath();
			}
			else
			{
				FlushPersistentDebugLines(GetWorld());
			}

		}
	});
	
	UNavigationSystemBase::OnNavigationInitDoneStaticDelegate().AddWeakLambda(this, [this](const UNavigationSystemBase& System)
	{
		if (GetWorld())
		{
			BuildPatrolPath();
		}
	});
	
#if WITH_EDITOR
	
	PatrolPathSplineComponent->EditorUnselectedSplineSegmentColor = FColor::Cyan.ReinterpretAsLinear();
	
#endif
}

TArray<FVector> ABSPatrolPath::GetPatrolPoints() const
{
	return PatrolPoints;
}

int32 ABSPatrolPath::GetPatrolPointCount() const
{
	return PatrolPoints.Num();
}

bool ABSPatrolPath::IsClosed() const
{
	return PatrolPathSplineComponent->IsClosedLoop();
}

FVector ABSPatrolPath::GetPatrolPointAt(const int32 Index) const
{
	check(PatrolPoints.IsValidIndex(Index));
	return PatrolPoints[Index];
}

void ABSPatrolPath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	if (bProjectToNavigation && NavSys && NavSys->IsInitialized())
	{
		BuildPatrolPath();
	}
}

void ABSPatrolPath::DrawDebugPatrolPath() const
{
	const UWorld* World = GetWorld();
	if (!World || PatrolPoints.Num() == 0) return;
	
	FlushPersistentDebugLines(World);

	for (int32 i = 0; i < PatrolPoints.Num(); ++i)
	{
		constexpr float Thickness = 2.f;
		constexpr uint8 DepthPriority = 0;
		constexpr float Duration = -1.f;
		constexpr float PointRadius = 20.f;
		const FVector& Current = PatrolPoints[i];
		
		DrawDebugSphere(World, Current, PointRadius, 8, FColor::Cyan, true, Duration, DepthPriority, Thickness);
		
		const bool bIsClosedLoop = PatrolPathSplineComponent->IsClosedLoop();

		if (const bool bHasNext = PatrolPoints.IsValidIndex(i + 1); bHasNext || bIsClosedLoop)
		{
			const FVector& Next = PatrolPoints[bHasNext ? i + 1 : 0];
			DrawDebugLine(World, Current, Next, FColor::Cyan, true, Duration, DepthPriority, Thickness);
		}
	}
}

void ABSPatrolPath::Destroyed()
{
#if WITH_EDITOR
	if (GetWorld())
	{
		FlushPersistentDebugLines(GetWorld());
	}
#endif
	Super::Destroyed();
}

FBSWayStop ABSPatrolPath::GetWayStopAt(const int32 Index) const
{
	return WayStops[Index];
}

void ABSPatrolPath::BuildPatrolPath()
{
	const int32 SplinePointCount = PatrolPathSplineComponent->GetNumberOfSplinePoints();
	const bool bIsClosedLoop = PatrolPathSplineComponent->IsClosedLoop();
	
	const int32 SegmentCount = bIsClosedLoop ? SplinePointCount : SplinePointCount - 1;

	if (SegmentCount <= 0)
	{
		return;
	}
	
	WayStopByPatrolIndex.Reset();

	PatrolPoints.Reset(SegmentCount * SubSteps);

	UNavigationSystemV1* NavSys = bProjectToNavigation
		? FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())
		: nullptr;

	for (int32 SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
	{
		for (int32 Step = 0; Step < SubSteps; ++Step)
		{
			const float T = static_cast<float>(Step) / static_cast<float>(SubSteps);

			FVector Point = PatrolPathSplineComponent->GetLocationAtSplineInputKey(
				SegmentIndex + T,
				ESplineCoordinateSpace::World
			);

			if (NavSys)
			{
				if (FNavLocation ProjectedLocation; 
					NavSys->ProjectPointToNavigation(Point, ProjectedLocation, ProjectionExtent))
				{
					Point = ProjectedLocation.Location;
				}
			}
			
			PatrolPoints.Add(Point);
			
			if (Step == 0)
			{
				const int32 PatrolIndex = PatrolPoints.Num() - 1;
				for (const FBSWayStop& WayStop : WayStops)
				{
					if (WayStop.SplineIndex == SegmentIndex)
					{
						WayStopByPatrolIndex.Add(PatrolIndex, WayStop);
						break;
					}
				}
			}
		}
	}
	
	if (!bIsClosedLoop)
	{
		FVector LastPoint = PatrolPathSplineComponent->GetLocationAtSplinePoint(
			SplinePointCount - 1,
			ESplineCoordinateSpace::World
		);

		if (NavSys)
		{
			if (FNavLocation ProjectedLocation; 
				NavSys->ProjectPointToNavigation(LastPoint, ProjectedLocation, ProjectionExtent))
			{
				LastPoint = ProjectedLocation.Location;
			}
		}
		
		PatrolPoints.Add(LastPoint);
		
		const int32 LastSplineIndex = SplinePointCount - 1;
		const int32 PatrolIndex = PatrolPoints.Num() - 1;
		for (const FBSWayStop& WayStop : WayStops)
		{
			if (WayStop.SplineIndex == LastSplineIndex)
			{
				WayStopByPatrolIndex.Add(PatrolIndex, WayStop);
				break;
			}
		}
	}

#if WITH_EDITOR
	if (BSConsoleVariables::CVarBSDebugPatrolPaths.GetValueOnGameThread())
	{
		DrawDebugPatrolPath();
	}
#endif
}

void ABSPatrolPath::OnNavGenComplete(ANavigationData* Data)
{
	BuildPatrolPath();
}
