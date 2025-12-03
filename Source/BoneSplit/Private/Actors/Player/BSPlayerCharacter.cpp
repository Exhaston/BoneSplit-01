// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerCharacter.h"


ABSPlayerCharacter::ABSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABSPlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

