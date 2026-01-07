// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UBSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, MagicRes, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Power, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, CritMod, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Friction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, JumpHeight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, AirControl, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, CooldownReduction, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Reach, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, SoulCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Thorns, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, SoulGems, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Bones, COND_None, REPNOTIFY_Always);
}

void UBSAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	NewValue = FMath::Clamp<float>(NewValue, GetMinForAttribute(Attribute), GetMaxForAttribute(Attribute));
	
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UBSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	NewValue = FMath::Clamp<float>(NewValue, GetMinForAttribute(Attribute), GetMaxForAttribute(Attribute));
	
	Super::PreAttributeChange(Attribute, NewValue);
}

void UBSAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(GetHealthAttribute(), OldValue, NewValue);
	}
}

float UBSAttributeSet::GetMaxForAttribute(const FGameplayAttribute& Attribute) const
{
	if (Attribute == GetHealthAttribute() && GetMaxHealth() != -1) return GetMaxHealth();
	
	if (Attribute == GetManaAttribute() && GetMaxMana() != -1) return GetMaxMana();
	
	return FLT_MAX;
}

float UBSAttributeSet::GetMinForAttribute(const FGameplayAttribute& Attribute)
{
	if (Attribute == GetMaxHealthAttribute()) return 1;
	if (Attribute == GetMaxManaAttribute()) return 1;
	return 0;
}

void UBSAttributeSet::AdjustAttributeForMaxChange(
	const FGameplayAttribute& AffectedAttribute, const float OldValue, const float NewMaxValue) const
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();

	if (const float CurrentMaxValue = OldValue; 
		!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AbilityComp->GetNumericAttribute(AffectedAttribute);
		const float NewDelta = (CurrentMaxValue > 0.f) ? 
		(CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
		AbilityComp->ApplyModToAttribute(AffectedAttribute, EGameplayModOp::Additive, NewDelta);
	}
}
