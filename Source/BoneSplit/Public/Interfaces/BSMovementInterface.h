// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSMovementInterface.generated.h"

enum class EBSMovementRotationMode : uint8;

UINTERFACE(BlueprintType, BlueprintType)
class UBSMovementInterface : public UInterface
{
	GENERATED_BODY()
};


class BONESPLIT_API IBSMovementInterface
{
	GENERATED_BODY()
	
public:
	
	virtual void LaunchActor(FVector Direction, float Magnitude) = 0;
	
	virtual void SetMovementRotationMode(uint8 NewMovementMode) = 0;
	
};
