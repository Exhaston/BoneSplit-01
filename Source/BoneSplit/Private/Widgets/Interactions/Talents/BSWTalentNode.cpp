// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Interactions/Talents/BSWTalentNode.h"

#include "AbilitySystemComponent.h"
#include "CommonLazyImage.h"
#include "CommonNumericTextBlock.h"
#include "Actors/Player/BSPlayerState.h"
#include "Components/TalentSystem/BSTalentComponent.h"
#include "Components/TalentSystem/BSTalentEffect.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/Base/BSWToolTipBase.h"

void UBSWTalentNode::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (TalentData)
	{
		const UBSTalentEffect* TalentCDO = GetDefault<UBSTalentEffect>(TalentData);
		if (!TalentCDO->TalentIcon.IsNull())
		{
			TalentIconImage->SetBrushFromLazyTexture(TalentCDO->TalentIcon);
			UpdateText(0);
		}
	}
}

void UBSWTalentNode::NativeConstruct()
{
	Super::NativeConstruct();
	if (TalentData)
	{
		const UBSTalentEffect* TalentCDO = GetDefault<UBSTalentEffect>(TalentData);
		TalentIconImage->SetBrushFromLazyTexture(TalentCDO->TalentIcon);
		
		if (const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>())
		{
			UBSWToolTipBase* ToolTip = UBSWToolTipBase::CreateGenericToolTip(GetOwningPlayer(),TalentCDO->TalentName, TalentCDO->Description, FText::FromString("Talent"));
			SetToolTip(ToolTip);
			
			UpdateTalentWidget();
			PS->GetAbilitySystemComponent()->OnActiveGameplayEffectAddedDelegateToSelf.AddWeakLambda(this, [this, PS]
				(UAbilitySystemComponent* Asc, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
			{
				PS->GetAbilitySystemComponent()->OnGameplayEffectStackChangeDelegate(Handle)->AddWeakLambda(this, [this, PS] (FActiveGameplayEffectHandle EffectHandle, int32 Old, int32 New)
				{
					UpdateTalentWidget();
				});
				UpdateTalentWidget();
			});
			
			PS->GetAbilitySystemComponent()->OnAnyGameplayEffectRemovedDelegate().AddWeakLambda(
				this, [this, PS](const FActiveGameplayEffect& ActiveEffect)
			{
					UpdateTalentWidget();
			}); 
		}
	}

}

void UBSWTalentNode::RemoveFromParent()
{
	if (const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>())
	{
		PS->GetAbilitySystemComponent()->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		PS->GetAbilitySystemComponent()->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	}
	
	Super::RemoveFromParent();
}

void UBSWTalentNode::UpdateTalentWidget()
{
	if (ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>())
	{

		const int32 FoundLevel = PS->GetTalentComponent()->GetTalentLevel(TalentData);
		UpdateColor(PS, FoundLevel);

		UpdateText(FoundLevel);
	}
}

void UBSWTalentNode::UpdateColor(ABSPlayerState* PS, int32 Level)
{
	if (PS->GetTalentComponent()->IsMaxLevel(TalentData))
	{
		const FLinearColor MaxedColor = FLinearColor::Green;
		SetColorAndOpacity(MaxedColor);
	}
	else
	{
		constexpr FLinearColor DisabledColor = FLinearColor(0.2f, 0.2f, 0.2f);
		SetColorAndOpacity(PS->GetTalentComponent()->CanUnlockOrUpgradeTalent(TalentData) ? FLinearColor::White : DisabledColor);
	}
}

void UBSWTalentNode::UpdateText(int32 Level)
{
	const UBSTalentEffect* TalentCDO = GetDefault<UBSTalentEffect>(TalentData);
	const FString CurrentLevel = FString::FromInt(Level);
	const FString MaxLevel = FString::FromInt(TalentCDO->GetMaxLevel());
	LevelText->SetText(FText::FromString(CurrentLevel + "/" + MaxLevel));
	
}

void UBSWTalentNode::NativeOnPressed()
{
	Super::NativeOnPressed();
	if (const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>())
	{
		if (PS->GetTalentComponent()->CanUnlockOrUpgradeTalent(TalentData))
		{
			PS->GetTalentComponent()->Server_LevelUpTalent(TalentData);
		}
	}
}
