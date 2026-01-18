// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobSubsystem.h"

#include "AbilitySystemInterface.h"
#include "Actors/Mob/BSGenericMobInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "Components/Targeting/BSThreatInterface.h"

bool UBSMobSubsystem::IsTickable() const
{
	return true;
}

TStatId UBSMobSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBSMobSubsystem, STATGROUP_Tickables);
}

void UBSMobSubsystem::RegisterMob(const TScriptInterface<IBSGenericMobInterface> MobInterface)
{
	if (IsValid(MobInterface.GetObject()))
	{
		Mobs.AddUnique(MobInterface);
	}
}

void UBSMobSubsystem::UnRegisterMob(const TScriptInterface<IBSGenericMobInterface> MobInterface)
{
	Mobs.Remove(MobInterface);
}

void UBSMobSubsystem::TickMobs(const float DeltaTime)
{
	Mobs.RemoveAll([](const TScriptInterface<IBSGenericMobInterface>& Mob)
	{
		return !IsValid(Mob.GetObject());
	});
	
	TickCombat(DeltaTime);
}

void UBSMobSubsystem::TickCombat(const float DeltaTime)
{
	for (auto& Mob : Mobs)
	{
		if (IBSGenericMobInterface::Execute_BP_IsInCombat(Mob.GetObject()))
		{
			Mob.GetInterface()->NativeOnCombatTick(true, DeltaTime);
			IBSGenericMobInterface::Execute_BP_OnCombatTick(Mob.GetObject(), true, DeltaTime);
			continue;
		}
		
		AActor* PassiveMobActor = Cast<AActor>(Mob.GetObject());
		check(PassiveMobActor);
		
		const float AggroRange = IBSGenericMobInterface::Execute_BP_GetAggroRange(Mob.GetObject());
		if (AggroRange <= 0) continue;
		
		const IAbilitySystemInterface* MobAscInterface = Cast<IAbilitySystemInterface>(PassiveMobActor);
		check(MobAscInterface);
		
		IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(PassiveMobActor);
		check(ThreatInterface);
		
		TArray<AActor*> OverlappedActors = UBSAbilityFunctionLibrary::GetActorsInRadius(
		PassiveMobActor, AggroRange, false);
	
#if WITH_EDITOR
	
		if (BSConsoleVariables::CVarBSDebugAggroSpheres.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorld(), 
				PassiveMobActor->GetActorLocation(), 
				AggroRange, 
				16,
				FColor::Yellow, 
				false, 
				DeltaTime);
		}
	
#endif
	
		for (const auto FoundActor : OverlappedActors)
		{
			if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(FoundActor))
			{
				if (UAbilitySystemComponent* OtherAsc = AscInterface->GetAbilitySystemComponent(); OtherAsc && 
					!UBSAbilityFunctionLibrary::FilterByNoMatchingFactions(MobAscInterface->GetAbilitySystemComponent(), OtherAsc)) 
					continue;
			
				if (!ThreatInterface->GetThreatComponent()->GetThreatMap().Contains(FoundActor))
				{
					if (UBSAbilityFunctionLibrary::CheckTargetVisibility(
						this, PassiveMobActor->GetActorLocation(), FoundActor))
					{
						ThreatInterface->GetThreatComponent()->AddThreat(FoundActor, 1);
						AddMobToCombat(Mob);
					}
				}
			}
		}
	}
}

void UBSMobSubsystem::AddMobToCombat(const TScriptInterface<IBSGenericMobInterface> MobInterface)
{
	MobInterface.GetInterface()->NativeOnCombatBegin();
	IBSGenericMobInterface::Execute_BP_OnCombatBegin(MobInterface.GetObject());
}

void UBSMobSubsystem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!GetWorld()) return;
	
	if (GetWorld()->IsNetMode(NM_Client)) return;
	
	CurrentTickTime += DeltaTime;

	while (CurrentTickTime >= MobTickRate)
	{
		TickMobs(MobTickRate);
		CurrentTickTime -= MobTickRate;
	}
}
