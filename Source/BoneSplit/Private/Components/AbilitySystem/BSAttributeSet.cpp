// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilitySystemInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Statics/BSMathLibrary.h"

void UBSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, MagicRes, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, Power, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSAttributeSet, MultiHit, COND_None, REPNOTIFY_Always);
	
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

#pragma region AttributeEvents

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
	if (Attribute == GetHealthAttribute() && GetMaxHealth() > 0) return GetMaxHealth();
	if (Attribute == GetManaAttribute() && GetMaxMana() > 0) return GetMaxMana();
	if (Attribute == GetSoulChargeAttribute()) return 1;
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
	if (AbilityComp->GetNumericAttribute(AffectedAttribute) <= 0) return;
	if (OldValue <= 0) return;
	if (const float CurrentMaxValue = OldValue; 
		!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AbilityComp->GetNumericAttribute(AffectedAttribute);
		const float NewDelta = (CurrentMaxValue > 0.f) ? 
		(CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
		AbilityComp->ApplyModToAttribute(AffectedAttribute, EGameplayModOp::Additive, NewDelta);
	}
}

void UBSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDead = GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(BSTags::Status_Dead);
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute() && GetHealth() > 0 && !bIsDead)
	{
		HandleDamage(Data);
	}
	
	if (Data.EvaluatedData.Attribute == GetHealingAttribute() && GetHealth() > 0 && !bIsDead)
	{
		HandleHealing(Data);
	}
	
	if (Data.EvaluatedData.Attribute == GetKnockbackAttribute() && GetHealth() > 0 && !bIsDead)
	{
		HandleKnockback(Data);
	}
	
	if (GetHealth() <= 0 && !bIsDead)
	{
		HandleDeath(Data);
	}
}

void UBSAttributeSet::HandleDamage(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayEffectSpec& Spec = Data.EffectSpec;
	
	FGameplayTagContainer EffectTags;
	Spec.GetAllAssetTags(EffectTags);
	
	UAbilitySystemComponent* SourceAsc = Spec.GetEffectContext().GetInstigatorAbilitySystemComponent();
	UAbilitySystemComponent* TargetAsc = &Data.Target;
	
	if (!SourceAsc || !TargetAsc) return;
	
	//Grab temp damage meta attribute and reset it for next calculation
	float LocalDamage = GetDamage();
	SetDamage(0);
	
	const float LocalCritMod = GetCriticalModifier(Data);
	const float PowerMod = GetPowerModifier(Data);
	
	LocalDamage += PowerMod;
	
	LocalDamage *= LocalCritMod;
	
	FGameplayEventData DamageEventPayload;
	DamageEventPayload.Target = TargetAsc->GetAvatarActor();
	DamageEventPayload.ContextHandle = Spec.GetEffectContext();
	DamageEventPayload.EventMagnitude = LocalDamage * Spec.GetStackCount();
	DamageEventPayload.InstigatorTags.AppendTags(EffectTags);
	if (LocalCritMod > 1) DamageEventPayload.InstigatorTags.AddTagFast(BSTags::EffectTag_Critical);

	//Source Event
	SourceAsc->HandleGameplayEvent(BSTags::GameplayEvent_DamageDealt, &DamageEventPayload);  
	
	//Target Event
	TargetAsc->HandleGameplayEvent(BSTags::GameplayEvent_DamageTaken, &DamageEventPayload); 
	
	if (float NewShield = GetShield(); NewShield > 0)
	{
		if (LocalDamage <= NewShield)
		{
			NewShield -= LocalDamage;
			LocalDamage = 0;
		}
		else
		{
			LocalDamage -= NewShield;
			NewShield = 0;
		}
		
		SetShield(FMath::Clamp(NewShield, 0, FLT_MAX));
	}
	
	if (LocalDamage > 0)
	{
		SetHealth(FMath::Clamp(GetHealth() - LocalDamage, 0, GetMaxHealth()));
	}
}

