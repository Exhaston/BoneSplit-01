// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSWToolTipBase.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

UBSWToolTipBase* UBSWToolTipBase::CreateGenericToolTip(APlayerController* OwningPlayer, const FText& Header,
                                                       const FText& Text, const FText& AltHeader)
{
	const UBSLocalWidgetSubsystem* LocalWidgetSubsystem = OwningPlayer->GetGameInstance()->GetSubsystem<UBSLocalWidgetSubsystem>();
	UBSWToolTipBase* ToolTip = CreateWidget<UBSWToolTipBase>(OwningPlayer, nullptr);
	ToolTip->SetToolTipText(Header, Text, AltHeader);
	return ToolTip;
}

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
