// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSThreatInterface.generated.h"

class UBSThreatComponent;

UINTERFACE()
class UBSThreatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for optimized quick access of the frequently needed threat component. 
 * The threat component contains valid data on the server only. 
 * Components carry overhead for replication so replicate targets etc. manually.
 */
class BONESPLIT_API IBSThreatInterface
{
	GENERATED_BODY()

public:
	
	virtual UBSThreatComponent* GetThreatComponent() = 0;
};
