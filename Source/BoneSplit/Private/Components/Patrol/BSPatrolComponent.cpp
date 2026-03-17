// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Patrol/BSPatrolComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Actors/Mob/BSMobController.h"
#include "AI/NavigationSystemBase.h"
#include "Components/Patrol/BSPatrolPath.h"


#include "Navigation/PathFollowingComponent.h"

UBSPatrolComponent::UBSPatrolComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Tick only needed while active; disabled until StartPatrol
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    
}

void UBSPatrolComponent::BeginPlay()
{
    Super::BeginPlay();
    
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        if (APawn* OwnerPawn = GetOwner<APawn>())
        {
            if (OwnerPawn->GetController())
            {
                OnPawnControllerSet(OwnerPawn, nullptr, OwnerPawn->GetController());
            }
            else
            {
               OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UBSPatrolComponent::OnPawnControllerSet); 
            }
        }
    }, 1, false);
    

}

void UBSPatrolComponent::OnPawnControllerSet(APawn* OwnerPawn, AController* OldController, AController* NewController)
{
    MobController = Cast<ABSMobController>(NewController);
    
    if (!MobController) return;
    
    if (FollowActor)
    {
        FollowActorOffset = FollowActor->GetActorTransform().InverseTransformPosition(
            GetOwner()->GetActorLocation());
    }
    
    MobController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
    this, &UBSPatrolComponent::OnMoveCompleted);
    
    if (const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()); 
        NavSys && NavSys->IsInitialized())
    {
        StartPatrol();
    }
    else
    {
        UNavigationSystemBase::OnNavigationInitDoneStaticDelegate().AddWeakLambda(
        this, [this](const UNavigationSystemBase&)
        {
            StartPatrol();
        });
    }
}

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

void UBSPatrolComponent::StartPatrol()
{
    if (!GetOwner() || !GetOwner()->HasAuthority()) return;
    
    if (FollowActor)
    {
        bPatrolActive = true;
        SetComponentTickEnabled(true);
        return;
    }

    if (!PatrolPathActor) return;

    const int32 PointCount = PatrolPathActor->GetPatrolPointCount();
    if (PointCount == 0) return;

    Direction         = bStartReversed ? -1 : 1;
    CurrentPointIndex = bStartReversed ? PointCount - 1 : 0;
    bPatrolActive     = true;
    bWaiting          = false;

    SetComponentTickEnabled(true);
    MoveToCurrentPoint();
}

void UBSPatrolComponent::StopPatrol()
{
    bPatrolActive = false;
    bWaiting      = false;
    SetComponentTickEnabled(false);

    if (MobController)
    {
        MobController->StopMovement();
    }
}

// ---------------------------------------------------------------------------
// Tick
// ---------------------------------------------------------------------------

void UBSPatrolComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bPatrolActive) return;

    if (FollowActor)
    {
        MoveToFollowActor();
        return;
    }

    if (!bWaiting) return;

    WaitTimeRemaining -= DeltaTime;
    if (WaitTimeRemaining <= 0.f)
    {
        bWaiting = false;
        AdvancePatrolIndex();
        MoveToCurrentPoint();
    }
}

// ---------------------------------------------------------------------------
// Movement
// ---------------------------------------------------------------------------

void UBSPatrolComponent::MoveToCurrentPoint()
{
    if (!MobController || !PatrolPathActor) return;
    
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("PATROL"));

    const FVector Point = PatrolPathActor->GetPatrolPointAt(CurrentPointIndex);
    MobController->MoveToLocation(
        Point, 
        AcceptanceRadius,
        true,
       true,
       true);
}

void UBSPatrolComponent::MoveToFollowActor()
{
    if (!FollowActor || !GetOwner() || !GetOwner()->HasAuthority()) return;
    
    if (!MobController) return;

    const FVector TargetLocation =
        FollowActor->GetActorTransform().TransformPositionNoScale(FollowActorOffset);

    // Only re-issue the move request if the target has moved meaningfully,
    // avoids spamming pathfinding every frame
    if (FVector::DistSquared(TargetLocation, LastIssuedFollowLocation)
        < FMath::Square(FollowMoveThreshold))
    {
        return;
    }

    LastIssuedFollowLocation = TargetLocation;
    MobController->MoveToLocation(TargetLocation, AcceptanceRadius,
        /*bStopOnOverlap*/true, /*bUsePathfinding*/true,
        /*bProjectDestinationToNavigation*/false);
}

// ---------------------------------------------------------------------------
// Patrol logic
// ---------------------------------------------------------------------------

void UBSPatrolComponent::OnMoveCompleted(FAIRequestID /*RequestID*/,
    const FPathFollowingResult& Result)
{
    if (!bPatrolActive || FollowActor) return;

    // Ignore partial moves (blocked, aborted, etc.)
    if (!Result.HasFlag(FPathFollowingResultFlags::Success)) return;

    /*
    FBSWayStop WayStop;
    if (PatrolPathActor->TryGetWayStopAtPatrolIndex(CurrentPointIndex, WayStop))
    {
        if (WayStop.WaitTime > 0.f)
        {
            bWaiting          = true;
            WaitTimeRemaining = WayStop.WaitTime;
            return; // Tick will call AdvancePatrolIndex when timer expires
        }
    }
    */

    AdvancePatrolIndex();
    MoveToCurrentPoint();
}

void UBSPatrolComponent::AdvancePatrolIndex()
{
    if (!PatrolPathActor) return;

    const int32 PointCount = PatrolPathActor->GetPatrolPointCount();
    if (PointCount == 0) return;

    const int32 LastIndex = PointCount - 1;

    if (PatrolPathActor->IsClosed())
    {
        // Respect Direction so closed loops can also run in reverse
        CurrentPointIndex = (CurrentPointIndex + Direction + PointCount) % PointCount;
    }
    else
    {
        const int32 Next = CurrentPointIndex + Direction;
        if (Next < 0 || Next > LastIndex)
        {
            Direction        *= -1;
            CurrentPointIndex += Direction;
        }
        else
        {
            CurrentPointIndex = Next;
        }
    }
}


