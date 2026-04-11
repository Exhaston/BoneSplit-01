// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BSDamageEffect.h"
#include "BSRawDamageEffect.generated.h"

/**
 * A damage effect that bypasses scaling and crits. A damage number on this effect will be the final value.
 */
UCLASS(DisplayName="Raw Damage Effect", Blueprintable, BlueprintType)
class BONESPLIT_API UBSRawDamageEffect : public UBSDamageEffect
{
	GENERATED_BODY()
	
public:
	
};
