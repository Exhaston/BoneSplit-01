// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobSpawner.h"

#include "Actors/Mob/BSMobCharacter.h"


ABSMobSpawner::ABSMobSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
}

void ABSMobSpawner::TickMobSpawner()
{
}

UBSMobManager::UBSMobManager()
{
}

void UBSMobManager::StartWaves(UBSWaveDataAsset* InWaveAsset, const TArray<ABSMobSpawner*> Spawners)
{
	WaveAsset = InWaveAsset;
	SpawnPoints = Spawners;
	CurrentWaveIndex = 0;
	StartWave();
}

EBSWaveState UBSMobManager::GetWaveState() const
{
	return WaveState;
}

void UBSMobManager::TickCombat()
{
}

bool UBSMobManager::TryClaimToken(AActor* InMob)
{
	if (AttackTokens.Contains(InMob))
	{
		return true; 
	}
	
	//TODO apply difficulty scaling to a max attackers variable.
	if (AttackTokens.Num() < 3)
	{
		AttackTokens.Add(InMob);
		return true;
	}
	
	return false;

}

bool UBSMobManager::HasToken(AActor* InMob)
{
	return AttackTokens.Contains(InMob);
}

bool UBSMobManager::ReleaseToken(AActor* InMob)
{
	if (AttackTokens.Contains(InMob))
	{
		AttackTokens.Remove(InMob);
		return true;
	}
	return false;
}

void UBSMobManager::StartWave()
{
	if (!WaveAsset->WaveInfo.IsValidIndex(CurrentWaveIndex))
	{
		OnCombatCompleted.Broadcast();
		WaveState = EBSWaveState::Completed;
		return;
	}
	
	TimerHandles.Empty();
	
	OnCombatBegun.Broadcast();
	
	WaveState = EBSWaveState::Spawning;
	const FBSWaveSpawnInfo& Wave = WaveAsset->WaveInfo[CurrentWaveIndex];

	for (const auto EnemyInfo : Wave.SpawnInfo)
	{
		RemainingEnemies += EnemyInfo.Count;
	}

	for (const FBSSpawnInfo& Info :  Wave.SpawnInfo)
	{
		for (int32 i = 0; i < Info.Count; i++)                    
		{
			FTimerHandle TimerHandle;                 
			TimerHandles.Add(TimerHandle); //Keep in scope
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this, Info]()
				{                                        
					SpawnEnemy(Info);
				},
				Info.SpawnDelay * i + 1, //Start at 1, timers of 0 won't trigger
				false
			);
		}
	}
}

void UBSMobManager::SpawnEnemy(const FBSSpawnInfo& Info)
{
	if (SpawnPoints.Num() == 0 || !Info.EnemyClass)
		return;
	
	WaveState = EBSWaveState::InProgress;

	const AActor* SpawnPoint = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

	ABSMobCharacter* Enemy = GetWorld()->SpawnActorDeferred<ABSMobCharacter>(
		Info.EnemyClass,
		SpawnPoint->GetTransform()
	);

	if (Enemy)
	{
		Enemy->OnDeathDelegate.AddDynamic(
			this,
			&UBSMobManager::HandleEnemyKilled
		);
		Enemy->FinishSpawning(SpawnPoint->GetTransform());
	}
}

void UBSMobManager::HandleEnemyKilled(UAbilitySystemComponent* SourceAsc, UAbilitySystemComponent* Target, float Damage)
{
	RemainingEnemies--;

	if (RemainingEnemies <= 0)
	{
		OnWaveClear.Broadcast();
		const float Delay = WaveAsset->WaveInfo[CurrentWaveIndex].TimeBeforeNextWave;

		FTimerHandle NextWaveHandle;
		GetWorld()->GetTimerManager().SetTimer(
			NextWaveHandle,
			this,
			&UBSMobManager::StartNextWave,
			Delay,
			false
		);
	}
}

void UBSMobManager::StartNextWave()
{
	CurrentWaveIndex++;
	StartWave();
}

