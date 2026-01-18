// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSMobSpawner.generated.h"

class ABSMobCharacter;
class UBSMobSpawnBatch;

USTRUCT(BlueprintType)
struct FBSSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABSMobCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0))
	float SpawnDelay = 0.5;

};

USTRUCT(BlueprintType)
struct FBSWaveSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(TitleProperty=EnemyClass))
	TArray<FBSSpawnInfo> SpawnInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0))
	float TimeBeforeNextWave = 5;
};

UCLASS(BlueprintType, DisplayName="Wave Data")
class UBSWaveDataAsset : public UDataAsset
{                                         
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FBSWaveSpawnInfo> WaveInfo;
};

UCLASS()
class BONESPLIT_API ABSMobSpawner : public AActor
{
	GENERATED_BODY()

public:
	
	explicit ABSMobSpawner(const FObjectInitializer& ObjectInitializer);
};

UENUM(BlueprintType)
enum class EBSWaveState : uint8
{
	Waiting,
	Spawning,
	InProgress,
	Completed
};

DECLARE_MULTICAST_DELEGATE(FBSOnMobManagerDelegate);
UCLASS()
class UBSMobManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	UBSMobManager();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void StartWaves(UBSWaveDataAsset* InWaveAsset, TArray<ABSMobSpawner*> Spawners);
	
	UFUNCTION(BlueprintPure)
	EBSWaveState GetWaveState() const;
	
	FBSOnMobManagerDelegate OnCombatCompleted;
	
	FBSOnMobManagerDelegate OnCombatBegun;
	
	FBSOnMobManagerDelegate OnWaveClear;
	
	virtual void TickCombat();
	
	virtual bool TryClaimToken(AActor* InMob);
	
	virtual bool HasToken(AActor* InMob);
	
	virtual bool ReleaseToken(AActor* InMob);
	
private:
	
	UPROPERTY()
	TArray<ABSMobCharacter*> Mobs;
	
	UPROPERTY()
	TArray<AActor*> AttackTokens;
	
	UPROPERTY(EditAnywhere)
	UBSWaveDataAsset* WaveAsset;

	UPROPERTY()
	TArray<ABSMobSpawner*> SpawnPoints;
	
	UPROPERTY()
	TArray<FTimerHandle> TimerHandles;

	UPROPERTY()
	int32 CurrentWaveIndex = 0;
	
	UPROPERTY()
	int32 RemainingEnemies = 0;

	EBSWaveState WaveState = EBSWaveState::Waiting;

	void StartWave();
	void SpawnEnemy(const FBSSpawnInfo& Info);
	
	UFUNCTION()
	void HandleEnemyKilled(AActor* Killer, float Damage);
	
	UFUNCTION()
	void StartNextWave();
	
};
