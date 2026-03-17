// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UBSInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BONESPLIT_API IBSInteractionInterface
{
	GENERATED_BODY()

public:
	
	virtual void SetInteractionEnabled(bool IsInteractable = true) = 0;
	
	virtual FGuid GetInteractionGuid() const = 0;
};
