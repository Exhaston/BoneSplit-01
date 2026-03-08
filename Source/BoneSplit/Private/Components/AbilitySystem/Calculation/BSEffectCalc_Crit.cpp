// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/Calculation/BSEffectCalc_Crit.h"

#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

UBSEffectCalc_Crit::UBSEffectCalc_Crit()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, CritChance, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSet, CritMod, Source, false);

	RelevantAttributesToCapture.Add(CritChanceDef);
	RelevantAttributesToCapture.Add(CritModDef);
}

void UBSEffectCalc_Crit::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	/*
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	float EffectCritChance = 0;
	float EffectCritMod = 0;
	
	for (const FGameplayModifierInfo& Mod : Spec.Def->Modifiers)
	{                                                                                                  
		if (Mod.Attribute == UBSAttributeSet::GetCritChanceAttribute())
		{
			Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, EffectCritChance);
		}
		if (Mod.Attribute == UBSAttributeSet::GetCritModAttribute())
		{
			Mod.ModifierMagnitude.AttemptCalculateMagnitude(Spec, EffectCritMod);
		}
	} 
	
	const FAggregatorEvaluateParameters EvalParams;
	
	float CasterCritChance = 0;
	float CasterCritMod = 0;
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritChanceDef, EvalParams, CasterCritChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritModDef, EvalParams, CasterCritMod);
	
	float TotalCritChance = CasterCritChance + EffectCritChance;
	TotalCritChance = FMath::Clamp(TotalCritChance, 0.0f, 1.f);
	
	float TotalCritMod = 1; //base if we dont crit
	
	if (UKismetMathLibrary::RandomBoolWithWeight(TotalCritChance))
	{
		TotalCritMod =  CasterCritMod + EffectCritMod;
	}		
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UBSAttributeSet::GetIncomingCritModifierAttribute(),
		EGameplayModOp::Override,
		TotalCritMod));
		
		*/
}
