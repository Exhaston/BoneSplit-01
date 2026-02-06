// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/Calculation/BSPowerCalculation.h"

#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilitySystemInterface.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "Components/Targeting/BSThreatInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UBSPowerCalculation::UBSPowerCalculation()
{
    DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, CritChance, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, CritMod, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, Power, Source, false);
    
	DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, Armor, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, MagicRes, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, Shield, Target, false);

    RelevantAttributesToCapture.Add(CritChanceDef);
    RelevantAttributesToCapture.Add(CritModDef);
	RelevantAttributesToCapture.Add(PowerDef);
    
	RelevantAttributesToCapture.Add(ArmorDef);
	RelevantAttributesToCapture.Add(MagicResDef);
    RelevantAttributesToCapture.Add(ShieldDef);
}

void UBSPowerCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
                    
    UAbilitySystemComponent* SourceAsc = Spec.GetEffectContext().GetOriginalInstigatorAbilitySystemComponent();
    const UAbilitySystemComponent* TargetAsc = ExecutionParams.GetTargetAbilitySystemComponent();
    
    // =================================================================================================================
    // Spec Attributes
    // ================================================================================================================= 
    
    float BasePhysicalDamage = 0;
    float BaseThreat = 0;
    float BaseMagicDamage = 0;
    float BaseHealing = 0;
    float BaseCritChance = 0;
    float BaseCritMod = 0;
    float BaseKnockback = 0;
    
    constexpr float DiminishingReturnFactor = 50;

    for (const FGameplayModifierInfo& Mod : Spec.Def->Modifiers)
    {                                                                                                  
        //Transfer the damage to the local variable so we can calculate on it before changing health
        if (Mod.Attribute == UBSAttributeSet::GetPhysicalDamageAttribute())
        {
            Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, BasePhysicalDamage);
            
            //Reset the damage attribute, it's purely a temp value for conversion
            OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
                UBSAttributeSet::GetPhysicalDamageAttribute(),
                EGameplayModOp::Override,
                0));
        }
        else if (Mod.Attribute == UBSAttributeSet::GetThreatAttribute())
        {
            Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, BaseThreat);
                                   
            //Reset the threat attribute, it's purely a temp value for conversion on the server
            OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
                UBSAttributeSet::GetThreatAttribute(),
                EGameplayModOp::Override,
                0));
                                       
        }
        else if (Mod.Attribute == UBSAttributeSet::GetMagicDamageAttribute())
        {
            Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, BaseMagicDamage);
            
            OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
                UBSAttributeSet::GetMagicDamageAttribute(),
                EGameplayModOp::Override,
                0));
        }
        else if (Mod.Attribute == UBSAttributeSet::GetHealingAttribute())
        {
            Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, BaseHealing);
            
            OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
                UBSAttributeSet::GetHealingAttribute(),
                EGameplayModOp::Override,
                0));
        }
        else if (Mod.Attribute == UBSAttributeSet::GetCritChanceAttribute())
        {
            Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, BaseCritChance);
        }
        else if (Mod.Attribute == UBSAttributeSet::GetCritModAttribute())
        {
            Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, BaseCritMod);
        }
        else if (Mod.Attribute == UBSAttributeSet::GetKnockbackAttribute())
        {
            Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, BaseKnockback);
        }
    } 
    
    if (!FMath::IsNearlyZero(BaseKnockback) && Spec.GetEffectContext().HasOrigin()
        && TargetAsc && TargetAsc->GetAvatarActor())
    {
        if (IBSAbilitySystemInterface* AscInterface = Cast<IBSAbilitySystemInterface>(TargetAsc->GetAvatarActor()))
        {
            const FVector Direction = 
                TargetAsc->GetAvatarActor()->GetActorLocation() - Spec.GetEffectContext().GetOrigin();
            
            AscInterface->Launch(
            Direction.GetSafeNormal() * BaseKnockback, 
            false);
        }
    }
    
    // =================================================================================================================
    // Ability System Attributes
    // ================================================================================================================= 
    
    const FAggregatorEvaluateParameters EvalParams;
    
    float SourcePower = 0; //Source 
    float SourceCritChance = 0; //Source 
    float SourceCritMod = 0; //Source 
    
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritChanceDef, EvalParams, SourceCritChance);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritModDef, EvalParams, SourceCritMod);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(PowerDef, EvalParams, SourcePower);
    
    const float TotalCritMod = BaseCritMod + SourceCritMod;
    const float TotalCritChance = FMath::Clamp(BaseCritChance + SourceCritChance, 0.0f, 1.f);
    
    const bool bCrit = UKismetMathLibrary::RandomBoolWithWeight(TotalCritChance);
    
    const float PowerDr = SourcePower / (SourcePower + DiminishingReturnFactor);
    const float PowerModifier = 1 + PowerDr;
    
    // =================================================================================================================
    // Healing
    // ================================================================================================================= 
    
    if (!FMath::IsNearlyZero(BaseHealing) && BaseHealing > 0)
    {
        const float TotalHealing = BaseHealing * PowerModifier * (bCrit ? TotalCritMod : 1);
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            UBSAttributeSet::GetHealthAttribute(),
            EGameplayModOp::Additive,
            TotalHealing
        ));
        
        if (SourceAsc)
        {
            FGameplayEventData Data;
            Data.ContextHandle = Spec.GetEffectContext();
            Data.EventMagnitude = TotalHealing;
            if (bCrit)
            {
                Data.InstigatorTags.AddTag(BSTags::Effect_Crit);
            }
            SourceAsc->HandleGameplayEvent(BSTags::GameplayEvent_HealingDealt, &Data);
        }
    }          
    
    // =================================================================================================================
    // Damage
    // =================================================================================================================
    
    if (FMath::IsNearlyZero(BasePhysicalDamage) && FMath::IsNearlyZero(BaseMagicDamage)) return; //No damage
    
    //Mitigation first
    float Armor = 0;
    float MagicRes = 0;
    float Shield = 0;

    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ArmorDef, EvalParams, Armor);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MagicResDef, EvalParams, MagicRes);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ShieldDef, EvalParams, Shield);

    const float PhysicalMitigation = Armor / (Armor + DiminishingReturnFactor);
    const float MagicMitigation = MagicRes / (MagicRes + DiminishingReturnFactor);

    const float TotalPhysicalDamage = BasePhysicalDamage * PowerModifier * (bCrit ? TotalCritMod : 1);
    const float TotalMagicDamage = BaseMagicDamage * PowerModifier * (bCrit ? TotalCritMod : 1);

    float DamageRemaining = TotalPhysicalDamage *  (1 - PhysicalMitigation) + TotalMagicDamage * (1 - MagicMitigation);
    
    if (IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(TargetAsc->GetAvatarActor()))
    {
        ThreatInterface->GetThreatComponent()->AddThreat(SourceAsc->GetAvatarActor(), DamageRemaining + BaseThreat);
    }
    
    if (SourceAsc) //We broadcast event regardless of shields, damage done is damage done.
    {
        FGameplayEventData Data;
        Data.Target = TargetAsc->GetAvatarActor();
        Data.ContextHandle = Spec.GetEffectContext();
        Data.EventMagnitude = DamageRemaining;
        FGameplayTagContainer EffectTags;
        Spec.GetAllAssetTags(EffectTags);
        Data.InstigatorTags.AppendTags(EffectTags);
        if (bCrit)
        {
            Data.InstigatorTags.AddTagFast(BSTags::EffectTag_Critical);
        }
        SourceAsc->HandleGameplayEvent(BSTags::GameplayEvent_DamageDealt, &Data);
    }
    
    if (float NewShield = Shield; NewShield > 0)
    {
        if (DamageRemaining <= NewShield)
        {
            NewShield -= DamageRemaining;
            DamageRemaining = 0;
        }
        else
        {
            DamageRemaining -= NewShield;
            NewShield = 0;
        }

        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            UBSAttributeSet::GetShieldAttribute(),
            EGameplayModOp::Override,
            NewShield
        ));
    }
    
    if (DamageRemaining > 0)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            UBSAttributeSet::GetHealthAttribute(),
            EGameplayModOp::Additive,
            -DamageRemaining));
    }
}
