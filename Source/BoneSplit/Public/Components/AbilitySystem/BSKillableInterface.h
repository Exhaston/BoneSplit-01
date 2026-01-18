// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSKillableInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBSOnKilled, AActor*, KillerActor, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBSOnRevived, AActor*, Reviver, float, Health);

UINTERFACE()
class UBSKillableInterface : public UInterface
{
	GENERATED_BODY()
};

class BONESPLIT_API IBSKillableInterface
{
	GENERATED_BODY()
	
public:
	
	virtual void OnKilled(AActor* Killer, float Damage) = 0;
	virtual bool CanBeKilled() const = 0;
};
