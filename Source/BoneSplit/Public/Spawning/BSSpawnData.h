// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BSSpawnData.generated.h"


struct FBSSpawnBatch;

UCLASS(Blueprintable, BlueprintType, Const)
class BONESPLIT_API UBSSpawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// All batches to execute, in order
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FBSSpawnBatch> Batches;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float GlobalSpawnDelay = 1;

	// If true, loop back to the first batch after all batches complete
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bLooping = false;
};
