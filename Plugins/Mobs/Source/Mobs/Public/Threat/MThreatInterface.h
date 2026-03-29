// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MThreatComponent.h"
#include "UObject/Interface.h"
#include "MThreatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMThreatInterface : public UInterface
{
	GENERATED_BODY()
};

class MOBS_API IMThreatInterface
{
	GENERATED_BODY()

public:
	
	virtual UMThreatComponent* GetThreatComponent() = 0;
};
