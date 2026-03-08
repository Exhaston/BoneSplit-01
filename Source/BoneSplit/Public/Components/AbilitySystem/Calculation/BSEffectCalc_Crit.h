// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BSEffectCalc_Crit.generated.h"

UCLASS(NotBlueprintable, DisplayName="Crit Calculation")
class BONESPLIT_API UBSEffectCalc_Crit : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	
	UBSEffectCalc_Crit();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
private:
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritMod);
};
