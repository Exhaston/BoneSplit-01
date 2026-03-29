// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWidget_ControlsSettings.generated.h"

class UBSWidget_IntSelector;
class UBSGameUserSettings;
class UBSWidget_Slider;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_ControlsSettings : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnActivated() override;
	
	virtual void NativeOnDeactivated() override;
	
	virtual void NativeOnSettingsRefreshed();
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_LookSensitivityX;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_LookSensitivityY;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_InvertY;
	
	UPROPERTY(Transient)
	UBSGameUserSettings* UserSettings;
};
