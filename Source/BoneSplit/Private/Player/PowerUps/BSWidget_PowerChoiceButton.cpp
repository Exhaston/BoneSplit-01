// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/PowerUps/BSWidget_PowerChoiceButton.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Player/PowerUps/BSPowerUpData.h"

void UBSWidget_PowerChoiceButton::InitializeButton(UBSPowerUpData* InPowerUpData)
{
	if (!InPowerUpData) return;
	PowerUpData = InPowerUpData;

	// Icon
	if (PowerUpIcon && !PowerUpData->PowerUpIcon.IsNull())
	{
		PowerUpIcon->SetBrushFromLazyTexture(PowerUpData->PowerUpIcon);
		PowerUpIcon->SetColorAndOpacity(InPowerUpData->PowerUpIconTint);
	}

	// Info text
	if (PowerUpInfo)
	{
		PowerUpInfo->SetText(InPowerUpData->PowerUpInfo);
	}
	
	if (PowerUpName)
	{
		PowerUpName->SetText(InPowerUpData->PowerUpName);
	}
	
	PowerUpRaritySet(PowerUpData->PowerUpRarity);
}

void UBSWidget_PowerChoiceButton::PowerUpRaritySet_Implementation(EBSPowerUpRarity OutRarity)
{
}
