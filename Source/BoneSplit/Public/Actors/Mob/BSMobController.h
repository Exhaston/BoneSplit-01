// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "BSMobController.generated.h"

UCLASS()
class BONESPLIT_API ABSMobController : public ADetourCrowdAIController
{
	GENERATED_BODY()

public:
	
	explicit ABSMobController(const FObjectInitializer& ObjectInitializer);
	
protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;
};
