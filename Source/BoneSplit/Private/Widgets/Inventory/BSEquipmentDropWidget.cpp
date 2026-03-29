// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Inventory/BSEquipmentDropWidget.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Player/Equipment/BSEquipmentInstance.h"

void UBSEquipmentDropWidget::InitializeEquipmentDropWidget(const UBSEquipmentEffect* EquipmentEffectCDO)
{
	/*
	EquipmentNameText->SetText(EquipmentEffectCDO->EquipmentName);
	EquipmentDescriptionText->SetText(FText::FromString("PlaceHolder"));
	EquipmentIcon->SetBrushFromLazyTexture(EquipmentEffectCDO->Icon);
	*/
}

void UBSEquipmentDropWidget::InitializeEquipmentDropWidget(const FBSEquipmentInstance EquipmentInstance)
{
	/*
	EquipmentNameText->SetText(EquipmentInstance.GetDef()->EquipmentName);
	EquipmentDescriptionText->SetText(EquipmentInstance.GetDef()->EquipmentDescription);
	EquipmentIcon->SetBrushFromLazyTexture(EquipmentInstance.GetDef()->EquipmentIcon);
	EquipmentIcon->SetBrushTintColor(EquipmentInstance.GetDef()->IconTint);
	EquipmentLevel->SetText(FText::AsNumber(EquipmentInstance.GetEquipmentLevel()));
	*/
}
