// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobController.h"

#include "NavigationSystem.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/PathFollowingComponent.h"

ABSMobController::ABSMobController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void ABSMobController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UCrowdFollowingComponent* CFC = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CFC->SetCrowdSeparationWeight(100, true);
	}
	
}

// Called every frame
void ABSMobController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

