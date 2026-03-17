// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Patrol/BSPatrolPath.h"

#include "NavigationSystem.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/SplineComponent.h"

ABSPatrolPath::ABSPatrolPath(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;

    PatrolPathSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolPathComponent"));
    SetRootComponent(PatrolPathSplineComponent);
    


#if WITH_EDITOR
    PatrolPathSplineComponent->EditorUnselectedSplineSegmentColor = FColor::Cyan.ReinterpretAsLinear();
    
    PatrolPathSplineComponent->GetOnSplineUpdated().AddWeakLambda(this, [this]()
    {
        for (int32 i = 0; i < PatrolPathSplineComponent->GetNumberOfSplinePoints(); ++i)
        {
            PatrolPathSplineComponent->SetSplinePointType(i, ESplinePointType::Linear, false);
        }
    });
    
#endif
}

// ---------------------------------------------------------------------------
// Geometry helpers
// ---------------------------------------------------------------------------

float ABSPatrolPath::PatrolIndexToSplineKey(const int32 PatrolIndex) const
{
    // Each spline segment is subdivided into SubSteps virtual points.
    // The last point of an open spline sits exactly on the final spline knot.
    return static_cast<float>(PatrolIndex) / static_cast<float>(SubSteps);
}

FVector ABSPatrolPath::ProjectToNav(const FVector& Point) const
{
    if (!bProjectToNavigation) return Point;

    if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        if (FNavLocation Projected;
            NavSys->ProjectPointToNavigation(Point, Projected, ProjectionExtent))
        {
            return Projected.Location;
        }
    }
    return Point;
}

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

int32 ABSPatrolPath::GetPatrolPointCount() const
{
    return PatrolPathSplineComponent->GetNumberOfSplinePoints();
}

FVector ABSPatrolPath::GetPatrolPointAt(const int32 PatrolIndex) const
{
    const FVector Raw = PatrolPathSplineComponent->GetLocationAtSplineInputKey(
        PatrolIndex, ESplineCoordinateSpace::World);
    return Raw;
}

bool ABSPatrolPath::TryGetWayStopAtPatrolIndex(const int32 PatrolIndex, FBSWayStop& OutWayStop) const
{
    // WayStops only align to spline knots (Step == 0), so bail early for mid-segment indices
    if (PatrolIndex % SubSteps != 0) return false;

    const int32 SplineIndex = PatrolIndex / SubSteps;

    for (const FBSWayStop& WayStop : WayStops)
    {
        if (WayStop.SplineIndex == SplineIndex)
        {
            OutWayStop = WayStop;
            return true;
        }
    }
    return false;
}

bool ABSPatrolPath::IsClosed() const
{
    return PatrolPathSplineComponent->IsClosedLoop();
}
