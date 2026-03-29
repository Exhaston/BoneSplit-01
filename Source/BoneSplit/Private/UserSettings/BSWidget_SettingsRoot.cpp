// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_SettingsRoot.h"

#include "CommonActivatableWidgetSwitcher.h"
#include "CommonButtonBase.h"
#include "UserSettings/BSGameUserSettings.h"
#include "UserSettings/BSWidget_AudioSettings.h"
#include "UserSettings/BSWidget_ControlsSettings.h"
#include "UserSettings/BSWidget_GameplaySettings.h"
#include "UserSettings/BSWidget_GraphicsSettings.h"


void UBSWidget_SettingsRoot::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	bIsBackHandler = true;
	
	Switcher_SettingsTab->SetActiveWidget(GameplayTab);
	
	Button_GameplaySettings->OnClicked().AddWeakLambda(this, [this]()
	{
		 Switcher_SettingsTab->SetActiveWidget(GameplayTab);
	});	
	
	Button_ControlsSettings->OnClicked().AddWeakLambda(this, [this]()
	{
		 Switcher_SettingsTab->SetActiveWidget(ControlsTab);
	});	
	
	Button_AudioSettings->OnClicked().AddWeakLambda(this, [this]()
	{
		 Switcher_SettingsTab->SetActiveWidget(AudioTab);
	});	
	
	Button_GraphicsSettings->OnClicked().AddWeakLambda(this, [this]()
	{
		 Switcher_SettingsTab->SetActiveWidget(GraphicsTab);
	});	
	
	Button_Return->OnClicked().AddWeakLambda(this, [this]()
	{
		UBSGameUserSettings* GameUserSettings = UBSGameUserSettings::GetBSGameUserSettings();
		GameUserSettings->ResetToCurrentSettings();
		DeactivateWidget();
	});
	
	Button_Apply->OnClicked().AddWeakLambda(this, [this]()
	{
		UBSGameUserSettings* GameUserSettings = UBSGameUserSettings::GetBSGameUserSettings();
		GameUserSettings->ApplySettings(false);
	});
	
	Button_Revert->OnClicked().AddWeakLambda(this, [this]()
	{
		UBSGameUserSettings* GameUserSettings = UBSGameUserSettings::GetBSGameUserSettings();
		GameUserSettings->SetToDefaults();
		GameUserSettings->ApplySettings(false);
	});
}

void UBSWidget_SettingsRoot::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	Button_GameplaySettings->OnClicked().Clear();
	Button_ControlsSettings->OnClicked().Clear();
	Button_AudioSettings->OnClicked().Clear();
	Button_GraphicsSettings->OnClicked().Clear();
	Button_Return->OnClicked().Clear();
	Button_Apply->OnClicked().Clear();
	Button_Revert->OnClicked().Clear();
}
