// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSDamageEffect.h"

#include "DefaultAttributeSet.h"
#include "Abilities/BSExtendedAttributeSet.h"

UBSDamageEffect::UBSDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	FGameplayModifierInfo ModDamage;
	ModDamage.Attribute = UDefaultAttributeSet::GetDamageAttribute();
		
	FSetByCallerFloat DamageCaller;
	DamageCaller.DataTag = DefaultSetByCallerTags::SetByCaller_Damage;
	ModDamage.ModifierMagnitude = FGameplayEffectModifierMagnitude(DamageCaller);
	ModDamage.ModifierOp = EGameplayModOp::Override;
	
	Modifiers.AddUnique(ModDamage);
	
	FGameplayModifierInfo ModKnockback;
	ModKnockback.Attribute = UBSExtendedAttributeSet::GetKnockbackAttribute();
		
	FSetByCallerFloat KnockbackCaller;
	KnockbackCaller.DataTag = BSExtendedAttributeTags::SetByCaller_Knockback;
	ModKnockback.ModifierMagnitude = FGameplayEffectModifierMagnitude(KnockbackCaller);
	ModKnockback.ModifierOp = EGameplayModOp::Override;
	
	Modifiers.AddUnique(ModKnockback);
}
