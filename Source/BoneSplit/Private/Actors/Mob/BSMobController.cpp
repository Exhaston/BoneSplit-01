// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobController.h"

#include "NavigationSystem.h"
#include "Actors/Mob/BSMobCharacter.h"
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
	if (!IsNavSysValid()) return;
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
	if (!IsNavSysValid()) return;
	GetWorldTimerManager().ClearTimer(ChaseTimerHandle);
	ReceiveMoveCompleted.RemoveDynamic(this, &ABSMobController::CustomOnMoveCompleted);
	StopMovement();
}

bool ABSMobController::IsOnValidNavMesh()
{
	if (!IsNavSysValid()) return false;

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

bool ABSMobController::IsNavSysValid()
{
	const UWorld* CurrentWorld = GetWorld();
	return CurrentWorld && !CurrentWorld->bIsTearingDown;
}

void ABSMobController::ChasePlayer()
{
	if (!IsNavSysValid()) return;
	const ABSMobCharacter* AIPawn = GetPawn<ABSMobCharacter>();
	const AActor* PlayerPawn = GetFocusActor();

	if (!AIPawn || !PlayerPawn) return;

	const bool bHasLOS = LineOfSightTo(PlayerPawn);
	const float Distance = FVector::Dist(AIPawn->GetActorLocation(), PlayerPawn->GetActorLocation());

	// Only stop if BOTH close enough AND can see the player
	if (bHasLOS && Distance <= AIPawn->FollowDistance)
	{
		StopMovement();
		return;
	}

	// Restart path if: too far, idle, or lost LOS
	if (Distance > RestartDistance || GetMoveStatus() == EPathFollowingStatus::Idle || !bHasLOS)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(PlayerPawn->GetActorLocation());
		MoveRequest.SetReachTestIncludesGoalRadius(false);
		MoveRequest.SetReachTestIncludesAgentRadius(false);
		MoveRequest.SetAcceptanceRadius(bHasLOS ? AcceptanceRadius : 0.f);
		MoveRequest.SetProjectGoalLocation(true);
		MoveRequest.SetUsePathfinding(IsOnValidNavMesh());

		MoveTo(MoveRequest);
	}
}

void ABSMobController::CustomOnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!IsNavSysValid()) return;
	
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

