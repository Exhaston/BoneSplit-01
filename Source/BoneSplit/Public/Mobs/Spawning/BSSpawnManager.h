// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSSpawnManager.generated.h"

class ABSMobCharacterBase;
class ABSReadyZone;
class ABSDoor;
class ABSSpawner;
class UBSSpawnData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllBatchesComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatchComplete, int32, BatchIndex);

UCLASS(Blueprintable, BlueprintType)
class BONESPLIT_API ABSSpawnManager : public AActor
{
	GENERATED_BODY()

public:
    
    ABSSpawnManager();
    
#if WITH_EDITOR
    
    UFUNCTION(CallInEditor)
    void PopulateEnemiesFromLevel();
    
    UFUNCTION(CallInEditor)
    void PopulateLockables();
    
    UFUNCTION(CallInEditor)
    void DebugConnections();
    
    UFUNCTION(CallInEditor)
    void ClearDebug();
    
#endif
    
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawning")
    bool bIncreaseDifficultyOnFinish = true;
    
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawning")
    bool bAutoStart = false;
    
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawning")
    TObjectPtr<ABSReadyZone> ReadyZone;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void StartSpawning(UBSSpawnData* InSpawnData);

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void StopSpawning();

    // ----------------------------------------------------------------
    //  Events
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintAssignable, Category = "Spawning")
    FOnAllBatchesComplete OnAllBatchesComplete;

    UPROPERTY(BlueprintAssignable, Category = "Spawning")
    FOnBatchComplete OnBatchComplete;

    // ----------------------------------------------------------------
    //  Configuration
    // ----------------------------------------------------------------

    // Default SpawnData to auto-start on BeginPlay (optional)
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
    TObjectPtr<UBSSpawnData> SpawnData;

    // Spawner actors to round-robin when placing mobs
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
    TArray<TObjectPtr<ABSSpawner>> Spawners;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Completion")
    TArray<TObjectPtr<AActor>> LockedActors;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Completion")
    TArray<TObjectPtr<ABSMobCharacterBase>> PreExistingMobs;

protected:
    virtual void BeginPlay() override;

private:
    // ---- Active state ----
    UPROPERTY()
    TObjectPtr<UBSSpawnData> ActiveSpawnData;

    int32 CurrentBatchIndex = 0;
    int32 CurrentInfoIndex  = 0;
    int32 CurrentMobIndex   = 0;
    int32 SpawnerRoundRobin = 0;
    bool  bSpawning         = false;

    UPROPERTY()
    TArray<TObjectPtr<ABSMobCharacterBase>> LiveMobs;

    FTimerHandle BatchDelayTimer;
    FTimerHandle InfoDelayTimer;
    FTimerHandle MobDelayTimer;
    
    virtual void OnCompletion();

    // ---- Flow ----
    void ScheduleNextBatch();
    void ExecuteBatch(int32 BatchIndex);
    void SpawnNextInfo();
    void SpawnNextMob();

    // ---- Helpers ----
    TArray<APawn*> GetPlayerPawns() const;
    ABSSpawner*    GetNextSpawner();
    bool           AreAllMobsDead() const;
    
    int32 TotalMobsToSpawn = 0;
    int32 TotalMobsSpawned = 0;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    bool IsExpectingMoreMobs() const;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    int32 GetTotalMobsToSpawn() const { return TotalMobsToSpawn; }

    UFUNCTION()
    void OnMobDied(ABSMobCharacterBase* Mob);
    
protected:
    
    
};
