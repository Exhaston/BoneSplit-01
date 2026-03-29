// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "BSGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	
	static UBSGameUserSettings* GetBSGameUserSettings()
	{
		return Cast<UBSGameUserSettings>(GetGameUserSettings());
	}
	
	FSimpleMulticastDelegate OnSettingsUpdatedDelegate;
	
	/** Returns all connected monitor info */
	static TArray<FMonitorInfo> GetAvailableMonitors();
	

	virtual void LoadSettings(bool bForceReload = false) override;
	
	
	virtual void ApplyNonResolutionSettings() override;
	
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	
	virtual void ApplyCVars();
	
	virtual bool SetActiveDisplay(int32 ActiveDisplay);

	virtual void SetToDefaults() override;

	virtual void ResetToCurrentSettings() override;

	/* Gameplay */

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	int32 GetCameraDistanceValue() const { return CameraDistanceValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetCameraDistanceValue(int32 NewValue) { CameraDistanceValue = 
		FMath::Clamp(NewValue,0 ,GetAvailableCameraDistances().Num() - 1); }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	bool GetShowOwnName() const { return bShowOwnName; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetShowOwnName(bool bNewValue) { bShowOwnName = bNewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	bool GetShowOtherNames() const { return bShowOtherNames; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetShowOtherNames(bool bNewValue) { bShowOtherNames = bNewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	bool GetShowUnitPlates() const { return bShowUnitPlates; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetShowUnitPlates(bool bNewValue) { bShowUnitPlates = bNewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	bool GetShowPercentages() const { return bShowPercentages; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetShowPercentages(bool bNewValue) { bShowPercentages = bNewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	bool GetShowValues() const { return bShowValues; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetShowValues(bool bNewValue) { bShowValues = bNewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	bool GetShowFloatingText() const { return bShowFloatingText; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetShowFloatingText(bool bNewValue) { bShowFloatingText = bNewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	bool GetDisableChat() const { return bDisableChat; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetDisableChat(bool bNewValue) { bDisableChat = bNewValue; }

	/* Controls */

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	float GetLookSensitivityHorizontal() const { return LookSensitivityHorizontal; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetLookSensitivityHorizontal(float NewValue) { LookSensitivityHorizontal = NewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	float GetLookSensitivityVertical() const { return LookSensitivityVertical; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetLookSensitivityVertical(float NewValue) { LookSensitivityVertical = NewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	bool GetInvertVerticalAxis() const { return bInvertVerticalAxis; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetInvertVerticalAxis(bool bNewValue) { bInvertVerticalAxis = bNewValue; }

	/* Audio */

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	float GetOverallVolume() const { return OverallVolume; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetOverallVolume(float NewValue) { OverallVolume = NewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	float GetEffectsVolume() const { return EffectsVolume; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetEffectsVolume(float NewValue) { EffectsVolume = NewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMusicVolume(float NewValue) { MusicVolume = NewValue; }

	/* Additional Graphics */
										  
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	int32 GetDisplayId() const { return DisplayId; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetDisplayId(int32 NewValue) { DisplayId = NewValue; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	int32 GetAntiAliasingMode() const { return AntiAliasingMode; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetAntiAliasingMode(int32 NewValue) { AntiAliasingMode = NewValue; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	bool GetFrameRateLocked() const { return bFrameRateLocked; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetFrameRateLocked(bool NewValue) { bFrameRateLocked = NewValue; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	float GetFieldOfView() const { return FieldOfView; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetFieldOfView(float NewValue) { FieldOfView = NewValue; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	bool GetPerformanceDisplay() const { return bPerformanceCounter; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetPerformanceDisplay(bool NewValue) { bPerformanceCounter = NewValue; }

private:
	
	virtual TArray<float> GetAvailableCameraDistances() { return {300, 350, 400, 450, 500}; }
	
	/* Gameplay */

	UPROPERTY(Config)
	int32 CameraDistanceValue = 2;

	UPROPERTY(Config)
	bool bShowOwnName = false;

	UPROPERTY(Config)
	bool bShowOtherNames = true;

	UPROPERTY(Config)
	bool bShowUnitPlates = true;

	UPROPERTY(Config)
	bool bShowPercentages = false;

	UPROPERTY(Config)
	bool bShowValues = true;

	UPROPERTY(Config)
	bool bShowFloatingText = true;

	UPROPERTY(Config)
	bool bDisableChat = false;

	/* Controls */

	UPROPERTY(Config)
	float LookSensitivityHorizontal = 1;

	UPROPERTY(Config)
	float LookSensitivityVertical = 1;

	UPROPERTY(Config)
	bool bInvertVerticalAxis = false;

	/* Audio */

	UPROPERTY(Config)
	float OverallVolume = 0.85f;

	UPROPERTY(Config)
	float EffectsVolume = 1;

	UPROPERTY(Config)
	float MusicVolume = 1;

	/* Graphics */

	UPROPERTY(Config)
	int32 DisplayId = 0;

	UPROPERTY(Config)
	int32 AntiAliasingMode = 5; //SMAA
	
	UPROPERTY(Config)
	bool bFrameRateLocked = false;
	
	UPROPERTY(Config)
	bool bPerformanceCounter = false;
	
	UPROPERTY(Config)
	float FieldOfView = 90;
};
