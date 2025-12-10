// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/EffectBases/BSGameplayEffect.h"

#include "Components/AbilitySystem/Calculation/BSPowerCalculation.h"

UBSGameplayEffect::UBSGameplayEffect()
{
	Executions.Add(FGameplayEffectExecutionDefinition(UBSPowerCalculation::StaticClass()));
}

#if WITH_EDITOR

bool UBSGameplayEffect::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSGameplayEffect, Executions))
	{
		return false;
	}
	return ParentVal;
}

#endif
