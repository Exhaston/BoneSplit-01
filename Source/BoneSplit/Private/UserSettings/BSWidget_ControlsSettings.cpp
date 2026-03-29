// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_ControlsSettings.h"

#include "Components/Slider.h"
#include "UserSettings/BSGameUserSettings.h"
#include "UserSettings/BSWidget_IntSelector.h"
#include "UserSettings/BSWidget_Slider.h"

void UBSWidget_ControlsSettings::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	UserSettings = UBSGameUserSettings::GetBSGameUserSettings();
	
	UserSettings->OnSettingsUpdatedDelegate.AddUObject(this, &UBSWidget_ControlsSettings::NativeOnSettingsRefreshed);
	
	Slider_LookSensitivityX->Slider_Value->SetStepSize(0.05f);
	Slider_LookSensitivityX->Slider_Value->SetMinValue(0.1f);
	Slider_LookSensitivityX->Slider_Value->SetMaxValue(2.f);
	Slider_LookSensitivityX->OnSliderValueChanged.AddWeakLambda(this, [this] (float NewValue)
	{
		UserSettings->SetLookSensitivityHorizontal(NewValue);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	Slider_LookSensitivityY->Slider_Value->SetStepSize(0.05f);
	Slider_LookSensitivityY->Slider_Value->SetMinValue(0.1f);
	Slider_LookSensitivityY->Slider_Value->SetMaxValue(2.f);
	Slider_LookSensitivityY->OnSliderValueChanged.AddWeakLambda(this, [this] (float NewValue)
	{
		UserSettings->SetLookSensitivityVertical(NewValue);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	IntSelector_InvertY->SetRange(0, 1);
	IntSelector_InvertY->SetDisplayTexts({
		NSLOCTEXT("Controls", "Normal", "Normal"),
		NSLOCTEXT("Controls", "Inverted", "Inverted")
	});
	IntSelector_InvertY->GetOnValueChangedDelegate().AddWeakLambda(this, [this] (int NewValue)
	{
		UserSettings->SetInvertVerticalAxis(NewValue == 1);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	NativeOnSettingsRefreshed();
}

void UBSWidget_ControlsSettings::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	Slider_LookSensitivityY->OnSliderValueChanged.Clear();
	Slider_LookSensitivityX->OnSliderValueChanged.Clear();
	IntSelector_InvertY->GetOnValueChangedDelegate().Clear();
	
	UserSettings->OnSettingsUpdatedDelegate.RemoveAll(this);
}

void UBSWidget_ControlsSettings::NativeOnSettingsRefreshed()
{
	Slider_LookSensitivityX->SetSliderValue(UserSettings->GetLookSensitivityHorizontal());
	Slider_LookSensitivityY->SetSliderValue(UserSettings->GetLookSensitivityVertical());
	IntSelector_InvertY->SetValue(UserSettings->GetInvertVerticalAxis());
}
