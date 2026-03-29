// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Abilities/BSPlayerAbilityBase.h"

#include "AbilitiesExtensionLib.h"
#include "AbilitySystemComponent.h"
#include "Abilities/BSExtendedAttributeSet.h"

TSoftObjectPtr<UTexture2D> UBSPlayerAbilityBase::GetIcon_Implementation() const
{
	return AbilityIcon;
}

FLinearColor UBSPlayerAbilityBase::GetTint_Implementation() const
{
	return IconTint;
}

void UBSPlayerAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	if (FGameplayAbilitySpec* MutableSpec = GetCurrentAbilitySpec(); MutableSpec && InputID > 0)
	{
		MutableSpec->InputID = InputID;
	}
}

float UBSPlayerAbilityBase::GetCostModifier() const
{
	return 1 - GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(
		UBSExtendedAttributeSet::GetAbilityCostModifierAttribute());
}

float UBSPlayerAbilityBase::GetCooldownModifier() const
{
	float Cdr = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(
		UBSExtendedAttributeSet::GetCooldownReductionModifierAttribute());
	
	Cdr = UAbilitiesExtensionLib::AsymptoticDr(Cdr, 1);
	return Cdr;
}
