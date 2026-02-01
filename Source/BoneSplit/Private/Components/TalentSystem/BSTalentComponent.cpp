// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/TalentSystem/BSTalentComponent.h"

#include "AbilitySystemComponent.h"
#include "Components/TalentSystem/BSTalentEffect.h"


UBSTalentComponent::UBSTalentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBSTalentComponent::SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
}

void UBSTalentComponent::Server_ResetTalents_Implementation()
{
	for (const auto TalentData : AbilitySystemComponent->GetActiveEffects(
		FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer())))
	{
		FGameplayEffectSpec EffectSpec = AbilitySystemComponent->GetActiveGameplayEffect(TalentData)->Spec;
		if (EffectSpec.Def.Get()->IsA(UBSTalentEffect::StaticClass()))
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(TalentData, -1);
		}
	}
}

void UBSTalentComponent::Server_GrantSavedTalents_Implementation(const TArray<FBSTalentSaveData>& TalentData)
{
	for (const auto Data : TalentData)
	{
		Server_LevelUpTalent(Data.TalentClass);
		Server_SetTalentLevel(Data.TalentClass, Data.CurrentLevel);
	}
}

void UBSTalentComponent::Server_GrantTalents_Implementation(const TArray<TSubclassOf<UBSTalentEffect>>& TalentData)
{
	for (const auto Data : TalentData)
	{
		Server_LevelUpTalent(Data);
	}
}

void UBSTalentComponent::Server_LevelDownTalent_Implementation(const TSubclassOf<UBSTalentEffect> TalentData)
{
	AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(
		TalentData, AbilitySystemComponent, 1);
}

void UBSTalentComponent::Server_LevelUpTalent_Implementation(const TSubclassOf<UBSTalentEffect> TalentData)
{
	const FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
		TalentData, 1, AbilitySystemComponent->MakeEffectContext());
	
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
}

void UBSTalentComponent::Server_SetTalentLevel_Implementation(
	const TSubclassOf<UBSTalentEffect> TalentData, int32 NewLevel)
{
	AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(
		TalentData, AbilitySystemComponent, 1);
}

int32 UBSTalentComponent::GetTalentLevel(const TSubclassOf<UBSTalentEffect> TalentData)
{
	if (const FActiveGameplayEffect* ActiveTalentEffect = GetEffectHandleFromClass(TalentData))
	{
		return ActiveTalentEffect->Spec.GetStackCount();
	}
	return 0;
}

bool UBSTalentComponent::IsMaxLevel(const TSubclassOf<UBSTalentEffect> TalentData)
{
	const UBSTalentEffect* TalentCDO = GetDefault<UBSTalentEffect>(TalentData);
	return TalentCDO->GetMaxLevel() <= GetTalentLevel(TalentData);
}

bool UBSTalentComponent::IsTalentActive(const TSubclassOf<UBSTalentEffect> TalentData)
{
	return GetTalentLevel(TalentData) > 0;
}

bool UBSTalentComponent::CanUnlockOrUpgradeTalent(const TSubclassOf<UBSTalentEffect> TalentData)
{
	//TODO Implement allocation points
	const UBSTalentEffect* TalentCDO = GetDefault<UBSTalentEffect>(TalentData);
	const bool ValidNewLevel = TalentCDO->GetMaxLevel() > GetTalentLevel(TalentData);
	bool ValidRequirements = true;
	
	for (const auto RequiredTalent : TalentCDO->RequiredTalents)
	{
		if (!IsMaxLevel(RequiredTalent))
		{
			ValidRequirements = false;
			break;
		}
	}
	
	bool SuccessAny = TalentCDO->AnyRequiredTalents.IsEmpty();

	for (const auto AnyRequiredTalent : TalentCDO->AnyRequiredTalents)
	{
		if (IsMaxLevel(AnyRequiredTalent))
		{
			SuccessAny = true;
			break;
		}
	}
	return ValidRequirements && ValidNewLevel && SuccessAny;
}

TMap<TSubclassOf<UBSTalentEffect>, int32> UBSTalentComponent::GetTalentData()
{
	TMap<TSubclassOf<UBSTalentEffect>, int32> TalentEffects;
	
	TArray<FActiveGameplayEffectHandle> ActiveEffects = 
	AbilitySystemComponent->GetActiveEffects(
		FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer()));

	for (auto ActiveEffect : ActiveEffects)
	{
		if (const FActiveGameplayEffect* ActiveGameplayEffect = AbilitySystemComponent->GetActiveGameplayEffect(ActiveEffect))
		{
			if (ActiveGameplayEffect->Spec.Def.Get()->IsA(UBSTalentEffect::StaticClass()))
			{
				TalentEffects.Add(ActiveGameplayEffect->Spec.Def.GetClass(), ActiveGameplayEffect->Spec.GetStackCount());
			}
		}
	}
	return TalentEffects;
}

const FActiveGameplayEffect* UBSTalentComponent::GetEffectHandleFromClass(const TSubclassOf<UGameplayEffect> EffectClass)
{
	TArray<FActiveGameplayEffectHandle> ActiveEffects = 
		AbilitySystemComponent->GetActiveEffects(
			FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer()));
	
	for (const auto& EffectHandle : ActiveEffects)
	{
		if (const FActiveGameplayEffect* ActiveGameplayEffect = 
			AbilitySystemComponent->GetActiveGameplayEffect(EffectHandle); ActiveGameplayEffect && 
			ActiveGameplayEffect->Spec.Def.GetClass() == EffectClass)
		{
			return ActiveGameplayEffect;
		}
	}
	return nullptr;
}
