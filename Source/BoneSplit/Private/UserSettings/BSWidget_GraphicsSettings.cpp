// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_GraphicsSettings.h"

#include "Components/Slider.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UserSettings/BSGameUserSettings.h"
#include "UserSettings/BSWidget_IntSelector.h"
#include "UserSettings/BSWidget_Slider.h"

void UBSWidget_GraphicsSettings::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	GameUserSettings = UBSGameUserSettings::GetBSGameUserSettings();
	
	GameUserSettings->OnSettingsUpdatedDelegate.AddUObject(
		this, &UBSWidget_GraphicsSettings::NativeOnSettingsRefreshed);
	
	InitResolution();
	InitDisplaySelection();
	InitWindowMode();
	InitRenderScaleSlider();
	InitFrameRateLocking();
	InitVsync();
	InitFieldOfView();
	InitQualitySelectors();
	
	IntSelector_AntiAliasingMode->SetRange(0, 5);
	IntSelector_AntiAliasingMode->SetDisplayTexts({
		NSLOCTEXT("AntiAliasingMode", "None", "None"),
		NSLOCTEXT("AntiAliasingMode", "FXAA", "FXAA"),
		NSLOCTEXT("AntiAliasingMode", "TAA", "TAA"),
		NSLOCTEXT("AntiAliasingMode", "MSAA", "MSAA"),
		NSLOCTEXT("AntiAliasingMode", "TSR", "TSR"),
		NSLOCTEXT("AntiAliasingMode", "SMAA", "SMAA")
	});
	
	IntSelector_AntiAliasingMode->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		GameUserSettings->SetAntiAliasingMode(NewValue);
		GameUserSettings->ApplyNonResolutionSettings();
	});
	
	NativeOnSettingsRefreshed();
}

void UBSWidget_GraphicsSettings::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	GameUserSettings->OnSettingsUpdatedDelegate.RemoveAll(this);
	
	IntSelector_AntiAliasingMode->GetOnValueChangedDelegate().Clear();
	IntSelector_Resolution->GetOnValueChangedDelegate().Clear();
	IntSelector_LockFrameRate->GetOnValueChangedDelegate().Clear();
	IntSelector_Vsync->GetOnValueChangedDelegate().Clear();
	IntSelector_WindowMode->GetOnValueChangedDelegate().Clear();
	
	Slider_FrameRate->OnSliderValueChanged.Clear();
	Slider_RenderScale->OnSliderValueChanged.Clear();
	Slider_FieldOfView->OnSliderValueChanged.Clear();
	
	IntSelector_OverallQuality->GetOnValueChangedDelegate().Clear();
	IntSelector_AntiAliasingQuality->GetOnValueChangedDelegate().Clear();
	IntSelector_Foliage->GetOnValueChangedDelegate().Clear();
	IntSelector_Shadows->GetOnValueChangedDelegate().Clear();
	IntSelector_Textures->GetOnValueChangedDelegate().Clear();
	IntSelector_ViewDistance->GetOnValueChangedDelegate().Clear();
	
	IntSelector_MonitorSelect->GetOnValueChangedDelegate().Clear();
}

void UBSWidget_GraphicsSettings::InitFieldOfView()
{
	Slider_FieldOfView->Slider_Value->SetMinValue(60.f);
	Slider_FieldOfView->Slider_Value->SetMaxValue(120.f);
	Slider_FieldOfView->Slider_Value->SetStepSize(5.f);
	
	Slider_FieldOfView->OnSliderValueChanged.AddWeakLambda(this, [this](const float NewValue)
	{
		GameUserSettings->SetFieldOfView(NewValue);
		GameUserSettings->ApplyNonResolutionSettings();
	});
}

void UBSWidget_GraphicsSettings::InitResolution()
{
	//IntSelector_Resolution->SetVisibility(GameUserSettings->GetFullscreenMode() != EWindowMode::Type::Fullscreen ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	SupportedResolutions.Empty();
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);

	// Fallback if driver returns nothing
	if (SupportedResolutions.IsEmpty())
	{
		SupportedResolutions = { {1280,720}, {1920,1080}, {2560,1440}, {3840,2160} };
	}

	TArray<FText> ResTexts;
	for (const FIntPoint& Res : SupportedResolutions)
	{
		ResTexts.Add(FText::FromString(FString::Printf(TEXT("%d x %d"), Res.X, Res.Y)));
	}

	IntSelector_Resolution->SetRange(0, SupportedResolutions.Num() - 1);
	IntSelector_Resolution->SetDisplayTexts(ResTexts);
	
	IntSelector_Resolution->GetOnValueChangedDelegate().Clear();
	IntSelector_Resolution->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		if (SupportedResolutions.IsValidIndex(NewValue))
		{
			GameUserSettings->SetScreenResolution(SupportedResolutions[NewValue]);
			GameUserSettings->ApplyResolutionSettings(false);
		}
	});


}

