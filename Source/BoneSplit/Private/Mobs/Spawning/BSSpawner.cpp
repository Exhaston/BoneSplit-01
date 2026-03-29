// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/Spawning/BSSpawner.h"

#include "Mobs/BSMobCharacterBase.h"


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

ABSMobCharacterBase* ABSSpawner::SpawnMob(const TSubclassOf<ABSMobCharacterBase> MobClass)
{
	if (!MobClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return GetWorld()->SpawnActor<ABSMobCharacterBase>(
		MobClass,
		SpawnPoint->GetComponentLocation(),
		SpawnPoint->GetComponentRotation(),
		Params
	);
}
