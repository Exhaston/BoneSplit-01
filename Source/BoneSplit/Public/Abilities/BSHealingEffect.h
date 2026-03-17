// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "BSHealingEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DisplayName="Healing Effect Base")
class BONESPLIT_API UBSHealingEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	
	UBSHealingEffect();
};
