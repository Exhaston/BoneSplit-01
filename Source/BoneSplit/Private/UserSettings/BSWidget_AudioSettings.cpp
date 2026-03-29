// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_AudioSettings.h"

#include "Components/Slider.h"
#include "UserSettings/BSGameUserSettings.h"
#include "UserSettings/BSWidget_IntSelector.h"
#include "UserSettings/BSWidget_Slider.h"

void UBSWidget_AudioSettings::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	UserSettings = UBSGameUserSettings::GetBSGameUserSettings();
	
	UserSettings->OnSettingsUpdatedDelegate.AddUObject(this, &UBSWidget_AudioSettings::NativeOnSettingsRefreshed);
	
	Slider_OverallVolume->Slider_Value->SetStepSize(0.05f);
	Slider_OverallVolume->Slider_Value->SetMinValue(0.f);
	Slider_OverallVolume->Slider_Value->SetMaxValue(1.f);
	Slider_OverallVolume->OnSliderValueChanged.AddWeakLambda(this, [this] (float NewValue)
	{
		UserSettings->SetOverallVolume(NewValue);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	Slider_EffectsVolume->Slider_Value->SetStepSize(0.05f);
	Slider_EffectsVolume->Slider_Value->SetMinValue(0.f);
	Slider_EffectsVolume->Slider_Value->SetMaxValue(1.f);
	Slider_EffectsVolume->OnSliderValueChanged.AddWeakLambda(this, [this] (float NewValue)
	{
		UserSettings->SetEffectsVolume(NewValue);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	Slider_MusicVolume->Slider_Value->SetStepSize(0.05f);
	Slider_MusicVolume->Slider_Value->SetMinValue(0.f);
	Slider_MusicVolume->Slider_Value->SetMaxValue(1.f);
	Slider_MusicVolume->OnSliderValueChanged.AddWeakLambda(this, [this] (float NewValue)
	{
		UserSettings->SetMusicVolume(NewValue);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	IntSelector_AudioQuality->SetRange(0, 3);
	IntSelector_AudioQuality->SetDisplayTexts({
		NSLOCTEXT("Audio", "Low", "Low"),
		NSLOCTEXT("Audio", "Medium", "Medium"),
		NSLOCTEXT("Audio", "High", "High"),
		NSLOCTEXT("Audio", "Highest", "Highest")
	});
	
	IntSelector_AudioQuality->GetOnValueChangedDelegate().AddWeakLambda(this, [this] (int32 NewValue)
	{
		UserSettings->SetAudioQualityLevel(NewValue);
	});
	
	NativeOnSettingsRefreshed();
}

void UBSWidget_AudioSettings::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	Slider_OverallVolume->OnSliderValueChanged.Clear();
	Slider_EffectsVolume->OnSliderValueChanged.Clear();
	Slider_MusicVolume->OnSliderValueChanged.Clear();
	IntSelector_AudioQuality->GetOnValueChangedDelegate().Clear();
	
	UserSettings->OnSettingsUpdatedDelegate.RemoveAll(this);
}

void UBSWidget_AudioSettings::NativeOnSettingsRefreshed()
{
	IntSelector_AudioQuality->SetValue(UserSettings->GetAudioQualityLevel());
	Slider_MusicVolume->SetSliderValue(UserSettings->GetMusicVolume());
	Slider_OverallVolume->SetSliderValue(UserSettings->GetOverallVolume());
	Slider_EffectsVolume->SetSliderValue(UserSettings->GetEffectsVolume());
}
