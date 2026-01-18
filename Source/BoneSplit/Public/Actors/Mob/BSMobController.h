// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "BSMobController.generated.h"

class UBSTargetSetting;

UCLASS()
class BONESPLIT_API ABSMobController : public ADetourCrowdAIController
{
	GENERATED_BODY()

public:
	
	explicit ABSMobController(const FObjectInitializer& ObjectInitializer);
	
};
