// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MMobPatrolPath.h"
#include "Components/ActorComponent.h"
#include "MMobPatrolComponent.generated.h"


struct FAIRequestID;
struct FPathFollowingResult;
class AAIController;
class AMMobPatrolPath;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MOBS_API UMMobPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UMMobPatrolComponent();
	
	virtual void InitializeComponent() override;
	
	UFUNCTION(BlueprintCallable)
	void StartPatrol(AMMobPatrolPath* InPatrolPath);
	
	UFUNCTION(BlueprintCallable)
	void StopPatrol() const;
	
private:
	
	void MoveToNextWayPoint();
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	
	UPROPERTY()
	TObjectPtr<AMMobPatrolPath> SelectedPatrolPath;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AllowPrivateAccess=true))
	float PatrolPointAcceptanceRadius = 100;
	
	UPROPERTY(Transient)
	TObjectPtr<AAIController> OwnerController;
	
	UPROPERTY(Transient)
	int32 CurrentWayPointIndex = 0;

	UPROPERTY(Transient)
	FMMobWayPoint CurrentWayPoint;

	FTimerHandle WaitTimerHandle;
};
