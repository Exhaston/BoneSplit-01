// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "DefaultAttributeSet.h"

#include "CharacterAbilitySystem.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

namespace DefaultSetByCallerTags
{
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Shield, "SetByCaller.Shield");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_CurrentHealth, "SetByCaller.CurrentHealth");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_MaxHealth, "SetByCaller.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Healing, "SetByCaller.Healing");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Duration, "SetByCaller.Duration");
}

namespace DefaultGameplayEvents
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_DamageTaken, "GameplayEvent.DamageTaken");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_DamageDealt, "GameplayEvent.DamageDealt");
}

void UDefaultAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDefaultAttributeSet, Shield, COND_None, REPNOTIFY_Always);
}

void UDefaultAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectSpec& Spec = Data.EffectSpec;
	
	FGameplayTagContainer EffectTags;
	Spec.GetAllAssetTags(EffectTags);
	
	UCharacterAbilitySystem* InstigatorAsc = 
		Cast<UCharacterAbilitySystem>(Spec.GetEffectContext().GetInstigatorAbilitySystemComponent());
	
	UCharacterAbilitySystem* TargetAsc = 
		Cast<UCharacterAbilitySystem>(&Data.Target);
	
	if (!InstigatorAsc || !TargetAsc || !InstigatorAsc->GetAvatarActor() || !TargetAsc->GetAvatarActor()) return;
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute() && GetDamage() > 0)
	{
		float IncomingDamage = GetDamage();
		SetDamage(0);
		HandleDamage(InstigatorAsc, TargetAsc, IncomingDamage, IncomingDamage, EffectTags, Data.EffectSpec);
	}
	
	if (Data.EvaluatedData.Attribute == GetHealingAttribute() && GetHealing() > 0)
	{
		float IncomingHealing = GetHealing();
		SetHealing(0);
		HandleHealing(InstigatorAsc, TargetAsc, IncomingHealing, IncomingHealing, EffectTags, Data.EffectSpec);
	}
}

void UDefaultAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	NewValue = FMath::Clamp<float>(NewValue, GetMinForAttribute(Attribute), GetMaxForAttribute(Attribute));
	
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UDefaultAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	NewValue = FMath::Clamp<float>(NewValue, GetMinForAttribute(Attribute), GetMaxForAttribute(Attribute));
	
	Super::PreAttributeChange(Attribute, NewValue);
}

void UDefaultAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(
			GetOwningAbilitySystemComponent(), GetCurrentHealthAttribute(), OldValue, NewValue);
	}
}

float UDefaultAttributeSet::GetMaxForAttribute(const FGameplayAttribute& Attribute) const
{
	if (Attribute == GetCurrentHealthAttribute() && GetMaxHealth() > 0) return GetMaxHealth();
	return FLT_MAX;
}

float UDefaultAttributeSet::GetMinForAttribute(const FGameplayAttribute& Attribute) const
{
	if (Attribute == GetCurrentHealthAttribute()) return 0;
	return 0;
}

void UDefaultAttributeSet::AdjustAttributeForMaxChange(
	UAbilitySystemComponent* OwningAsc, 
	const FGameplayAttribute& AffectedAttribute, 
	const float OldValue, 
	const float NewMaxValue)
{
	if (!OwningAsc || !AffectedAttribute.IsValid() || 
		OwningAsc->GetNumericAttribute(AffectedAttribute) <= 0 || OldValue <= 0) return;
	
	const float CurrentMaxValue = OldValue;
	
	if (FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue)) return;
	
	const float CurrentValue = OwningAsc->GetNumericAttribute(AffectedAttribute);
	const float NewDelta = (CurrentMaxValue > 0.f) ? 
	(CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
	OwningAsc->ApplyModToAttribute(AffectedAttribute, EGameplayModOp::Additive, NewDelta);
}

void UDefaultAttributeSet::HandleHealing(UCharacterAbilitySystem* InstigatorAsc, UCharacterAbilitySystem* TargetAsc,
	float BaseHeal, float& HealToApply, FGameplayTagContainer EffectTags, const FGameplayEffectSpec& EffectSpec)
{
	SetCurrentHealth(FMath::Clamp(GetCurrentHealth() + HealToApply, 0, GetMaxHealth()));
	
	InstigatorAsc->NetMulticast_OnHealingDealt(InstigatorAsc, TargetAsc, EffectTags, BaseHeal, HealToApply, EffectSpec);
	TargetAsc->NetMulticast_OnHealReceived(InstigatorAsc, TargetAsc, EffectTags, BaseHeal, HealToApply, EffectSpec);
}

void UDefaultAttributeSet::HandleDamage(UCharacterAbilitySystem* InstigatorAsc, UCharacterAbilitySystem* TargetAsc,
	float BaseDamage, float& DamageToApply, FGameplayTagContainer EffectTags, const FGameplayEffectSpec& EffectSpec)
{
	if (!InstigatorAsc || !TargetAsc) return;
	
	float MitigatedDamage = DamageToApply;
	
	if (float NewShield = GetShield(); NewShield > 0)
	{
		if (MitigatedDamage <= NewShield)
		{
			NewShield -= MitigatedDamage;
			MitigatedDamage = 0;
		}
		else
		{
			MitigatedDamage -= NewShield;
			NewShield = 0;
		}
		
		SetShield(FMath::Clamp(NewShield, 0, FLT_MAX));
	}
	
	if (MitigatedDamage > 0)
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth() - MitigatedDamage, 0, GetMaxHealth()));
	}
	
	FGameplayEventData DamagePayload;
	DamagePayload.ContextHandle = TargetAsc->MakeEffectContext();
	DamagePayload.Target = TargetAsc->GetAvatarActor();
	DamagePayload.EventTag = DefaultGameplayEvents::GameplayEvent_DamageTaken;
	DamagePayload.Instigator = InstigatorAsc->GetAvatarActor();
	DamagePayload.EventMagnitude = MitigatedDamage;
	
	TargetAsc->HandleGameplayEvent(DefaultGameplayEvents::GameplayEvent_DamageTaken, &DamagePayload);
	
	TargetAsc->NetMulticast_OnDamageTaken(InstigatorAsc, TargetAsc, EffectTags, BaseDamage, DamageToApply, EffectSpec);
	InstigatorAsc->NetMulticast_OnDamageDealt(InstigatorAsc, TargetAsc, EffectTags, BaseDamage, DamageToApply, EffectSpec);
	
	if (GetCurrentHealth() <= 0)
	{
		TargetAsc->NetMulticast_OnHealthReachZero(InstigatorAsc, TargetAsc, EffectTags, BaseDamage, DamageToApply, EffectSpec);
	}
}

