// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSWToolTipBase.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"

void UBSWToolTipBase::SetToolTipText(const FText Header, const FText Text, const FText AltHeader)
{
	ToolTipHeader->SetText(Header);
	ToolTipTextBlock->SetText(Text);
	ToolTipAltHeader->SetText(AltHeader);
}

void UBSWToolTipBase::SetToolTipImage(const TSoftObjectPtr<UTexture2D> InBrush)
{
	ToolTipIcon->SetBrushFromLazyTexture(InBrush);
	ToolTipIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UBSWToolTipBase::SetToolTipAltImage(const TSoftObjectPtr<UTexture2D> InBrush)
{
	ToolTipIconAlt->SetBrushFromLazyTexture(InBrush);
	ToolTipIconAlt->SetVisibility(ESlateVisibility::HitTestInvisible);
}
