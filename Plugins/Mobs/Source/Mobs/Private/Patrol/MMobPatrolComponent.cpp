// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Patrol/MMobPatrolComponent.h"

#include "AIController.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"


UMMobPatrolComponent::UMMobPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UMMobPatrolComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	OwnerController = Cast<AAIController>(GetOwner());
}

void UMMobPatrolComponent::StartPatrol(AMMobPatrolPath* InPatrolPath)
{
	if (!OwnerController) return;

	// Bind once so we know when each move finishes
	OwnerController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
		this, &UMMobPatrolComponent::OnMoveCompleted);
	
	SelectedPatrolPath = InPatrolPath;
	if (!SelectedPatrolPath) return;
	MoveToNextWayPoint();
}

void UMMobPatrolComponent::StopPatrol() const
{
	if (!OwnerController) return;
	OwnerController->StopMovement();
	OwnerController->GetWorldTimerManager().ClearAllTimersForObject(this);
	// Bind once so we know when each move finishes
	OwnerController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
}

void UMMobPatrolComponent::MoveToNextWayPoint()
{
	if (!SelectedPatrolPath || !OwnerController) return;

	SelectedPatrolPath->GetNextWayPoint(CurrentWayPoint, CurrentWayPointIndex, CurrentWayPointIndex);

	OwnerController->MoveToLocation(
		CurrentWayPoint.WayPointTransform.GetLocation(),
		PatrolPointAcceptanceRadius,
		false);
}

void UMMobPatrolComponent::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	// Ignore failed/interrupted moves (e.g. during combat)
	if (!Result.IsSuccess()) return;

	const FFloatRange& WaitRange = CurrentWayPoint.WaitTimeRange;
	const float WaitTime = FMath::FRandRange(
		FMath::Clamp(WaitRange.GetLowerBoundValue(), 0.f, FLT_MAX),
		WaitRange.GetUpperBoundValue());

	if (WaitTime > SMALL_NUMBER)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WaitTimerHandle,
			this, &UMMobPatrolComponent::MoveToNextWayPoint,
			WaitTime,
			false);
	}
	else
	{
		MoveToNextWayPoint();
	}
}

