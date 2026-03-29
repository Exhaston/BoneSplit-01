// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "MMobMovementComponent.h"


// Sets default values for this component's properties
UMMobMovementComponent::UMMobMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	NavAgentProps.AgentHeight = 128;
	NavAgentProps.AgentRadius = 32;
	NavAgentProps.AgentStepHeight = 60;
	NavAgentProps.bCanWalk = true;
	NavMovementProperties.bUseAccelerationForPaths = false;
	
	MaxStepHeight = 60;
	
	bSlideAlongNavMeshEdge = true;
	NavWalkingFloorDistTolerance = 0;
	bSweepWhileNavWalking = false;
	
	bUseFlatBaseForFloorChecks = false;
	GravityScale = 2;
	
	bUseControllerDesiredRotation = false;
	bOrientRotationToMovement = true;
	DefaultLandMovementMode = MOVE_NavWalking;
	bAllowPhysicsRotationDuringAnimRootMotion = false;
	RotationRate = {0,400, 0};
	
	bRunPhysicsWithNoController = true;
	
	bUseSeparateBrakingFriction = false;
	
	//MaxAcceleration = 8192;
	//BrakingDecelerationWalking = 8192;
	
	AirControl = 0;
	GravityScale = 2;
	
	bRequestedMoveUseAcceleration = false;
	
	bUseRVOAvoidance = true;
	AvoidanceConsiderationRadius = 100;
	AvoidanceWeight = 0.5;
}

float UMMobMovementComponent::GetMaxSpeed() const
{
	if (bNormalWalking)
	{
		return NormalWalkSpeed * GetSpeedModifier();
	}
	return Super::GetMaxSpeed() * GetSpeedModifier();
}

float UMMobMovementComponent::GetMaxSpeedForNavMovement() const
{
	return GetMaxSpeed();
}

float UMMobMovementComponent::GetSpeedModifier() const
{
	return 1;
}

FRotator UMMobMovementComponent::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime,
	FRotator& DeltaRotation) const
{
	if (IsFalling()) return CurrentRotation;
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
