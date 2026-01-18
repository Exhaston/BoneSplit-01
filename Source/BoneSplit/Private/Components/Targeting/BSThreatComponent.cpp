// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Targeting/BSThreatComponent.h"


UBSThreatComponent::UBSThreatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UBSThreatComponent::IsInCombat() const
{
	return !ThreatMap.IsEmpty();
}

void UBSThreatComponent::AddThreat(AActor* InActor, const float Threat)
{
	const bool bWasInCombat = IsInCombat();
	
	float& Current = ThreatMap.FindOrAdd(InActor);
	Current = FMath::Max(Current + Threat, 0);
	
	OnThreatUpdate.Broadcast(InActor, Current);
	
	if (FMath::IsNearlyZero(Current))
	{
		ThreatMap.Remove(InActor);
	}
	
	UpdateThreat();
	
	if (bWasInCombat != IsInCombat())
	{
		OnCombatChanged.Broadcast(IsInCombat());
	}
}

void UBSThreatComponent::AddUniqueThreat(AActor* InActor, const float Threat)
{
	if (!ThreatMap.Contains(InActor))
	{
		AddThreat(InActor, Threat);
	}
}

void UBSThreatComponent::ClearThreat(AActor* InActor)
{
	ThreatMap.Remove(InActor);
	
	UpdateThreat();
}

void UBSThreatComponent::ValidateMap()
{
	for (auto It = ThreatMap.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid()) It.RemoveCurrent(); 
	}
}

AActor* UBSThreatComponent::GetHighestThreatActor()
{
	return HighestThreatActor;
}

AActor* UBSThreatComponent::GetLowestThreatActor()
{
	return LowestThreatActor;
}

TTuple<AActor*, AActor*> UBSThreatComponent::GetHighestAndLowestFromMap(const TMap<TWeakObjectPtr<AActor>, float>& ThreatMap)
{
	if (ThreatMap.IsEmpty())
	{
		return { nullptr, nullptr };
	}
	
	AActor* HighestActor = nullptr;
	AActor* LowestActor = nullptr;
	
	float HighestThreat = TNumericLimits<float>::Lowest();
	float LowestThreat  = TNumericLimits<float>::Max();
	
	for (const auto& Pair : ThreatMap)
	{
		if (!Pair.Key.IsValid()) continue;
		if (Pair.Value > HighestThreat)
		{
			HighestThreat = Pair.Value;
			HighestActor = Pair.Key.Get();
		}
		if (Pair.Value < LowestThreat)
		{
			LowestThreat = Pair.Value;
			LowestActor = Pair.Key.Get();
		}
	}

	return {HighestActor, LowestActor};
}

TMap<TWeakObjectPtr<AActor>, float>& UBSThreatComponent::GetThreatMap()
{
	return ThreatMap;
}

TArray<AActor*> UBSThreatComponent::GetThreatActors()
{
	ValidateMap();
	TArray<TWeakObjectPtr<AActor>> WeakThreatArray;
	ThreatMap.GetKeys(WeakThreatArray);
	TArray<AActor*> ThreatArray;
	for (auto Threat : WeakThreatArray)
	{
		ThreatArray.Add(Threat.Get());
	}
	return ThreatArray;
}

AActor* UBSThreatComponent::GetHighestThreatTarget() const
{
	return HighestThreatActor;
}

void UBSThreatComponent::UpdateThreat()
{
	ValidateMap();
	const TTuple<AActor*, AActor*> NewHighestAndLowest = GetHighestAndLowestFromMap(ThreatMap);
	
	if (NewHighestAndLowest.Key != HighestThreatActor)
	{
		HighestThreatActor = NewHighestAndLowest.Key;
		
		float CurrentThreat = 0; //if nullptr broadcast 0 threat
		if (HighestThreatActor)
		{
			CurrentThreat = ThreatMap[HighestThreatActor];
		}
		OnMaxThreatChanged.Broadcast(HighestThreatActor, CurrentThreat);
	}
	
	if (LowestThreatActor != NewHighestAndLowest.Value)
	{
		LowestThreatActor = NewHighestAndLowest.Value;
	}
}
