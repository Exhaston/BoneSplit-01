// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "FactionInterface.generated.h"


UINTERFACE()
class UFactionInterface : public UInterface
{
	GENERATED_BODY()
};

class CHARACTERABILITIES_API IFactionInterface
{
	GENERATED_BODY()

public:
	
	virtual FGameplayTagContainer& GetFactionTags() = 0;
	virtual bool HasAnyMatchingFactionTag(FGameplayTagContainer InFactionTags) = 0;
};
