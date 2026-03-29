// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "UObject/Object.h"
#include "BSSpawnBatch.generated.h"

class ABSMobCharacterBase;

USTRUCT(BlueprintType)
struct FBSMobSpawnInfo
{
	GENERATED_BODY()

	// If true, this mob slot won't spawn until all previously spawned mobs are dead
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRequireClear = false;
	
	// Delay (seconds) between each individual FBSMobSpawnInfo entry within this batch
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 0.f))
	float SpawnDelay = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ABSMobCharacterBase> SpawnedMob;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 1))
	FScalableFloat Count = 1;
};

USTRUCT(BlueprintType)
struct FBSSpawnBatch
{
	GENERATED_BODY()

	// If true, this entire batch waits until all previous batch mobs are dead before starting
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRequireClear = false;

	// Delay (seconds) before this batch starts spawning, after the previous batch finishes (or clears)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 0.f))
	float BatchDelay = 0.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FBSMobSpawnInfo> SpawnInfos;
};
