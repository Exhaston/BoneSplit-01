// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/BSMobSubsystem.h"

#include "Mobs/BSMobCharacterBase.h"

UBSMobSubsystem* UBSMobSubsystem::GetMobSubsystem(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	return World->GetSubsystem<UBSMobSubsystem>();
}

void UBSMobSubsystem::IncreaseDifficulty()
{
	CurrentLevel++;

	for (auto Mob : Mobs)
	{
		if (IsValid(Mob))
		{
			Mob->ReInitCharacterData();
		}
	}
}

void UBSMobSubsystem::RegisterMob(ABSMobCharacterBase* MobCharacterBase)
{
	if (!Mobs.Contains(MobCharacterBase))
	{
		Mobs.Add(MobCharacterBase);
		MobCharacterBase->GetOnMobDiedDelegate().AddUObject(this, &UBSMobSubsystem::OnMobDied);
	}
}

void UBSMobSubsystem::OnMobDied(ABSMobCharacterBase* Mob)
{
	Mobs.Remove(Mob);
	OnMobDiedDelegate.Broadcast(Mob);
}
