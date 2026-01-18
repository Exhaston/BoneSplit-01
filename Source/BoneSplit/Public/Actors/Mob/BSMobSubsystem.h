// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSMobSubsystem.generated.h"

class IBSGenericMobInterface;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSMobSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	virtual bool IsTickable() const override;
	
	virtual TStatId GetStatId() const override;
	
	//Registers the mob for combat tracking and ticking. Call Unregister when the mob dies or gets destroyed
	virtual void RegisterMob(TScriptInterface<IBSGenericMobInterface> MobInterface);
	
	virtual void UnRegisterMob(TScriptInterface<IBSGenericMobInterface> MobInterface);
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY()
	float MobTickRate = 1;
	
	UPROPERTY()
	float CurrentTickTime = 0;
	
	UPROPERTY()
	int32 MaxAttackers = 3;

private:
	
	virtual void TickMobs(float DeltaTime);
	
	virtual void TickCombat(float DeltaTime);
	
	virtual void AddMobToCombat(TScriptInterface<IBSGenericMobInterface> MobInterface);
	
	UPROPERTY()
	TArray<TScriptInterface<IBSGenericMobInterface>> Mobs;
};
