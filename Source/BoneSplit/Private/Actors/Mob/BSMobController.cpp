// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobController.h"

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

void ABSMobController::ChasePlayer()
{
	APawn* AIPawn = GetPawn();
	AActor* PlayerPawn = GetFocusActor();

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
		MoveRequest.SetUsePathfinding(true);

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

