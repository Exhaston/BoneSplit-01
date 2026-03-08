// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BSMobController.generated.h"

class UBSTargetSetting;

UCLASS()
class BONESPLIT_API ABSMobController : public AAIController
{
	GENERATED_BODY()

public:
	
	explicit ABSMobController(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void OnUnPossess() override;

	void StartChasing();
	void StopChasing();
	
	bool IsOnValidNavMesh();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
	float AcceptanceRadius = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
	float RestartDistance = 600.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
	float CheckInterval = 0.25f;

private:
	UPROPERTY()
	FTimerHandle ChaseTimerHandle;

	void ChasePlayer();
	
	UFUNCTION()
	virtual void CustomOnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
