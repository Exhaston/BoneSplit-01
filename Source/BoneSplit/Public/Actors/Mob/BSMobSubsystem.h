// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSMobSubsystem.generated.h"

class ABSMobSpawner;
class ABSMobCharacter;
class IBSGenericMobInterface;

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnMobSpawn, ABSMobCharacter* MobCharacter);
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSMobSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	static UBSMobSubsystem* Get(const UObject* WorldContext);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual bool IsTickable() const override;
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	virtual TStatId GetStatId() const override;
	
	virtual void DeclareMob(ABSMobCharacter* MobCharacter, bool bAutoAssignTarget = false);
	virtual void UndeclareMob(ABSMobCharacter* MobCharacter);
	
	virtual void RegisterSpawner(ABSMobSpawner* Spawner);
	virtual void UnregisterSpawner(ABSMobSpawner* Spawner);
	
	TArray<ABSMobCharacter*> GetMobCharacters();
	
	FBSOnMobSpawn& GetOnMobSpawnDelegate() { return OnMobSpawnDelegate; }
	
	AActor* GetPlayerFromArrayDeterministic();
	
protected:
	
	float SpawnerTickRate = 2;
	
	UPROPERTY()
	float CurrentSpawnTime = 0;
	
	UPROPERTY()
	int32 CurrentPlayerIt = 0;
	
	FBSOnMobSpawn OnMobSpawnDelegate;
	
	UPROPERTY()
	TArray<ABSMobCharacter*> MobCharacters;
	
	UPROPERTY()
	TArray<ABSMobSpawner*> CurrentMobSpawners;
};
