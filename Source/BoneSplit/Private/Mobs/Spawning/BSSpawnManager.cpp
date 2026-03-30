// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/Spawning/BSSpawnManager.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Group/BSReadyZone.h"
#include "LockedActors/BSLockableInterface.h"
#include "Mobs/BSMobCharacterBase.h"
#include "Mobs/Spawning/BSSpawnBatch.h"
#include "Mobs/Spawning/BSSpawnData.h"
#include "Mobs/Spawning/BSSpawner.h"


ABSSpawnManager::ABSSpawnManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates      = true;
    bAlwaysRelevant  = true;
    SetNetUpdateFrequency(30);
    
    SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
}

#if WITH_EDITOR

void ABSSpawnManager::DebugConnections()
{
    if (!GetWorld()) return;
    
    FlushPersistentDebugLines(GetWorld());

    for (const auto Lockable : LockedActors)
    {
        if (!Lockable) continue;
        DrawDebugLine(GetWorld(), Lockable->GetActorLocation(), GetActorLocation(), FColor::Green, true, -1, 1, 5);
    }

    for (auto Spawner : Spawners)
    {
        if (!Spawner) continue;
        DrawDebugLine(GetWorld(), Spawner->GetActorLocation(), GetActorLocation(), FColor::Red, true, -1, 1, 5);
    }
    
    if (ReadyZone)
    {
        DrawDebugLine(GetWorld(), ReadyZone->GetActorLocation(), GetActorLocation(), FColor::Yellow, true, -1, 1, 5);
    }

    for (auto ExistingMob : PreExistingMobs)
    {
        if (!ExistingMob) continue;
        DrawDebugLine(GetWorld(), ExistingMob->GetActorLocation(), GetActorLocation(), FColor::Red, true, -1, 1, 5);
    }
}

void ABSSpawnManager::ClearDebug()
{
    if (!GetWorld()) return;
    
    FlushPersistentDebugLines(GetWorld());
}

#endif

void ABSSpawnManager::BeginPlay()
{
    Super::BeginPlay();
    
    if (!GetWorld()->GetAuthGameMode()) return;
    
    for (auto Mob : PreExistingMobs)
    {
        if (!Mob) continue;
        LiveMobs.AddUnique(Mob);
        Mob->GetOnMobDiedDelegate().AddUObject(this, &ABSSpawnManager::OnMobDied);
        ++TotalMobsSpawned;
    }
    
    if (!SpawnData) return;
    
    if (bAutoStart)
    {
        StartSpawning(SpawnData);
        return;
    }
    
    if (ReadyZone)
    {
        ReadyZone->OnAllPlayersReadyDelegate.AddWeakLambda(this, [this]()
        {
            StartSpawning(SpawnData);
        });
    }
}

// ============================================================
//  Public API
// ============================================================

void ABSSpawnManager::StartSpawning(UBSSpawnData* InSpawnData)
{
    if (!HasAuthority()) return;
    if (!InSpawnData || InSpawnData->Batches.IsEmpty()) return;
    
    for (auto Lockable : LockedActors)
    {
        if (IBSLockableInterface* LockI = Cast<IBSLockableInterface>(Lockable))
        {
            LockI->SetLockState(true);
        }
    }

    StopSpawning();

    ActiveSpawnData   = InSpawnData;
    CurrentBatchIndex = 0;
    CurrentInfoIndex  = 0;
    CurrentMobIndex   = 0;
    bSpawning         = true;
    
    TotalMobsToSpawn = 0;
    for (const FBSSpawnBatch& Batch : InSpawnData->Batches)
    {
        for (const FBSMobSpawnInfo& Info : Batch.SpawnInfos)
        {
            TotalMobsToSpawn += Info.Count.AsInteger(1);
        }
    }

    ScheduleNextBatch();
}

void ABSSpawnManager::StopSpawning()
{
    bSpawning = false;
    GetWorldTimerManager().ClearTimer(BatchDelayTimer);
    GetWorldTimerManager().ClearTimer(InfoDelayTimer);
    GetWorldTimerManager().ClearTimer(MobDelayTimer);
    LiveMobs.Empty();
}

// ============================================================
//  Internal Flow:  Batch → Info → Mob
//
//  ScheduleNextBatch
//      └─ waits BatchDelay, then ExecuteBatch
//            └─ SpawnNextInfo
//                  └─ waits for bRequireClear if needed
//                  └─ resets MobIndex, calls SpawnNextMob
//                        └─ spawns one mob
//                        └─ waits MobSpawnDelay, repeats until Count reached
//                        └─ when Count reached: waits PostInfoDelay, calls SpawnNextInfo
// ============================================================

void ABSSpawnManager::OnCompletion()
{
    for (auto Lockable : LockedActors)
    {
        if (IBSLockableInterface* LockI = Cast<IBSLockableInterface>(Lockable))
        {
            LockI->SetLockState(false);
        }
    }
}

