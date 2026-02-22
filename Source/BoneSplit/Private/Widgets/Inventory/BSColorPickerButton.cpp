// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Inventory/BSColorPickerButton.h"

#include "Actors/Player/BSPlayerState.h"
#include "GameSettings/BSDeveloperSettings.h"

void UBSColorPickerButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>())
	{
		if (DeveloperSettings->PlayerColors.IsValidIndex(ColorIndex))
		{
			SetColorAndOpacity(FLinearColor::FromSRGBColor(DeveloperSettings->PlayerColors[ColorIndex]));
		}
	}
}

void UBSColorPickerButton::NativeOnClicked()
{
	Super::NativeOnClicked();
	
	if (ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>())
	{
		if (const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>())
		{
			if (DeveloperSettings->PlayerColors.IsValidIndex(ColorIndex))
			{
				PS->GetInventoryComponent()->Server_SetPlayerColor(ColorIndex);
			}
		}
	}
}
