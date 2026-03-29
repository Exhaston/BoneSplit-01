// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSGameUserSettings.h"

#include "Match/BSOnlineSettings.h"
#include "Player/BSPlayerCharacterBase.h"
#include "Player/BSPlayerControllerBase.h"
#include "Player/HUD/BSWidget_AttributeBar.h"
#include "Sound/SoundClass.h"
#include "UserSettings/BSWidget_PerformanceDisplay.h"

TArray<FMonitorInfo> UBSGameUserSettings::GetAvailableMonitors()
{
	TArray<FMonitorInfo> Monitors;
	if (FDisplayMetrics Metrics; FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetDisplayMetrics(Metrics);
		Monitors = Metrics.MonitorInfo;
	}
	return Monitors;
}

void UBSGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	
	SetActiveDisplay(GetDisplayId());
	
	ApplyCVars();
}

void UBSGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	
	ApplyCVars();
	
	OnSettingsUpdatedDelegate.Broadcast();
}

void UBSGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
}

void UBSGameUserSettings::ApplyCVars()
{
	BSConsoleVariables::CVarBSShowOwnName->Set(bShowOwnName, ECVF_SetByCode);
	BSConsoleVariables::CVarBSShowOtherNames->Set(bShowOtherNames, ECVF_SetByCode);
	
	BSConsoleVariables::CVarBSCameraOffset->Set(
		GetAvailableCameraDistances()[CameraDistanceValue], ECVF_SetByCode);
	
	BSConsoleVariables::CVarBSBarsShowNumbers->Set(GetShowValues(), ECVF_SetByCode);
	BSConsoleVariables::CVarBSBarsShowPercentages->Set(GetShowPercentages(), ECVF_SetByCode);
	
	static const auto CVarAAMethod = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
	CVarAAMethod->Set(AntiAliasingMode, ECVF_SetByCode);
	
	BSConsoleVariables::CVarBSLookSensitivityX->Set(GetLookSensitivityHorizontal(), ECVF_SetByCode);
	BSConsoleVariables::CVarBSLookSensitivityY->Set(GetLookSensitivityVertical(), ECVF_SetByCode);
	
	BSConsoleVariables::CVarBSCameraFieldOfView->Set(GetFieldOfView(), ECVF_SetByCode);
	BSConsoleVariables::CVarInvertLookY->Set(GetInvertVerticalAxis(), ECVF_SetByCode);
	BSConsoleVariables::CVarBSEnablePerf->Set(GetPerformanceDisplay(), ECVF_SetByCode);
	
	if (!GetBSOnlineSettings()->MasterSoundClass.IsNull())
	{
		GetBSOnlineSettings()->MasterSoundClass.LoadSynchronous()->Properties.Volume = GetOverallVolume();
	}
	if (!GetBSOnlineSettings()->EffectsSoundClass.IsNull())
	{
		GetBSOnlineSettings()->EffectsSoundClass.LoadSynchronous()->Properties.Volume = GetEffectsVolume() * GetOverallVolume();
	}
	if (!GetBSOnlineSettings()->MusicSoundClass.IsNull())
	{
		GetBSOnlineSettings()->MusicSoundClass.LoadSynchronous()->Properties.Volume = GetMusicVolume() * GetOverallVolume();
	}
}

bool UBSGameUserSettings::SetActiveDisplay(const int32 ActiveDisplay)
{
	if (!GEngine || !GEngine->GameViewport) return false;
	FDisplayMetrics Displays;
	FSlateApplication::Get().GetDisplayMetrics(Displays);
	if (Displays.MonitorInfo.IsValidIndex(ActiveDisplay))
	{
		const FMonitorInfo TargetMonitor = Displays.MonitorInfo[ActiveDisplay];
		const FVector2D WindowPosition(TargetMonitor.WorkArea.Left, TargetMonitor.WorkArea.Top);
		
		if (const TSharedPtr<SWindow> Window = GEngine->GameViewport->GetWindow(); Window.IsValid())
		{
			if (Window->GetPositionInScreen() != WindowPosition)
			{
				Window->MoveWindowTo(WindowPosition);
				return true;
			}
		}
	}
	return false;
}

void UBSGameUserSettings::SetToDefaults()
{
	
	// Gameplay
	CameraDistanceValue = 2;
	bShowOwnName = false;
	bShowOtherNames = true;
	bShowUnitPlates = true;
	bShowPercentages = false;
	bShowValues = true;
	bShowFloatingText = true;
	bDisableChat = false;

	// Controls
	LookSensitivityHorizontal = 1.f;
	LookSensitivityVertical = 1.f;
	bInvertVerticalAxis = false;

	// Audio
	OverallVolume = 0.85f;
	EffectsVolume = 1.f;
	MusicVolume = 1.f;

	// Graphics
	int32 CurrentMonitorIndex = -100;
	
	for (int32 i = 0; i < GetAvailableMonitors().Num(); i++)
	{
		if (const FMonitorInfo& Mon = GetAvailableMonitors()[i]; 
			Mon.bIsPrimary)
			CurrentMonitorIndex = i;
	}
	
	if (CurrentMonitorIndex == -100)
	{
		DisplayId = 0;
	}
	else
	{
		DisplayId = CurrentMonitorIndex;
	}
	
	AntiAliasingMode = 5;
	bFrameRateLocked = false;
	FieldOfView = 90.f;
	
	bPerformanceCounter = false;
	
	Super::SetToDefaults();
	
	OnSettingsUpdatedDelegate.Broadcast();
}

void UBSGameUserSettings::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();
	OnSettingsUpdatedDelegate.Broadcast();
}