void ABSSpawnManager::ScheduleNextBatch()
{
    if (!bSpawning || !ActiveSpawnData) return;

    if (!ActiveSpawnData->Batches.IsValidIndex(CurrentBatchIndex))
    {
        if (ActiveSpawnData->bLooping)
        {
            CurrentBatchIndex = 0;
            ScheduleNextBatch();
        }
        else
        {
            bSpawning = false;
            OnAllBatchesComplete.Broadcast();

        }
        return;
    }

    const FBSSpawnBatch& Batch = ActiveSpawnData->Batches[CurrentBatchIndex];

    // Poll until clear if required
    if (Batch.bRequireClear && !AreAllMobsDead())
    {
        FTimerDelegate Poll;
        Poll.BindUObject(this, &ABSSpawnManager::ScheduleNextBatch);
        GetWorldTimerManager().SetTimer(BatchDelayTimer, Poll, 0.5f, false);
        return;
    }

    const float Delay = Batch.BatchDelay > 0.f ? Batch.BatchDelay : ActiveSpawnData->GlobalSpawnDelay;

    if (Delay > 0.f)
    {
        FTimerDelegate Del;
        Del.BindUObject(this, &ABSSpawnManager::ExecuteBatch, CurrentBatchIndex);
        GetWorldTimerManager().SetTimer(BatchDelayTimer, Del, Delay, false);
    }
    else
    {
        ExecuteBatch(CurrentBatchIndex);
    }
}

void ABSSpawnManager::ExecuteBatch(int32 BatchIndex)
{
    if (!bSpawning || !ActiveSpawnData) return;

    CurrentBatchIndex = BatchIndex;
    CurrentInfoIndex  = 0;
    SpawnNextInfo();
}

void ABSSpawnManager::SpawnNextInfo()
{
    if (!bSpawning || !ActiveSpawnData) return;

    const FBSSpawnBatch& Batch = ActiveSpawnData->Batches[CurrentBatchIndex];

    if (!Batch.SpawnInfos.IsValidIndex(CurrentInfoIndex))
    {
        // All infos in this batch done
        OnBatchComplete.Broadcast(CurrentBatchIndex);
        ++CurrentBatchIndex;
        ScheduleNextBatch();
        return;
    }

    const FBSMobSpawnInfo& Info = Batch.SpawnInfos[CurrentInfoIndex];

    // Poll until clear if required
    if (Info.bRequireClear && !AreAllMobsDead())
    {
        FTimerDelegate Poll;
        Poll.BindUObject(this, &ABSSpawnManager::SpawnNextInfo);
        GetWorldTimerManager().SetTimer(InfoDelayTimer, Poll, 0.5f, false);
        return;
    }

    CurrentMobIndex = 0;
    SpawnNextMob();
}

void ABSSpawnManager::SpawnNextMob()
{
    if (!bSpawning || !ActiveSpawnData) return;

    const FBSMobSpawnInfo& Info = ActiveSpawnData->Batches[CurrentBatchIndex].SpawnInfos[CurrentInfoIndex];
    const int32 TotalCount = Info.Count.AsInteger(1);

    if (CurrentMobIndex >= TotalCount)
    {
        // All mobs for this info spawned — delay then move to next info
        ++CurrentInfoIndex;

        SpawnNextInfo();
        return;
    }

    // Spawn one mob
    TArray<APawn*> PlayerPawns = GetPlayerPawns();

    if (ABSSpawner* Spawner = GetNextSpawner())
    {
        if (ABSMobCharacterBase* Mob = Spawner->SpawnMob(Info.SpawnedMob))
        {
            LiveMobs.Add(Mob);
            ++TotalMobsSpawned;
            if (!PlayerPawns.IsEmpty())
            {
                Mob->GetThreatComponent()->AddThreat(PlayerPawns[CurrentMobIndex % PlayerPawns.Num()], 1);
            }

            Mob->GetOnMobDiedDelegate().AddUObject(this, &ABSSpawnManager::OnMobDied);
        }
    }

    ++CurrentMobIndex;

    if (ActiveSpawnData->GlobalSpawnDelay > 0.f)
    {
        FTimerDelegate Del;
        Del.BindUObject(this, &ABSSpawnManager::SpawnNextMob);
        GetWorldTimerManager().SetTimer(MobDelayTimer, Del, ActiveSpawnData->GlobalSpawnDelay, false);
    }
    else
    {
        SpawnNextMob();
    }
}

// ============================================================
//  Helpers
// ============================================================

bool ABSSpawnManager::IsExpectingMoreMobs() const
{
    return bSpawning || !AreAllMobsDead();
}

void ABSSpawnManager::OnMobDied(ABSMobCharacterBase* Mob)
{
    LiveMobs.Remove(Mob);
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("MOB DIED"));
    
    if (!IsExpectingMoreMobs())
    {
        OnCompletion();
    }
}

bool ABSSpawnManager::AreAllMobsDead() const
{
    for (const TObjectPtr<ABSMobCharacterBase>& Mob : LiveMobs)
    {
        if (IsValid(Mob)) return false;
    }
    return true;
}

TArray<APawn*> ABSSpawnManager::GetPlayerPawns() const
{
    TArray<APawn*> PlayerPawns;

    if (GetWorld() && GetWorld()->GetGameState())
    {
        for (const APlayerState* PS : GetWorld()->GetGameState()->PlayerArray)
        {
            if (APawn* Pawn = PS->GetPawn())
            {
                PlayerPawns.AddUnique(Pawn);
            }
        }
    }

    return PlayerPawns;
}

ABSSpawner* ABSSpawnManager::GetNextSpawner()
{
    if (Spawners.IsEmpty()) return nullptr;

    SpawnerRoundRobin %= Spawners.Num();
    ABSSpawner* S = Spawners[SpawnerRoundRobin++];
    return IsValid(S) ? S : nullptr;
}

