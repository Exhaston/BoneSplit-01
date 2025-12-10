// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSLaunchableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UBSLaunchableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BONESPLIT_API IBSLaunchableInterface
{
	GENERATED_BODY()
	
public:
	
	virtual void LaunchActor(FVector Direction, float Magnitude) = 0;
	
};
