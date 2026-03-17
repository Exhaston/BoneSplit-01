// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Spawning/BSSpawner.h"

#include "Actors/Mob/BSMobCharacter.h"


ABSSpawner::ABSSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	RootComponent = SpawnPoint;
}

void ABSSpawner::BeginPlay()
{
	Super::BeginPlay();
}

ABSMobCharacter* ABSSpawner::SpawnMob(const TSubclassOf<ABSMobCharacter> MobClass)
{
	if (!MobClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return GetWorld()->SpawnActor<ABSMobCharacter>(
		MobClass,
		SpawnPoint->GetComponentLocation(),
		SpawnPoint->GetComponentRotation(),
		Params
	);
}
