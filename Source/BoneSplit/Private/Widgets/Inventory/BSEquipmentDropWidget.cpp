// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Inventory/BSEquipmentDropWidget.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Components/Inventory/BSEquipment.h"

void UBSEquipmentDropWidget::InitializeEquipmentDropWidget(const UBSEquipmentEffect* EquipmentEffectCDO)
{
	EquipmentNameText->SetText(EquipmentEffectCDO->EquipmentName);
	EquipmentDescriptionText->SetText(FText::FromString("PlaceHolder"));
	EquipmentIcon->SetBrushFromLazyTexture(EquipmentEffectCDO->Icon);
}
