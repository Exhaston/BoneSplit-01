// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSLauncherBase.generated.h"

class USplineComponent;
class UCapsuleComponent;

UCLASS()
class BONESPLIT_API ABSLauncherBase : public AActor
{
	GENERATED_BODY()

public:
	explicit ABSLauncherBase(const FObjectInitializer& ObjectInitializer);
	
#if WITH_EDITOR
	
	virtual bool ShouldTickIfViewportsOnly() const override;
	
	virtual void PostEditMove(bool bFinished) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
#endif
	
#if WITH_EDITORONLY_DATA
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float LaunchCurveLength = 2;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UCapsuleComponent* TargetVisualizer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* TrajectorySpline;

#endif
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(CallInEditor)
	void ShowApproximatePath() const;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<USceneComponent> TargetLocator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0.1, ClampMax=0.9))
	float ArcDistribution = 0.5;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	bool ComputeLaunchVelocity(const AActor* Target, FVector& OutLaunchVelocity) const;
};
