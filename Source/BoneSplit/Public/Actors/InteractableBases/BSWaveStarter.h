// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSWaveStarter.generated.h"

class ABSLockableActor;
class IBSInteractableInterface;
class UBSWaveDataAsset;
class ABSMobSpawner;

/*
 * 
 */
UCLASS(DisplayName="Wave Starter", Category="BoneSplit", Blueprintable, BlueprintType)
class BONESPLIT_API ABSWaveStarter : public AActor
{
	GENERATED_BODY()

public:
	
	explicit ABSWaveStarter(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Completion")
	TArray<ABSLockableActor*> LockedActors;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UBSWaveDataAsset* SpawnBatch;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<ABSMobSpawner*> Spawners;
	
	UFUNCTION(BlueprintCallable)
	void StartCombat() const;
	
	UFUNCTION(BlueprintCallable)
	void ForceStopCombat();
};