void UBSWidget_GraphicsSettings::InitDisplaySelection()
{
	const TArray<FMonitorInfo> MonitorInfos = GameUserSettings->GetAvailableMonitors();
	IntSelector_MonitorSelect->SetRange(0, MonitorInfos.Num() - 1);
	
	TArray<FText> MonitorIDs;
	
	for (int32 i = 0; i < MonitorInfos.Num(); i++)
	{
		MonitorIDs.Add(FText::FromString(MonitorInfos[i].Name));
	}
	
	IntSelector_MonitorSelect->SetDisplayTexts(MonitorIDs);
	
	IntSelector_MonitorSelect->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		GameUserSettings->SetDisplayId(NewValue);
		if (GameUserSettings->SetActiveDisplay(NewValue))
		{
			InitResolution();
		}

		GameUserSettings->ApplyNonResolutionSettings();
	});
}

void UBSWidget_GraphicsSettings::InitWindowMode()
{
	IntSelector_WindowMode->SetRange(0, 2);
	IntSelector_WindowMode->SetDisplayTexts({
		NSLOCTEXT("Graphics", "Fullscreen", "Fullscreen"),
		NSLOCTEXT("Graphics", "Borderless", "Borderless"),
		NSLOCTEXT("Graphics", "Windowed", "Windowed")
	});
	
	IntSelector_WindowMode->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		GameUserSettings->SetFullscreenMode(EWindowMode::ConvertIntToWindowMode(NewValue));
		InitResolution(); // re-query valid resolutions for this window mode
		GameUserSettings->ApplyResolutionSettings(false);
	});
}

void UBSWidget_GraphicsSettings::InitVsync()
{
	IntSelector_Vsync->SetRange(0, 1);
	IntSelector_Vsync->SetDisplayTexts({
		NSLOCTEXT("Graphics", "Off", "Off"),
		NSLOCTEXT("Graphics", "On",  "On")
	});
	
	
	IntSelector_Vsync->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		GameUserSettings->SetVSyncEnabled(NewValue == 1);
		GameUserSettings->ApplyNonResolutionSettings();
	});
}

void UBSWidget_GraphicsSettings::InitRenderScaleSlider()
{
	
	Slider_RenderScale->Slider_Value->SetMinValue(30.f);
	Slider_RenderScale->Slider_Value->SetMaxValue(100.f);
	Slider_RenderScale->Slider_Value->SetStepSize(5.f);
	
	Slider_RenderScale->OnSliderValueChanged.AddWeakLambda(this, [this](const float NewValue)
	{
		const float Normalized = NewValue * 0.01f;
		GameUserSettings->SetResolutionScaleNormalized(Normalized);
		GameUserSettings->ApplyNonResolutionSettings();
	});
}

void UBSWidget_GraphicsSettings::InitFrameRateLocking()
{
	IntSelector_LockFrameRate->SetRange(0, 1);
	IntSelector_LockFrameRate->SetDisplayTexts({
		NSLOCTEXT("Graphics", "Unlocked", "Unlocked"),
		NSLOCTEXT("Graphics", "Locked", "Locked")
	});
	
	
	IntSelector_LockFrameRate->GetOnValueChangedDelegate().AddWeakLambda(this, [this](int NewValue)
	{
		GameUserSettings->SetFrameRateLocked(NewValue == 1);
    
		if (NewValue == 1)
		{
			// Switching to Locked - use slider's current value as the frame rate limit
			float CurrentSliderValue = Slider_FrameRate->Slider_Value->GetValue();
			GameUserSettings->SetFrameRateLimit(CurrentSliderValue);
		}
		else
		{
			// Switching to Unlocked - set -1
			GameUserSettings->SetFrameRateLimit(-1);
		}
    
		Slider_FrameRate->Slider_Value->SetLocked(NewValue == 0);
		GameUserSettings->ApplyNonResolutionSettings();
	});
	
	Slider_FrameRate->Slider_Value->SetMinValue(30);
	Slider_FrameRate->Slider_Value->SetMaxValue(400);
	Slider_FrameRate->Slider_Value->SetStepSize(5.f);
	Slider_FrameRate->OnSliderValueChanged.AddWeakLambda(this, [this](float NewValue)
	{
		GameUserSettings->SetFrameRateLimit(NewValue);
		GameUserSettings->ApplyNonResolutionSettings();
	});
}

