// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWidget_GraphicsSettings.generated.h"

class UBSWidget_Slider;
class UBSGameUserSettings;
class UBSWidget_IntSelector;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_GraphicsSettings : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnActivated() override;
	
	virtual void NativeOnDeactivated() override;
	
	virtual void InitFieldOfView();
	
	virtual void InitResolution();
	
	virtual void InitDisplaySelection();
	
	virtual void InitWindowMode();
	
	virtual void InitVsync();
	
	virtual void InitRenderScaleSlider();
	
	virtual void InitFrameRateLocking();
	
	virtual void NativeOnSettingsRefreshed();
	
	virtual void InitQualitySelectors();
	
	TArray<FIntPoint> SupportedResolutions;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_Resolution;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_MonitorSelect;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_WindowMode;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_AntiAliasingMode;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_LockFrameRate;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_FrameRate;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_RenderScale;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_FieldOfView;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_Vsync;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_OverallQuality;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_ViewDistance;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_Textures;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_AntiAliasingQuality;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_Shadows;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_Foliage;
	
	UPROPERTY(Transient)
	UBSGameUserSettings* GameUserSettings;
};
