// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BSPatrolPath.generated.h"

class ANavigationData;
class USplineComponent;

USTRUCT(BlueprintType)
struct FBSWayStop
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0))
	int32 SplineIndex = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0))
	float WaitTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> Focus;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EventTag;
	
	bool operator==(const FBSWayStop& Other) const
	{
		return SplineIndex == Other.SplineIndex;
	}
};

UCLASS(DisplayName="Patrol Path", Blueprintable, BlueprintType)
class BONESPLIT_API ABSPatrolPath : public AActor
{
	GENERATED_BODY()

public:
	
	explicit ABSPatrolPath(const FObjectInitializer& ObjectInitializer);
	
	TArray<FVector> GetPatrolPoints() const;
	
	int32 GetPatrolPointCount() const;
	
	bool IsClosed() const;

	FVector GetPatrolPointAt(int32 Index) const;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	void DrawDebugPatrolPath() const;
	
	virtual void Destroyed() override;
	
	FBSWayStop GetWayStopAt(int32 Index) const;
	
	const FBSWayStop* GetWayStopAtPatrolIndex(const int32 PatrolIndex) const
	{
		return WayStopByPatrolIndex.Find(PatrolIndex);
	}

protected:
	
	void BuildPatrolPath();
	
	UFUNCTION()
	void OnNavGenComplete(ANavigationData* Data);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Patrol")
	TArray<FBSWayStop> WayStops;
	
	UPROPERTY()
	TArray<FVector> PatrolPoints;
	
	UPROPERTY()
	TMap<int32, FBSWayStop> WayStopByPatrolIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol", meta=(ClampMin="1"))
	int32 SubSteps = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol")
	bool bDrawDebugPoints = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol")
	bool bProjectToNavigation = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol", meta=(ClampMin="0", EditCondition = "bProjectToNavigation"))
	FVector ProjectionExtent = {100, 100, 2500 };
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USplineComponent> PatrolPathSplineComponent;
};
