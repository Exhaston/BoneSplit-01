// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "MMobController.h"

#include "NavigationSystem.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Patrol/MMobPatrolComponent.h"
#include "Threat/MThreatComponent.h"

namespace MobConsoleVariables
{
	TAutoConsoleVariable<bool> CVarMMobVicinityDebug(
	TEXT("Mobs.Debug.Vicinity"),false,
	TEXT("Default = 0"),
	ECVF_Default);

	TAutoConsoleVariable<bool> CVarMMobThreatDebug(
		TEXT("Mobs.Debug.Threat"),false,
		TEXT("Default = 0"),
		ECVF_Default);

	TAutoConsoleVariable<bool> CVarMMobFocusDebug(
		TEXT("Mobs.Debug.Focus"),false,
		TEXT("Default = 0"),
		ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarMMobAlliesDebug(
	TEXT("Mobs.Debug.Allies"),false,
	TEXT("Default = 0"),
	ECVF_Default);
}

AMMobController::AMMobController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	PatrolComponent = CreateDefaultSubobject<UMMobPatrolComponent>(TEXT("PatrolComponent"));
	
	ThreatComponent = CreateDefaultSubobject<UMThreatComponent>("ThreatComponent");
	ThreatComponent->GetOnTargetChangedDelegate().AddUObject(this, &AMMobController::OnTargetChanged);
}

void AMMobController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	
	DebugTargets();
	
#endif
	
	if (!AdvanceMobTick(DeltaSeconds)) return;
	
	FindPawnsInVicinity(DeltaSeconds);
	
	TryMoveToTarget(DeltaSeconds);
}

#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
void AMMobController::DebugTargets()
{
	if (const UWorld* CurrentWorld = GetWorld(); CurrentWorld && GetPawn())
	{
		if (MobConsoleVariables::CVarMMobFocusDebug.GetValueOnGameThread() && GetCurrentTargetActor())
		{
			DrawDebugLine(
				CurrentWorld, 
				GetCurrentTargetActor()->GetActorLocation(), 
				GetPawn()->GetActorLocation(), 
				FColor::Green, 
				false, 
				-1);
		}
		if (MobConsoleVariables::CVarMMobThreatDebug.GetValueOnGameThread() && GetHighestThreatActor())
		{
			DrawDebugLine(
				CurrentWorld, 
				GetHighestThreatActor()->GetActorLocation(), 
				GetPawn()->GetActorLocation(), 
				FColor::Red, 
				false, 
				-1);
		}
		if (MobConsoleVariables::CVarMMobAlliesDebug.GetValueOnGameThread() && !GetAlliesInVicinity().IsEmpty())
		{
			for (const auto AllyActor : GetAlliesInVicinity())
			{
				DrawDebugLine(
					CurrentWorld, 
					AllyActor->GetActorLocation(), 
					GetPawn()->GetActorLocation(), 
					FColor::Cyan, 
					false, 
					-1);
			}
		}
	}

}

#endif

void AMMobController::SetCurrentTarget(AActor* InFocus)
{
	SetFocus(InFocus, EAIFocusPriority::Gameplay);
}

void AMMobController::ClearCurrentTarget()
{
	ClearFocus(EAIFocusPriority::Gameplay);
}

void AMMobController::SetMovementPaused(const bool bNewMovementPaused)
{
	bMovementIssuingPaused = bNewMovementPaused;
	if (bNewMovementPaused)
	{
		StopMovement();
	}
}

AActor* AMMobController::GetHighestThreatActor()
{
	return CurrentTarget.IsValid() ? CurrentTarget.Get() : nullptr;
}


void AMMobController::SetFocus(AActor* NewFocus, const EAIFocusPriority::Type InPriority)
{
	Super::SetFocus(NewFocus, InPriority);
	
	if (NewFocus && InPriority == EAIFocusPriority::Gameplay && bAutoPauseMovementWhenFocusSet)
	{
		bMovementIssuingPaused = true;
		StopMovement();
	}
}

void AMMobController::ClearFocus(const EAIFocusPriority::Type InPriority)
{
	Super::ClearFocus(InPriority);
	
	if (InPriority == EAIFocusPriority::Gameplay && bAutoPauseMovementWhenFocusSet)
	{
		bMovementIssuingPaused = false;
	}
}

TArray<AActor*> AMMobController::GetAlliesInVicinity()
{
	TArray<AActor*> ValidAllies;
	TArray<TWeakObjectPtr<AActor>> InvalidAllies;
	
	for (auto& Ally : NearbyAllies)
	{
		Ally.IsValid() ? ValidAllies.Add(Ally.Get()) : InvalidAllies.Add(Ally);
	}
	
	for (auto& InvalidAlly : InvalidAllies)
	{
		NearbyAllies.Remove(InvalidAlly);
	}
	
	return ValidAllies;
}

TArray<AActor*> AMMobController::GetEnemiesInVicinity()
{
	TArray<AActor*> ValidEnemies;
	TArray<TWeakObjectPtr<AActor>> InvalidEnemies;
	
	for (auto& Enemy : NearbyEnemies)
	{
		Enemy.IsValid() ? ValidEnemies.Add(Enemy.Get()) : InvalidEnemies.Add(Enemy);
	}
	
	for (auto& InvalidEnemy : InvalidEnemies)
	{
		NearbyEnemies.Remove(InvalidEnemy);
	}
	
	return ValidEnemies;
}

bool AMMobController::HasLineOfSightToTarget(AActor* Target)
{
	return NearbyEnemies.Contains(Target);
}

