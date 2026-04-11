// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSMobCharacterBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSMobSubsystem.generated.h"

class ABSMobCharacterBase;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSMobSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	static UBSMobSubsystem* GetMobSubsystem(const UObject* WorldContext);
	
	virtual void IncreaseDifficulty();
	
	virtual void RegisterMob(ABSMobCharacterBase* MobCharacterBase);
	
	virtual int32 GetCurrentDifficulty() { return CurrentLevel; }
	
	FBSOnMobDied GetOnMobDiedDelegate() { return OnMobDiedDelegate; }
	
protected:
	
	FBSOnMobDied OnMobDiedDelegate;
	
	virtual void OnMobDied(ABSMobCharacterBase* Mob);
	
	UPROPERTY()
	TArray<TObjectPtr<ABSMobCharacterBase>> Mobs;
	
	int32 CurrentLevel = 1;
};
