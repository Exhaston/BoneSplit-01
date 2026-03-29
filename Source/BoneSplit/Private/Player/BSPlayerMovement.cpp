// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/BSPlayerMovement.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "GameFramework/Character.h"


UBSPlayerMovement::UBSPlayerMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	NavAgentProps.AgentHeight = 144;
	NavAgentProps.AgentRadius = 34;
	NavAgentProps.bCanWalk = true;
	PerchRadiusThreshold = 10;
	PerchAdditionalHeight = 45;
	MaxStepHeight = 45;
	SetWalkableFloorAngle(45);
	bUseFlatBaseForFloorChecks = false;
	DefaultLandMovementMode = MOVE_Walking;
	bRunPhysicsWithNoController = true;
	bUseSeparateBrakingFriction = false;
	MaxAcceleration = 8192;
	BrakingDecelerationWalking = 8192;
	AirControl = 0.1;
	GravityScale = 2;
	RotationRate = {0,500, 0};
	
	bUseControllerDesiredRotation = true;
	bOrientRotationToMovement = false;
}

void UBSPlayerMovement::SetupPlayerMovement(UAbilitySystemComponent* InAbilitySystem)
{
	check(InAbilitySystem);
	OwnerAbilitySystem = InAbilitySystem;
}

float UBSPlayerMovement::GetMaxSpeed() const
{
	return Super::GetMaxSpeed() * GetSpeedModifier();
}

float UBSPlayerMovement::GetSpeedModifier() const
{
	return OwnerAbilitySystem.IsValid() ? 
	OwnerAbilitySystem->GetNumericAttribute(UBSExtendedAttributeSet::GetSpeedAttribute()) : 0;
}

FRotator UBSPlayerMovement::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime,
	FRotator& DeltaRotation) const
{
	if (Velocity.SizeSquared() < UE_KINDA_SMALL_NUMBER)
	{
		// AI path following request can orient us in that direction (it's effectively an acceleration)
		if (bHasRequestedVelocity && Velocity.SizeSquared() > UE_KINDA_SMALL_NUMBER)
		{
			return Velocity.GetSafeNormal().Rotation();
		}

		// Don't change rotation if there is no acceleration.
		return CurrentRotation;
	}

	// Rotate toward direction of acceleration.
	return Velocity.GetSafeNormal().Rotation();
}

bool UBSPlayerMovement::DoJump(const bool bReplayingMoves, const float DeltaTime)
{
	const float CalculatedJumpZVelocity =  
		FMath::Sqrt(2.0f * -GetGravityZ() * (OwnerAbilitySystem.IsValid() ? 
			OwnerAbilitySystem->GetNumericAttribute(UBSExtendedAttributeSet::GetJumpHeightAttribute()) * 100 : 0));
	
	//From the Super::. Needed to modify directly for convenience with coyote time.
	if (CharacterOwner && (CharacterOwner->CanJump() || IsCoyoteTimeActive()))
	{
		if (!bConstrainToPlane || 
			!FMath::IsNearlyEqual(FMath::Abs(GetGravitySpaceZ(PlaneConstraintNormal)), 1.f))
		{
			if (const bool bFirstJump = CharacterOwner->JumpCurrentCountPreJump == 0; 
				bFirstJump || bDontFallBelowJumpZVelocityDuringJump)
			{
				if (HasCustomGravity())
				{
					SetGravitySpaceZ(
						Velocity, 
						FMath::Max<FVector::FReal>(GetGravitySpaceZ(Velocity), CalculatedJumpZVelocity));
				}
				else
				{
					Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, CalculatedJumpZVelocity);
				}
			}
			
			if (IsCoyoteTimeActive())
			{
				ConsumeCoyoteTime();
			}
			
			//Double jump snap to input direction. GetCurrentAcceleration is replicated
			if (!IsMovingOnGround() && !FMath::IsNearlyZero(GetCurrentAcceleration().Length()))
			{
				const float VerticalVelocity = Velocity.Z;
				Velocity = GetCurrentAcceleration().GetSafeNormal() * GetMaxSpeed();
				Velocity.Z = VerticalVelocity;
			}
			
			SetMovementMode(MOVE_Falling);
			return true;
		}
	}
	
	return false;
}

bool UBSPlayerMovement::CanAttemptJump() const
{
	return !CharacterOwner->IsPlayingRootMotion() && CanMove() && Super::CanAttemptJump();
}

void UBSPlayerMovement::HandleWalkingOffLedge(const FVector& PreviousFloorImpactNormal,
	const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	Super::HandleWalkingOffLedge(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation, TimeDelta);
	StartCoyoteTime();
}

void UBSPlayerMovement::StartCoyoteTime()
{
	CoyoteStartTime = GetWorld()->GetTimeSeconds();
}

bool UBSPlayerMovement::IsCoyoteTimeActive() const
{
	return !IsMovingOnGround() && 
		GetWorld()->GetTimeSeconds() - CoyoteTimeDuration < CoyoteStartTime;
}

void UBSPlayerMovement::ConsumeCoyoteTime()
{
	CharacterOwner->JumpCurrentCount = 0;
	CoyoteStartTime = 0;
}

