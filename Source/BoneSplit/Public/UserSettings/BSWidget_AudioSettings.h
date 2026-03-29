// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWidget_AudioSettings.generated.h"

class UBSWidget_IntSelector;
class UBSGameUserSettings;
class UBSWidget_Slider;
class USlider;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_AudioSettings : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	
	virtual void NativeOnSettingsRefreshed();
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_OverallVolume;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_EffectsVolume;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_Slider* Slider_MusicVolume;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_AudioQuality;
	
	UPROPERTY(Transient)
	UBSGameUserSettings* UserSettings;
};
