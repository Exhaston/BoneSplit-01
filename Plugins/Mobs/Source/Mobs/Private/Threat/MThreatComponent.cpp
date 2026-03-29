// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Threat/MThreatComponent.h"

UMThreatComponent::UMThreatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.TickInterval = 2;
}

void UMThreatComponent::AddThreat(AActor* InActor, const float ThreatValue)
{
	//During taunts only the taunter can generate more threat.
	if (LockThreatActor.IsValid() && LockThreatActor != InActor) return; 
	
	float& CurrentThreat = ActorThreatMap.FindOrAdd(InActor);
	CurrentThreat = FMath::Clamp(CurrentThreat + ThreatValue, 0.f, FLT_MAX);
	
	TryUpdateTarget();
}

void UMThreatComponent::Taunt(AActor* InActor, const float Duration)
{
	LockThreatActor = InActor;
	
	LevelAllThreat();
	
	AddThreat(InActor, 1);
	
	GetOwner()->GetWorld()->GetTimerManager().SetTimer(TauntTimer, [this]()
	{
		LockThreatActor = nullptr;
	}, Duration, false);
}

void UMThreatComponent::ClearThreatFor(AActor* InActor)
{
	ActorThreatMap.Remove(InActor);
	
	TryUpdateTarget();
}

void UMThreatComponent::ClearAllThreat()
{
	ActorThreatMap.Empty();
	
	TryUpdateTarget();
}

void UMThreatComponent::TryUpdateTarget()
{
	AActor* OldHighest = GetHighestThreat(); //Get current highest threat
	
	HighestThreatActor = EvaluateHighestThreat(); //Re evaluate highest
	
	if (OldHighest != HighestThreatActor) //If state has changed we broadcast change.
	{
		OnTargetChanged.Broadcast(OldHighest, GetHighestThreat());
	}
}

AActor* UMThreatComponent::GetHighestThreat()
{
	return HighestThreatActor.IsValid() ? HighestThreatActor.Get() : nullptr;
}

TArray<AActor*> UMThreatComponent::GetAllThreateningActors()
{
	TArray<AActor*> AllThreateningActors;
	for (auto& ThreatPair : ActorThreatMap)
	{
		if (ThreatPair.Key.IsValid())
		{
			AllThreateningActors.Add(ThreatPair.Key.Get());
		}
	}
	return AllThreateningActors;
}

AActor* UMThreatComponent::EvaluateHighestThreat()
{
	AActor* HighestThreat = nullptr;
	float HighestThreatValue = 0.f;

	for (auto& ThreatPair : ActorThreatMap)
	{
		if (!ThreatPair.Key.IsValid()) continue;
		if (ThreatPair.Value > HighestThreatValue)
		{
			HighestThreatValue = ThreatPair.Value;
			HighestThreat = ThreatPair.Key.Get();
		}
	}
	
	return HighestThreat;
}

void UMThreatComponent::LevelAllThreat()
{
	for (auto& ThreatPair : ActorThreatMap)
	{
		ThreatPair.Value = 0;
	}
}

void UMThreatComponent::CleanThreatMap()
{
	for (auto It = ActorThreatMap.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid()) It.RemoveCurrent();
		if (It.Value() <= 0) It.RemoveCurrent();
	}
}

