// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWidget_SettingsRoot.generated.h"

class UCommonActivatableWidgetSwitcher;
class UBSWidget_GraphicsSettings;
class UBSWidget_AudioSettings;
class UBSWidget_ControlsSettings;
class UBSWidget_GameplaySettings;
class UCommonButtonBase;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_SettingsRoot : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnActivated() override;
	
	virtual void NativeOnDeactivated() override;
	
protected:
	
	UPROPERTY(meta=(BindWidget))
	UCommonActivatableWidgetSwitcher* Switcher_SettingsTab;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_GameplaySettings;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_GameplaySettings* GameplayTab;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_ControlsSettings;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_ControlsSettings* ControlsTab;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_AudioSettings;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_AudioSettings* AudioTab;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_GraphicsSettings;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_GraphicsSettings* GraphicsTab;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_Apply;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_Revert;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_Return;
};
