// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MThreatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FMOnTargetChanged, AActor* OldTarget, AActor* NewTarget);

UCLASS(BlueprintType, Blueprintable)
class MOBS_API UMThreatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UMThreatComponent();
	
	static UMThreatComponent* GetThreatComponent(const AActor* Target)
	{
		return Target->GetComponentByClass<UMThreatComponent>();
	}
	
	virtual void AddThreat(AActor* InActor, float ThreatValue);
	
	//Resets all threat and keeps the taunt target for the duration
	virtual void Taunt(AActor* InActor, float Duration = 1);
	
	virtual void ClearThreatFor(AActor* InActor);
	
	virtual void ClearAllThreat();
	
	//Compares old to new target, even if nullptr and broadcasts changes if needed.
	virtual void TryUpdateTarget();
	
	virtual bool HasThreatForTarget(AActor* InActor) { return ActorThreatMap.Contains(InActor); }
	
	FMOnTargetChanged& GetOnTargetChangedDelegate() { return OnTargetChanged; }
	
	AActor* GetHighestThreat();
	
	TArray<AActor*> GetAllThreateningActors();
	
	AActor* EvaluateHighestThreat();
	
	virtual void LevelAllThreat();
	
protected:
	
	UPROPERTY()
	FTimerHandle TauntTimer;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> LockThreatActor = nullptr;
	
	FMOnTargetChanged OnTargetChanged;
	
	virtual void CleanThreatMap();
	
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> HighestThreatActor = nullptr;
	
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, float> ActorThreatMap;
};
