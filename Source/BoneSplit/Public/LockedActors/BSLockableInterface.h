// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSLockableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UBSLockableInterface : public UInterface
{
	GENERATED_BODY()
};

class BONESPLIT_API IBSLockableInterface
{
	GENERATED_BODY()

public:
	
	virtual void SetLockState(bool bLocked) = 0;
	virtual bool GetIsLocked() = 0;
};
