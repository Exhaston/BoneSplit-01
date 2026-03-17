// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSHealingEffect.h"

#include "DefaultAttributeSet.h"
#include "Abilities/BSExtendedAttributeSet.h"

UBSHealingEffect::UBSHealingEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	FGameplayModifierInfo ModHealing;
	ModHealing.Attribute = UDefaultAttributeSet::GetHealingAttribute();
		
	FSetByCallerFloat DamageCaller;
	DamageCaller.DataTag = DefaultSetByCallerTags::SetByCaller_Healing;
	ModHealing.ModifierMagnitude = FGameplayEffectModifierMagnitude(DamageCaller);
	ModHealing.ModifierOp = EGameplayModOp::Override;
	
	Modifiers.AddUnique(ModHealing);
	
	FGameplayModifierInfo ModKnockback;
	ModKnockback.Attribute = UBSExtendedAttributeSet::GetKnockbackAttribute();
		
	FSetByCallerFloat KnockbackCaller;
	KnockbackCaller.DataTag = BSExtendedAttributeTags::Attribute_Knockback;
	ModKnockback.ModifierMagnitude = FGameplayEffectModifierMagnitude(KnockbackCaller);
	ModKnockback.ModifierOp = EGameplayModOp::Override;
	
	Modifiers.AddUnique(ModKnockback);
}
