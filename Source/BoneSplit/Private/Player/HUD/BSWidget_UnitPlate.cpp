// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSWidget_UnitPlate.h"

#include "CommonRichTextBlock.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Player/HUD/BSWidget_AttributeBar.h"

void UBSWidget_UnitPlate::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (NameText) NameText->SetVisibility(ESlateVisibility::Hidden);
	if (HealthBar) HealthBar->SetVisibility(ESlateVisibility::Hidden);
}

void UBSWidget_UnitPlate::NativeConstruct()
{
	Super::NativeConstruct();

}

void UBSWidget_UnitPlate::SetOwningAsc(UAbilitySystemComponent* InAsc)
{
	HealthBar->SetVisibility(ESlateVisibility::HitTestInvisible);
	HealthBar->SetAbilitySystemToTrack(
		UBSExtendedAttributeSet::GetCurrentHealthAttribute(), UBSExtendedAttributeSet::GetMaxHealthAttribute(), InAsc);
}

void UBSWidget_UnitPlate::SetUnitName(const FText& InName)
{
	NameText->SetVisibility(ESlateVisibility::HitTestInvisible);
	NameText->SetText(InName);
}
