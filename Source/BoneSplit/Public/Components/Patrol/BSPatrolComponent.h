// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "BSPatrolComponent.generated.h"


class ABSMobController;
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
    explicit UBSPatrolComponent(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
    
    UFUNCTION()
    void OnPawnControllerSet(APawn* OwnerPawn, AController* OldController, AController* NewController);
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Patrol")
    void StartPatrol();

    UFUNCTION(BlueprintCallable, Category="Patrol")
    void StopPatrol();

protected:
    void MoveToCurrentPoint();
    void MoveToFollowActor();
    void AdvancePatrolIndex();
    
    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

    // --- Config ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol")
    TObjectPtr<ABSPatrolPath> PatrolPathActor;

    // If set, the AI will shadow this actor instead of walking a patrol path
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol")
    TObjectPtr<AActor> FollowActor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol")
    bool bStartReversed = false;

    // Minimum distance the follow target must move before re-issuing a move request
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol", meta=(ClampMin="1.0"))
    float FollowMoveThreshold = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Patrol", meta=(ClampMin="1.0"))
    float AcceptanceRadius = 50.f;

    // --- Runtime state ---

    UPROPERTY(Transient)
    int32 CurrentPointIndex = 0;

    UPROPERTY(Transient)
    int32 Direction = 1;

    UPROPERTY(Transient)
    bool bPatrolActive = false;

    UPROPERTY(Transient)
    bool bWaiting = false;

    UPROPERTY(Transient)
    float WaitTimeRemaining = 0.f;

    // Last location we issued a follow-move to, used for threshold check
    FVector LastIssuedFollowLocation = FVector::ZeroVector;

    FVector FollowActorOffset = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Transient)
    TObjectPtr<ABSMobController> MobController;
};
