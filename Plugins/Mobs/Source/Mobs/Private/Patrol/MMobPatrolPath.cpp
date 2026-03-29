// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Patrol/MMobPatrolPath.h"

#include "NavigationSystem.h"


AMMobPatrolPath::AMMobPatrolPath(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
}

void AMMobPatrolPath::GetNextWayPoint(FMMobWayPoint& OutWayPoint, const int32 CurrentPointIndex, int32& OutNextIndex)
{
	const FVector PointLocation = GetActorLocation();
	
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	
	if (!NavSystem) return;
	
	if (!bUseWayPoints || WayPoints.IsEmpty())
	{
		FNavLocation ProjectedRandomLocation;
		NavSystem->GetRandomReachablePointInRadius(
			PointLocation, RandomPatrolRadius, ProjectedRandomLocation);
		
		OutWayPoint.WayPointTransform.SetLocation(ProjectedRandomLocation.Location);
		OutWayPoint.WaitTimeRange = RandomPatrolWaitTimeRange;
		OutWayPoint.EventTag = "Random";
		OutNextIndex = 0;
		return;
	}
	
	if (bRandomOrder)
	{
		int32 NewIndex;
		do { NewIndex = FMath::RandRange(0, WayPoints.Num() - 1); } 
		while (WayPoints.Num() > 1 && NewIndex == CurrentPointIndex);
		OutNextIndex = NewIndex;
		
		OutWayPoint =  WayPoints[OutNextIndex];
	}
	else
	{
		OutNextIndex = WayPoints.IsValidIndex(CurrentPointIndex + 1) ? CurrentPointIndex + 1 : 0;
		OutWayPoint =  WayPoints[OutNextIndex];
	}

	
	const FVector TransformedPosition = 
		GetActorTransform().TransformPosition(OutWayPoint.WayPointTransform.GetLocation());
	
	if (FNavLocation ProjectedLocation; 
		NavSystem->ProjectPointToNavigation(
			TransformedPosition, 
			ProjectedLocation, 
			{0,0,500}))
	{
		OutWayPoint.WayPointTransform.SetLocation(ProjectedLocation.Location);
		return;
	}
	
	OutWayPoint.WayPointTransform.SetLocation(TransformedPosition);
	
}

void AMMobPatrolPath::BeginPlay()
{
	Super::BeginPlay();
	
}

