// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSToggleButton.h"

#include "CommonLazyImage.h"

void UBSToggleButton::SetState(const bool bTrue)
{
	CheckedImage->SetVisibility(bTrue ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}
