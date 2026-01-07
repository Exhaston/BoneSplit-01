// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoneSplit/BoneSplit.h"
#include "UObject/Interface.h"
#include "BSMobInterface.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnMoveStateChanged, EBSMoveState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnMoveStateChangedDynamic, EBSMoveState, MoveState);

UINTERFACE()
class UBSMobInterface : public UInterface
{
	GENERATED_BODY()
};

class BONESPLIT_API IBSMobInterface
{
	GENERATED_BODY()

public:
	
	virtual bool TryMoveTo(FVector Location, float AcceptanceRadius = 0) = 0;
	
	virtual bool TryMoveTo(AActor* Target, bool bTrack = true, float AcceptanceRadius = 0) = 0;
};
