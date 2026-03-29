// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSWidget_PlayerFrame.h"

#include "CommonRichTextBlock.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Player/HUD/BSWidget_AttributeBar.h"

void UBSWidget_PlayerFrame::SetPlayerAbilitySystem(UAbilitySystemComponent* InAbilitySystem, const FString PlayerName)
{
	PlayerNameText->SetText(FText::FromString(PlayerName));
	HealthBar->SetAbilitySystemToTrack(UBSExtendedAttributeSet::GetCurrentHealthAttribute(), UBSExtendedAttributeSet::GetMaxHealthAttribute(), InAbilitySystem);
	ManaBar->SetAbilitySystemToTrack(UBSExtendedAttributeSet::GetCurrentManaAttribute(), UBSExtendedAttributeSet::GetMaxManaAttribute(), InAbilitySystem);
}
