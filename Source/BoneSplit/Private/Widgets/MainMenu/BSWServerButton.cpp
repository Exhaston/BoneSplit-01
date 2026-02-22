// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSWServerButton.h"

#include "CommonRichTextBlock.h"

void UBSWServerButton::InitializeServerButton(const FText InSessionName, const FText InSessionUserCount, const FText InSessionPingText)
{
	SessionNameText->SetText(InSessionName);
	SessionCountText->SetText(InSessionUserCount);
	SessionPingText->SetText(InSessionPingText);
}
