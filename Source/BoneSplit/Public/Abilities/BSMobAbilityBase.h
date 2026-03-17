// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AbilitySystem/AbilityBases/BSAbilityBase.h"
#include "BSMobAbilityBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract, DisplayName="Mob Ability Base", HideCategories=("Buffering", "Player"))
class BONESPLIT_API UBSMobAbilityBase : public UBSAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=0, ClampMax=100, Units=Percent))
	float AbilityWeight = 100;
};
