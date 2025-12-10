// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobController.h"


ABSMobController::ABSMobController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABSMobController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABSMobController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

