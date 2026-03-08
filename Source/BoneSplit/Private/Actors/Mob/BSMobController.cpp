// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobController.h"

#include "NavigationSystem.h"
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
	if (!GetWorld() || GetWorld()->bIsTearingDown) return;
	ReceiveMoveCompleted.AddDynamic(this, &ABSMobController::CustomOnMoveCompleted);
	
	GetWorldTimerManager().SetTimer(
		ChaseTimerHandle,
		this,
		&ABSMobController::ChasePlayer,
		CheckInterval,
		true
	); 
}

void ABSMobController::StopChasing()
{
	if (!GetWorld() || GetWorld()->bIsTearingDown) return;
	GetWorldTimerManager().ClearTimer(ChaseTimerHandle);
	ReceiveMoveCompleted.RemoveDynamic(this, &ABSMobController::CustomOnMoveCompleted);
	StopMovement();
}

bool ABSMobController::IsOnValidNavMesh()
{
	if (!GetWorld() || !GetPawn() || GetWorld()->bIsTearingDown) return false;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetPawn()->GetWorld());
	if (!NavSys) return false;

	FNavLocation NavLocation;
	const FVector ActorLocation = GetPawn()->GetActorLocation();

	const bool bOnNav = NavSys->ProjectPointToNavigation(
		ActorLocation,
		NavLocation,
		FVector(0.f, 0.f, GetPawn()->GetSimpleCollisionHalfHeight() + 50)  // Query extent (Z tolerance)
	);

	return bOnNav;
}

void ABSMobController::ChasePlayer()
{
	if (!GetWorld() || GetWorld()->bIsTearingDown) return;
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
		FVector VelocityAdjust = { PlayerPawn->GetVelocity().X, PlayerPawn->GetVelocity().Y, 0 };
		MoveRequest.SetGoalLocation(PlayerPawn->GetActorLocation());
		MoveRequest.SetReachTestIncludesGoalRadius(false);
		MoveRequest.SetReachTestIncludesAgentRadius(false);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
		MoveRequest.SetProjectGoalLocation(true);
		MoveRequest.SetUsePathfinding(IsOnValidNavMesh());

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

