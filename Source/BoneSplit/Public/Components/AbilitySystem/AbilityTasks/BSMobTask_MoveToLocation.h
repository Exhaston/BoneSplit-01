// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "BSMobTask_MoveToLocation.generated.h"

class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMoveTaskCompletedSignature, TEnumAsByte<EPathFollowingResult::Type>, Result, AAIController*, AIController);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMoveTaskFinishedSignature, TEnumAsByte<EPathFollowingResult::Type>, AAIController*);

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSMobTask_MoveToLocation : public UAITask
{
	GENERATED_BODY()
	
public:
	
	explicit UBSMobTask_MoveToLocation(const FObjectInitializer& ObjectInitializer);

	/** tries to start move request and handles retry timer */
	void ConditionalPerformMove();

	/** prepare move task for activation */
	void SetUp(AAIController* Controller, const FAIMoveRequest& InMoveRequest);

	EPathFollowingResult::Type GetMoveResult() const { return MoveResult; }
	bool WasMoveSuccessful() const { return MoveResult == EPathFollowingResult::Success; }
	bool WasMovePartial() const { return Path.IsValid() && Path->IsPartial(); }

	/**
	 * Move to a location or an actor
	 * @param AvatarActor
	 * @param Controller
	 * @param InGoalLocation
	 * @param InGoalActor
	 * @param AcceptanceRadius
	 * @param AcceptPartialPath
	 * @param bUsePathfinding
	 * @param bUseContinuousGoalTracking Will make the target stick to the target and never finish moving unless failed.
	 * @param ProjectGoalOnNavigation Try to move the goal to the navigation surface before requesting the move, fails if it can't.
	 * @param RequireNavigableEndLocation Set to No to allow pursuing the request even if no navigation surface is found at the goal location. 
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (AdvancedDisplay = "InGoalLocation, AcceptPartialPath,bUsePathfinding,bUseContinuousGoalTracking,ProjectGoalOnNavigation,RequireNavigableEndLocation", BlueprintInternalUseOnly = "TRUE", DisplayName = "Mob Move Task"))
	static UBSMobTask_MoveToLocation* MobMoveTo(AActor* AvatarActor, FVector InGoalLocation, AActor* InGoalActor = nullptr,
		float AcceptanceRadius = 100, EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Enable,
		bool bUsePathfinding = true, bool bUseContinuousGoalTracking = false, EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Enable,
		EAIOptionFlag::Type RequireNavigableEndLocation = EAIOptionFlag::Default);

	/** Allows custom move request tweaking. Note that all MoveRequest need to 
	 *	be performed before PerformMove is called. */
	FAIMoveRequest& GetMoveRequestRef() { return MoveRequest; }

	/** Switch task into continuous tracking mode: keep restarting move toward goal actor. Only pathfinding failure or external cancel will be able to stop this task. */
	void SetContinuousGoalTracking(bool bEnable);
	
	FMoveTaskFinishedSignature OnMoveTaskFinished;
protected:
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnRequestFailed;

	UPROPERTY(BlueprintAssignable)
	FMoveTaskCompletedSignature OnMoveFinished;

	/** parameters of move request */
	UPROPERTY()
	FAIMoveRequest MoveRequest;

	/** handle of path following's OnMoveFinished delegate */
	FDelegateHandle PathFinishDelegateHandle;

	/** handle of path's update event delegate */
	FDelegateHandle PathUpdateDelegateHandle;

	/** handle of active ConditionalPerformMove timer  */
	FTimerHandle MoveRetryTimerHandle;

	/** handle of active ConditionalUpdatePath timer */
	FTimerHandle PathRetryTimerHandle;

	/** request ID of path following's request */
	FAIRequestID MoveRequestID;

	/** currently followed path */
	FNavPathSharedPtr Path;

	TEnumAsByte<EPathFollowingResult::Type> MoveResult;
	uint8 bUseContinuousTracking : 1;

	virtual void Activate() override;
	virtual void OnDestroy(bool bOwnerFinished) override;

	virtual void Pause() override;
	virtual void Resume() override;

	/** finish task */
	void FinishMoveTask(EPathFollowingResult::Type InResult);

	/** stores path and starts observing its events */
	void SetObservedPath(const FNavPathSharedPtr& InPath);

	/** remove all delegates */
	virtual void ResetObservers();

	/** remove all timers */
	virtual void ResetTimers();

	/** tries to update invalidated path and handles retry timer */
	void ConditionalUpdatePath();

	/** start move request */
	virtual void PerformMove();

	/** event from followed path */
	virtual void OnPathEvent(FNavigationPath* InPath, ENavPathEvent::Type Event);

	/** event from path following */
	virtual void OnRequestFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