bool AMMobController::GetIsMoving()
{
	return IsFollowingAPath();
}

bool AMMobController::AdvanceMobTick(const float DeltaSeconds)
{
	if (!GetPawn() || GetWorld()->bIsTearingDown) return false;
	
	if (MobTickRate <= 0)
	{
		OnMobTickDelegate.Broadcast(CurrentMobTickTime);
		return true;
	}
	
	CurrentMobTickTime += DeltaSeconds;
	
	if (CurrentMobTickTime < MobTickRate)
	{
		return false;
	}
	
	OnMobTickDelegate.Broadcast(CurrentMobTickTime);
	
	CurrentMobTickTime = 0;
	
	return true;
}

void AMMobController::TryMoveToTarget(const float DeltaSeconds)
{
	if (bMovementIssuingPaused) return;
	if (const ACharacter* CharacterPawn = GetPawn<ACharacter>())
	{
		if (CharacterPawn->GetCharacterMovement()->IsFalling()) return;
	}
	else return;
	
	AActor* TargetActor = CurrentTarget.IsValid() ? CurrentTarget.Get() : nullptr;
	if (!TargetActor) return;
	
	const bool bHasLineOfSight = LineOfSightTo(TargetActor);

	const bool bIsWithinDistance = 
		FVector::DistSquared(
			TargetActor->GetActorLocation(), GetPawn()->GetActorLocation()) <= FMath::Square(TargetAcceptanceRadius);
	
	if (bIsWithinDistance && bHasLineOfSight)
	{
		return; //Skip issuing new requests when already within threshold and line of sight is maintained.
	}

	const float AcceptanceRadius = bHasLineOfSight ? TargetAcceptanceRadius : 0;
	
	FVector Destination = TargetActor->GetActorLocation();
	
	ProjectLocationToNavigation(Destination);
	
	MoveToLocation(
		Destination, 
		AcceptanceRadius, 
		false, 
		true, 
		false, 
		false);
}

void AMMobController::ProjectLocationToNavigation(FVector& Location, const FVector Extents)
{
	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		if (FNavLocation ProjectedLocation; 
			NavSys->ProjectPointToNavigation(Location, ProjectedLocation, Extents))
		{
			Location = ProjectedLocation.Location;	
		}
	}
}

void AMMobController::OnTargetChanged(AActor* OldActor, AActor* NewActor)
{
	CurrentTarget = NewActor;
}

void AMMobController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	if (Result.Flags == FPathFollowingResultFlags::Success)
	{
		OnMovementSuccessDelegate.Broadcast();
	}
}

void AMMobController::StartPatrol(AMMobPatrolPath* InPatrolPath)
{
	PatrolComponent->StartPatrol(InPatrolPath);
}

void AMMobController::FindPawnsInVicinity(float DeltaSeconds)
{
	const UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown) return;

	const FVector RefLocation = GetPawn()->GetActorLocation();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetPawn());
	
	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByObjectType(OverlapResults, RefLocation, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeSphere(VicinitySphereRadius), QueryParams);

	TArray<AActor*> CurrentAllies;
	
	TArray<AActor*> CurrentEnemies;
	
	for (auto& OverlapResult : OverlapResults)
	{
		AActor* OverlappedActor = OverlapResult.GetActor();
		if (OverlappedActor == GetPawn()) continue;
		if (!IsValid(OverlappedActor)) continue;
		
		if (IsActorEnemy(OverlappedActor))
		{
			CurrentEnemies.Add(OverlappedActor);
			if (bAddThreatFromVicinityOnce && ThreatComponent->HasThreatForTarget(OverlappedActor))
			{
				continue; //Only add threat for vicinity once
			}
			if (bRequireLineOfSightForPawnGathering && !LineOfSightTo(OverlappedActor)) continue;
			ThreatComponent->AddThreat(OverlappedActor, 1);
			continue;
		}

		if (IsActorAlly(OverlappedActor)) CurrentAllies.Add(OverlappedActor);
	}
	
	for (int32 i = NearbyEnemies.Num() - 1; i >= 0; i--)
	{
		if (!CurrentEnemies.Contains(NearbyEnemies[i]))
		{
			NearbyEnemies.RemoveAt(i);
		}
	}
	
	for (const auto NearbyEnemy : CurrentEnemies)
	{
		NearbyEnemies.AddUnique(NearbyEnemy);
	}

	//Remove allies that left vicinity
	for (int32 i = NearbyAllies.Num() - 1; i >= 0; i--)
	{
		if (!CurrentAllies.Contains(NearbyAllies[i]))
		{
			NearbyAllies.RemoveAt(i);
		}
	}
						  
	//Add newly detected allies
	for (const auto NearbyAlly : CurrentAllies)
	{
		NearbyAllies.AddUnique(NearbyAlly);
	}
	
#if WITH_EDITOR
	
	if (MobConsoleVariables::CVarMMobVicinityDebug.GetValueOnGameThread())
	{
		DrawDebugSphere(
			GetWorld(), 
			RefLocation, 
			VicinitySphereRadius, 
			16, 
			FColor::Red, 
			false, 
			MobTickRate);
	}
	
#endif
	
}

bool AMMobController::IsActorEnemy(AActor* InActor)
{
	if (!GetPawn()) return false;
	return !GetPawn()->Tags.ContainsByPredicate([&](const FName& Tag)
	{
		return InActor->ActorHasTag(Tag);
	});
}

bool AMMobController::IsActorAlly(AActor* InActor)
{
	if (!GetPawn()) return false;
	return !IsActorEnemy(InActor);
}

