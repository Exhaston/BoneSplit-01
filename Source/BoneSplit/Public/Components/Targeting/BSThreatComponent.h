// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSThreatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBSOnThreatUpdate, AActor*, Actor, float, Threat);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnCombat, bool, InCombat);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSThreatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSThreatComponent();
	
	UPROPERTY()
	FBSOnCombat OnCombatChanged;
	
	UPROPERTY()
	FBSOnThreatUpdate OnThreatUpdate;
	
	UPROPERTY()
	FBSOnThreatUpdate OnMaxThreatChanged;
	
	bool IsInCombat() const;
	
	virtual void AddThreat(AActor* InActor, float Threat);
	
	//Only adds the Actor if it doesn't exist in the threat map
	virtual void AddUniqueThreat(AActor* InActor, float Threat);
	
	virtual void ClearThreat(AActor* InActor);
	
	virtual void ValidateMap();
	
	//May return null
	virtual AActor* GetHighestThreatActor();
	
	//May return null
	virtual AActor* GetLowestThreatActor();
	
	static TTuple<AActor*, AActor*> GetHighestAndLowestFromMap(const TMap<TWeakObjectPtr<AActor>, float>& ThreatMap);
	
	TMap<TWeakObjectPtr<AActor>, float>& GetThreatMap();
	
	TArray<AActor*> GetThreatActors();
	
	UFUNCTION(BlueprintPure)
	AActor* GetHighestThreatTarget() const;
	
private:
	
	virtual void UpdateThreat();
	
	UPROPERTY()
	AActor* HighestThreatActor = nullptr;
	
	UPROPERTY()
	AActor* LowestThreatActor = nullptr;
	
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, float> ThreatMap;
};
