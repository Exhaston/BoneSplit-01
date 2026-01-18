// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSWaveStarter.h"

#include "Actors/InteractableBases/BSLockableActor.h"
#include "Actors/Mob/BSMobSpawner.h"

ABSWaveStarter::ABSWaveStarter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(5);
}

void ABSWaveStarter::StartCombat() const
{
	if (!HasAuthority()) return;
	if (!Spawners.IsEmpty())
	{
		if (UBSMobManager* MobManager = GetWorld()->GetSubsystem<UBSMobManager>(); 
			MobManager->GetWaveState() == EBSWaveState::Waiting)
		{
			for (const auto LockableActor : LockedActors)
			{
				LockableActor->NativeOnLocked();
			}
			
			MobManager->OnCombatCompleted.AddWeakLambda(this, [this]()
			{
				for (const auto LockableActor : LockedActors)
				{
					LockableActor->NativeOnUnLocked();
				}
			});
			MobManager->StartWaves(SpawnBatch, Spawners);
		}
	}
}

void ABSWaveStarter::ForceStopCombat()
{
}
