// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_GameplaySettings.h"

#include "UserSettings/BSGameUserSettings.h"
#include "UserSettings/BSWidget_IntSelector.h"

void UBSWidget_GameplaySettings::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	UserSettings = UBSGameUserSettings::GetBSGameUserSettings();
	
	IntSelector_CameraOffset->SetRange(0, 4);
	IntSelector_CameraOffset->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		UserSettings->SetCameraDistanceValue(NewValue);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	IntSelector_ShowOwnName->SetRange(0, 1);
	IntSelector_ShowOwnName->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		UserSettings->SetShowOwnName(NewValue == 1);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	IntSelector_ShowOtherNames->SetRange(0, 1);
	IntSelector_ShowOtherNames->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		UserSettings->SetShowOtherNames(NewValue == 1);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	IntSelector_ShowValues->SetRange(0, 1);
	IntSelector_ShowValues->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		UserSettings->SetShowValues(NewValue == 1);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	IntSelector_ShowPercentages->SetRange(0, 1);
	IntSelector_ShowPercentages->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		UserSettings->SetShowPercentages(NewValue == 1);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	IntSelector_DisplayPerformance->SetRange(0, 1);
	IntSelector_DisplayPerformance->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		UserSettings->SetPerformanceDisplay(NewValue == 1);
		UserSettings->ApplyNonResolutionSettings();
	});
	
	UserSettings->OnSettingsUpdatedDelegate.AddUObject(this, &UBSWidget_GameplaySettings::NativeOnSettingsRefreshed);
	NativeOnSettingsRefreshed();
}

void UBSWidget_GameplaySettings::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	IntSelector_CameraOffset->GetOnValueChangedDelegate().Clear();
	IntSelector_ShowOwnName->GetOnValueChangedDelegate().Clear();
	IntSelector_ShowOtherNames->GetOnValueChangedDelegate().Clear();
	IntSelector_ShowValues->GetOnValueChangedDelegate().Clear();
	IntSelector_ShowPercentages->GetOnValueChangedDelegate().Clear();
	IntSelector_DisplayPerformance->GetOnValueChangedDelegate().Clear();
	
	UserSettings->OnSettingsUpdatedDelegate.RemoveAll(this);
	
}

void UBSWidget_GameplaySettings::NativeOnSettingsRefreshed()
{
	IntSelector_ShowPercentages->SetValue(UserSettings->GetShowPercentages() ? 1 : 0);
	IntSelector_ShowValues->SetValue(UserSettings->GetShowValues() ? 1 : 0);
	IntSelector_ShowOtherNames->SetValue(UserSettings->GetShowOtherNames() ? 1 : 0);
	IntSelector_ShowOwnName->SetValue(UserSettings->GetShowOwnName() ? 1 : 0);
	IntSelector_CameraOffset->SetValue(UserSettings->GetCameraDistanceValue());
	IntSelector_DisplayPerformance->SetValue(UserSettings->GetPerformanceDisplay() ? 1 : 0);
}
