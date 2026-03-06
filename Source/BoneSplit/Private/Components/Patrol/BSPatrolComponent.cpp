// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Patrol/BSPatrolComponent.h"
#include "AIController.h"
#include "Components/Patrol/BSPatrolPath.h"


#include "Navigation/PathFollowingComponent.h"

UBSPatrolComponent::UBSPatrolComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBSPatrolComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (!GetOwner()->HasAuthority()) return;
    
    if (FollowActor)
    {
        FollowActorOffset = FollowActor->GetActorTransform().InverseTransformPosition(
            GetOwner()->GetActorLocation()
        );
    }

    if (const AAIController* Controller = GetOwnerController())
    {
        Controller->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
            this, &UBSPatrolComponent::OnMoveCompleted
        );
    }
}

void UBSPatrolComponent::MoveToFollowActor()
{
    if (GetOwner() && GetOwner()->HasAuthority() && FollowActor && GetOwnerController())
    {
        FVector TargetLocation = FollowActor->GetActorTransform().TransformPositionNoScale(FollowActorOffset);
        GetOwnerController()->MoveToLocation(
            TargetLocation, 
            50, true, true, false);
    }
}

void UBSPatrolComponent::StartPatrol()
{
    if (FollowActor)
    {
        bPatrolActive = true;
        return;
    }
    
    if (!GetOwner() || !GetOwner()->HasAuthority()) return; 
    
    if (!PatrolPathActor) return;

    const int32 PointCount = PatrolPathActor->GetPatrolPointCount();
    if (PointCount == 0) return;

    Direction         = bStartReversed ? -1 : 1;
    CurrentPointIndex = bStartReversed ? PointCount - 1 : 0;
    bPatrolActive     = true;
    bWaiting          = false;

    MoveToCurrentPoint();
}

void UBSPatrolComponent::StopPatrol()
{
    bPatrolActive = false;
    bWaiting      = false;

    if (AAIController* Controller = GetOwnerController())
    {
        Controller->StopMovement();
    }
}

void UBSPatrolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (FollowActor && bPatrolActive)
    {
        MoveToFollowActor();
        return;
    }

    if (!bPatrolActive || !bWaiting) return;

    WaitTimeRemaining -= DeltaTime;
    if (WaitTimeRemaining <= 0.f)
    {
        bWaiting = false;
        AdvancePatrolIndex();
        MoveToCurrentPoint();
    }
}

void UBSPatrolComponent::MoveToCurrentPoint()
{
    AAIController* Controller = GetOwnerController();
    if (!Controller || !PatrolPathActor) return;

    const FVector Point = PatrolPathActor->GetPatrolPointAt(CurrentPointIndex);
    Controller->MoveToLocation(Point, 50, true, true, false);
}

void UBSPatrolComponent::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (!bPatrolActive || FollowActor) return;
    if (Result.Flags != FPathFollowingResultFlags::Success) return;

    if (const FBSWayStop* WayStop = PatrolPathActor->GetWayStopAtPatrolIndex(CurrentPointIndex))
    {
        if (WayStop->WaitTime > 0.f)
        {
            bWaiting          = true;
            WaitTimeRemaining = WayStop->WaitTime;
            return;
        }
    }

    AdvancePatrolIndex();
    MoveToCurrentPoint();
}

void UBSPatrolComponent::AdvancePatrolIndex()
{
    const int32 PointCount = PatrolPathActor->GetPatrolPointCount();
    const int32 LastIndex  = PointCount - 1;

    if (PatrolPathActor->IsClosed())
    {
        CurrentPointIndex = (CurrentPointIndex + 1) % PointCount;
    }
    else
    {
        if (CurrentPointIndex + Direction < 0 || CurrentPointIndex + Direction > LastIndex)
        {
            Direction *= -1;
        }

        CurrentPointIndex += Direction;
    }
}

AAIController* UBSPatrolComponent::GetOwnerController()
{
    if (CachedOwnerController.IsValid()) return CachedOwnerController.Get();
    const APawn* OwnerPawn = GetOwner<APawn>();
    if (!OwnerPawn) return nullptr;
    CachedOwnerController = OwnerPawn->GetController<AAIController>();
    return CachedOwnerController.IsValid() ? CachedOwnerController.Get() : nullptr;
}


