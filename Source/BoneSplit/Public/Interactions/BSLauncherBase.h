// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSLauncherBase.generated.h"

class USplineComponent;
class UCapsuleComponent;

/*
 * Add an overlap component and adjust target location transform to the desired position. 
 * The curve will approximate the trajectory to target location. Slight deviations might occur. 
 * See ArcDistribution to adjust trajectory.
 */
UCLASS(DisplayName="Launcher Base", Category="BoneSplit")
class BONESPLIT_API ABSLauncherBase : public AActor
{
	GENERATED_BODY()

public:
	explicit ABSLauncherBase(const FObjectInitializer& ObjectInitializer);
	
#if WITH_EDITOR
	
	virtual bool ShouldTickIfViewportsOnly() const override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	void ShowApproximatePath() const;
	
#endif
	
#if WITH_EDITORONLY_DATA

	UPROPERTY()
	UCapsuleComponent* TargetVisualizer;
	
	UPROPERTY()
	USplineComponent* TrajectorySpline;

#endif
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> TargetLocator;
	
	//Adjust the Arc to match the desired launch trajectory. Note: Some arcs might be less accurate due to air drag.
	//0.5 provides the most accurate result to the preview. Other values might add overshoot.
	//Clamped to 0.1 - 0.9 because of arc calculation issues in specific edge cases.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0.1, ClampMax=0.9), Category="Launch")
	float ArcDistribution = 0.5;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	bool ComputeLaunchVelocity(const AActor* Target, FVector& OutLaunchVelocity) const;
};
