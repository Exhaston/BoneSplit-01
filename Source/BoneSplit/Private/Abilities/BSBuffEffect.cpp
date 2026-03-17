// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSBuffEffect.h"

#include "DefaultAttributeSet.h"

UBSBuffEffect::UBSBuffEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	//CachedGrantedTags.AddTag(DefaultAbilityTags::Cooldown_Ability);
	
	FSetByCallerFloat DurationCaller;
	DurationCaller.DataTag = DefaultSetByCallerTags::SetByCaller_Duration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationCaller);
}

TSoftObjectPtr<UTexture2D> UBSBuffEffect::GetIcon_Implementation() const
{
	return BuffIcon;
}

FLinearColor UBSBuffEffect::GetTint_Implementation() const
{
	return BuffIconTint;
}
