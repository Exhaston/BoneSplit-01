// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMobPatrolPath.generated.h"                                

USTRUCT(BlueprintType, DisplayName="MobWayPoint")
struct FMMobWayPoint
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFloatRange WaitTimeRange = FFloatRange(0.f,0.f);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString EventTag = "";
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(MakeEditWidget))
	FTransform WayPointTransform = FTransform::Identity;
};

UCLASS(DisplayName="Mob Patrol Path")
class MOBS_API AMMobPatrolPath : public AActor
{
	GENERATED_BODY()                     

public:
	
	explicit AMMobPatrolPath(const FObjectInitializer& ObjectInitializer);
	
	bool GetHasWayPoints() { return bUseWayPoints && !WayPoints.IsEmpty(); }
	
	virtual void GetNextWayPoint(FMMobWayPoint& OutWayPoint, int32 CurrentPointIndex, int32& OutNextIndex);

protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bUseWayPoints = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition="!bUseWayPoints", EditConditionHides))
	float RandomPatrolRadius = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition="!bUseWayPoints", EditConditionHides))
	FFloatRange RandomPatrolWaitTimeRange = FFloatRange(0.f,10.f);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition="bUseWayPoints", EditConditionHides))
	bool bRandomOrder = false;
	
	//When the end is reached, will switch the direction backwards.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition="!bRandomOrder", EditConditionHides))
	bool bPingPong = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition="bUseWayPoints", EditConditionHides))
	TArray<FMMobWayPoint> WayPoints;
	
	virtual void BeginPlay() override;
};