void UBSWidget_GraphicsSettings::NativeOnSettingsRefreshed()
{
	// Find the index matching the current resolution
	const FIntPoint CurrentRes = GameUserSettings->GetScreenResolution();
	const int32 CurrentIndex = SupportedResolutions.IndexOfByPredicate(
		[&CurrentRes](const FIntPoint& R){ return R == CurrentRes; });

	IntSelector_Resolution->SetValue(CurrentIndex != INDEX_NONE ? CurrentIndex : 0);
	
	// EWindowMode::Fullscreen=0, WindowedFullscreen=1, Windowed=2
	const int32 CurrentMode = GameUserSettings->GetFullscreenMode();
	IntSelector_WindowMode->SetValue(CurrentMode);
	
	Slider_FieldOfView->SetSliderValue(GameUserSettings->GetFieldOfView());
	
	IntSelector_Vsync->SetValue(GameUserSettings->IsVSyncEnabled() ? 1 : 0);
	
	static const auto CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
	float CurrentScreenPct = CVarScreenPercentage->GetFloat();
	Slider_RenderScale->SetSliderValue(CurrentScreenPct);
	IntSelector_LockFrameRate->SetValue(GameUserSettings->GetFrameRateLocked() ? 1 : 0);
	IntSelector_AntiAliasingMode->SetValue(GameUserSettings->GetAntiAliasingMode());
	Slider_FrameRate->SetSliderValue(GameUserSettings->GetFrameRateLocked() ? GameUserSettings->GetFrameRateLimit() : 30);
	Slider_FrameRate->Slider_Value->SetLocked(!GameUserSettings->GetFrameRateLocked());
	IntSelector_OverallQuality->SetValue(GameUserSettings->GetOverallScalabilityLevel());
	IntSelector_AntiAliasingQuality->SetValue(GameUserSettings->GetAntiAliasingQuality());
	IntSelector_Foliage->SetValue(GameUserSettings->GetFoliageQuality());
	IntSelector_Shadows->SetValue(GameUserSettings->GetShadowQuality());
	IntSelector_Textures->SetValue(GameUserSettings->GetTextureQuality());
	IntSelector_ViewDistance->SetValue(GameUserSettings->GetViewDistanceQuality());
	
	IntSelector_MonitorSelect->SetValue(GameUserSettings->GetDisplayId());
}

void UBSWidget_GraphicsSettings::InitQualitySelectors()
{
	constexpr int32 Min = 0;
	constexpr int32 Max = 3;
	
	const TArray QualityTexts = {
		NSLOCTEXT("Graphics", "Low", "Low"),
		NSLOCTEXT("Graphics", "Medium", "Medium"),
		NSLOCTEXT("Graphics", "High", "High"),
		NSLOCTEXT("Graphics", "Highest", "Highest"),
	};
	
	IntSelector_OverallQuality->SetRange(Min, Max);
	IntSelector_OverallQuality->SetDisplayTexts(QualityTexts);
	IntSelector_OverallQuality->GetOnValueChangedDelegate().AddWeakLambda(this,[this](int NewValue)
	{
		GameUserSettings->SetOverallScalabilityLevel(NewValue);
	});
	
	IntSelector_ViewDistance->SetRange(Min, Max);
	IntSelector_ViewDistance->SetDisplayTexts(QualityTexts);
	IntSelector_ViewDistance->GetOnValueChangedDelegate().AddWeakLambda(this,[this](int NewValue)
	{
		GameUserSettings->SetViewDistanceQuality(NewValue);
	});
	
	IntSelector_Textures->SetRange(Min, Max);
	IntSelector_Textures->SetDisplayTexts(QualityTexts);
	IntSelector_Textures->GetOnValueChangedDelegate().AddWeakLambda(this,[this](int NewValue)
	{
		GameUserSettings->SetTextureQuality(NewValue);
	});
	
	IntSelector_AntiAliasingQuality->SetRange(Min, Max);
	IntSelector_AntiAliasingQuality->SetDisplayTexts(QualityTexts);
	IntSelector_AntiAliasingQuality->GetOnValueChangedDelegate().AddWeakLambda(this,[this](int NewValue)
	{
		GameUserSettings->SetAntiAliasingQuality(NewValue);
	});
	
	IntSelector_Shadows->SetRange(Min, Max);
	IntSelector_Shadows->SetDisplayTexts(QualityTexts);
	IntSelector_Shadows->GetOnValueChangedDelegate().AddWeakLambda(this,[this](int NewValue)
	{
		GameUserSettings->SetShadowQuality(NewValue);
	});
	
	IntSelector_Foliage->SetRange(Min, Max);
	IntSelector_Foliage->SetDisplayTexts(QualityTexts);
	IntSelector_Foliage->GetOnValueChangedDelegate().AddWeakLambda(this,[this](int NewValue)
	{
		GameUserSettings->SetFoliageQuality(NewValue);
	});
}
