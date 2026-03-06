// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobController.h"

#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

ABSMobController::ABSMobController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void ABSMobController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	StartChasing();
}

void ABSMobController::OnUnPossess()
{
	StopChasing();
	Super::OnUnPossess();
}

void ABSMobController::StartChasing()
{
	ReceiveMoveCompleted.AddDynamic(this, &ABSMobController::CustomOnMoveCompleted);
	
	GetWorldTimerManager().SetTimer(
		ChaseTimerHandle,
		this,
		&ABSMobController::ChasePlayer,
		CheckInterval,
		true,
		0.f
	); 
}

void ABSMobController::StopChasing()
{
	GetWorldTimerManager().ClearTimer(ChaseTimerHandle);
	ReceiveMoveCompleted.RemoveDynamic(this, &ABSMobController::CustomOnMoveCompleted);
	StopMovement();
}

bool ABSMobController::IsOnValidNavMesh()
{
	if (!GetPawn()) return false;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetPawn()->GetWorld());
	if (!NavSys) return false;

	FNavLocation NavLocation;
	const FVector ActorLocation = GetPawn()->GetActorLocation();

	const bool bOnNav = NavSys->ProjectPointToNavigation(
		ActorLocation,
		NavLocation,
		FVector(0.f, 0.f, GetPawn()->GetSimpleCollisionHalfHeight())  // Query extent (Z tolerance)
	);

	if (bOnNav)
	{
		const float DistZ = FMath::Abs(NavLocation.Location.Z - ActorLocation.Z);
		return DistZ < 25.f;
	}

	return false;
}

void ABSMobController::ChasePlayer()
{
	const APawn* AIPawn = GetPawn();
	const AActor* PlayerPawn = GetFocusActor();

	if (!AIPawn || !PlayerPawn)
		return;

	const float Distance = FVector::Dist(AIPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
	
	if (Distance <= AcceptanceRadius)
	{
		StopMovement();
		return;
	}
	
	if (Distance > RestartDistance || GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(PlayerPawn);
		MoveRequest.SetReachTestIncludesGoalRadius(false);
		MoveRequest.SetReachTestIncludesAgentRadius(false);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
		MoveRequest.SetUsePathfinding(IsOnValidNavMesh()); //Force move towards if outside navmesh

		MoveTo(MoveRequest);
	}
}

void ABSMobController::CustomOnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		// Reached the player — stop the chase loop
		//StopChasing();
	}
	else
	{
		// Path failed or was interrupted — ChasePlayer() loop will retry automatically
	}
}

