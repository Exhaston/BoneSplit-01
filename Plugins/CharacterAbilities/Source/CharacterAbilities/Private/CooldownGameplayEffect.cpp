// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "CooldownGameplayEffect.h"

#include "AbilitiesExtensionLib.h"
#include "DefaultAttributeSet.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UCooldownGameplayEffect::UCooldownGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	//CachedGrantedTags.AddTag(DefaultAbilityTags::Cooldown_Ability);
	
	FSetByCallerFloat DurationCaller;
	DurationCaller.DataTag = DefaultSetByCallerTags::SetByCaller_Duration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationCaller);
	
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	StackingType = EGameplayEffectStackingType::None;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	
	/*

	
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	
	//Blame epic

	
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration;
	
	StackLimitCount = 100;
	*/
}
