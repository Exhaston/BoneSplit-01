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
    float WaitTime = 1.f;

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

    // Total number of virtual patrol points across all spline segments
    int32 GetPatrolPointCount() const;

    // Project and return the world position for a given patrol index on-demand
    FVector GetPatrolPointAt(int32 PatrolIndex) const;

    // Returns true and fills OutWayStop if a WayStop is associated with this patrol index
    bool TryGetWayStopAtPatrolIndex(int32 PatrolIndex, FBSWayStop& OutWayStop) const;

    bool IsClosed() const;


protected:

    // Projects a world-space point onto the navmesh, returns original point if projection fails
    FVector ProjectToNav(const FVector& Point) const;

    // Converts a patrol index into a spline input key (e.g. 3.5 = segment 3, halfway)
    float PatrolIndexToSplineKey(int32 PatrolIndex) const;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Patrol")
    TArray<FBSWayStop> WayStops;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol", meta=(ClampMin="1"))
    int32 SubSteps = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol")
    bool bProjectToNavigation = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol",
        meta=(ClampMin="0", EditCondition="bProjectToNavigation"))
    FVector ProjectionExtent = { 100.f, 100.f, 2500.f };

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    TObjectPtr<USplineComponent> PatrolPathSplineComponent;
};