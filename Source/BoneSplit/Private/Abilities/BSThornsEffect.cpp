// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSThornsEffect.h"

#include "DefaultAttributeSet.h"

UBSThornsEffect::UBSThornsEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	FGameplayModifierInfo ModDamage;
	ModDamage.Attribute = UDefaultAttributeSet::GetDamageAttribute();
		
	FSetByCallerFloat DamageCaller;
	DamageCaller.DataTag = DefaultSetByCallerTags::SetByCaller_Damage;
	ModDamage.ModifierMagnitude = FGameplayEffectModifierMagnitude(DamageCaller);
	ModDamage.ModifierOp = EGameplayModOp::Override;
	
	Modifiers.AddUnique(ModDamage);
}
