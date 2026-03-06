// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSPatrolComponent.generated.h"


struct FPathFollowingResult;
struct FAIRequestID;

namespace EPathFollowingResult
{
	enum Type : int;
}

class AAIController;
class ABSPatrolPath;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSPatrolComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UBSPatrolComponent();

	virtual void StartPatrol();
	virtual void StopPatrol();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void BeginPlay() override;
	virtual void MoveToFollowActor();
	void MoveToCurrentPoint();
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	void AdvancePatrolIndex();

	AAIController* GetOwnerController();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Patrol")
	bool bStartReversed = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Patrol")
	TObjectPtr<ABSPatrolPath> PatrolPathActor;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Patrol")
	TObjectPtr<AActor> FollowActor;
	
	UPROPERTY()
	FVector FollowActorOffset;
	
	UPROPERTY()
	TWeakObjectPtr<AAIController> CachedOwnerController;

private:

	int32  CurrentPointIndex  = 0;
	int32  Direction          = 1;       // 1 = forward, -1 = reverse
	bool   bPatrolActive      = false;
	bool   bWaiting           = false;
	float  WaitTimeRemaining  = 0.f;
};