void UBSAttributeSet::HandleHealing(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayEffectSpec& Spec = Data.EffectSpec;
	
	FGameplayTagContainer EffectTags;
	Spec.GetAllAssetTags(EffectTags);
	
	UAbilitySystemComponent* SourceAsc = Spec.GetEffectContext().GetInstigatorAbilitySystemComponent();
	UAbilitySystemComponent* TargetAsc = &Data.Target;
	
	if (!SourceAsc || !TargetAsc) return;
	
	float LocalHealing = GetHealing();
	SetHealing(0);
	
	const float LocalCritMod = GetCriticalModifier(Data);
	const float PowerMod = GetPowerModifier(Data);
	
	LocalHealing += PowerMod;
	
	LocalHealing *= LocalCritMod;
	
	FGameplayEventData HealingEventPayload;
	HealingEventPayload.Target = TargetAsc->GetAvatarActor();
	HealingEventPayload.ContextHandle = Spec.GetEffectContext();
	HealingEventPayload.EventMagnitude = LocalHealing * Spec.GetStackCount();
	HealingEventPayload.InstigatorTags.AppendTags(EffectTags);
	if (LocalCritMod > 1) HealingEventPayload.InstigatorTags.AddTagFast(BSTags::EffectTag_Critical);

	//Source Event
	SourceAsc->HandleGameplayEvent(BSTags::GameplayEvent_DamageDealt, &HealingEventPayload);  
	
	//Target Event
	TargetAsc->HandleGameplayEvent(BSTags::GameplayEvent_DamageTaken, &HealingEventPayload); 
	
	if (LocalHealing > 0)
	{
		SetHealth(FMath::Clamp(GetHealth() + LocalHealing, 0, GetMaxHealth()));
	}
}

void UBSAttributeSet::HandleKnockback(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayEffectSpec& Spec = Data.EffectSpec;
	const UAbilitySystemComponent* SourceAsc = Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAsc = &Data.Target;
	const AActor* SourceAvatar = SourceAsc->GetAvatarActor();
	
	if (!SourceAsc || !TargetAsc || !SourceAvatar) return;

	const float LocalKnockback = GetKnockback();
	SetKnockback(0);

	const FVector Origin = 
		Spec.GetEffectContext().HasOrigin() ? Spec.GetEffectContext().GetOrigin() : SourceAvatar->GetActorLocation();
	
	if (!FMath::IsNearlyZero(LocalKnockback) && Spec.GetEffectContext().HasOrigin()
		&& TargetAsc && TargetAsc->GetAvatarActor())
	{
		if (IBSAbilitySystemInterface* AscInterface = Cast<IBSAbilitySystemInterface>(TargetAsc->GetAvatarActor()))
		{
			const FVector Direction = 
				TargetAsc->GetAvatarActor()->GetActorLocation() - Origin;
            
			AscInterface->Launch(
			Direction.GetSafeNormal() * LocalKnockback, 
			false);
		}
	}
}

void UBSAttributeSet::HandleDeath(const FGameplayEffectModCallbackData& Data)
{
	UAbilitySystemComponent* TargetAsc = &Data.Target;
	if (!TargetAsc) return;
	
	AActor* AvatarActor = TargetAsc->GetAvatarActor();
	if (!AvatarActor) return;
	
	if (IBSAbilitySystemInterface* Killable = Cast<IBSAbilitySystemInterface>(AvatarActor))
	{
		TargetAsc->SetLooseGameplayTagCount(
		BSTags::Status_Dead, 1, EGameplayTagReplicationState::TagAndCountToAll);
		
		Killable->Die(Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent(), Data.EvaluatedData.Magnitude);
	}
}

float UBSAttributeSet::GetCriticalModifier(const FGameplayEffectModCallbackData& Data)
{
	const UAbilitySystemComponent* CasterAsc = Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent();
	float TotalCritChance = CasterAsc->GetNumericAttribute(GetCritChanceAttribute());
	if (auto* CritAttrMod = 
	Data.EffectSpec.GetModifiedAttribute(GetCritChanceAttribute()))
	{
		TotalCritChance += CritAttrMod->TotalMagnitude;
	}
	
	float TotalCritMod = 1; //base if we dont crit
	
	if (UKismetMathLibrary::RandomBoolWithWeight(FMath::Clamp(TotalCritChance, 0.0f, 1.f)))
	{
		TotalCritMod = CasterAsc->GetNumericAttribute(GetCritModAttribute());
		if (auto* CritAttrMod = 
		Data.EffectSpec.GetModifiedAttribute(GetCritModAttribute()))
		{
			TotalCritMod += CritAttrMod->TotalMagnitude;
		}
		
		if (TotalCritChance > 1) TotalCritMod += TotalCritChance - 1; //Bonus crit as bonus mod damage instead.
	}	
	
	return TotalCritMod;
}

float UBSAttributeSet::GetPowerModifier(const FGameplayEffectModCallbackData& Data)
{
	const UAbilitySystemComponent* CasterAsc = Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent();
	float TotalPower = CasterAsc->GetNumericAttribute(GetPowerAttribute());
	if (auto* CritAttrMod = 
	Data.EffectSpec.GetModifiedAttribute(GetPowerAttribute()))
	{
		TotalPower += CritAttrMod->TotalMagnitude;
	}
	
	TotalPower = UBSMathLibrary::CalculateDiminishingReturns(TotalPower, 200);
	
	return TotalPower;
}

#pragma endregion
