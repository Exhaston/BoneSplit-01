// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWidget_GameplaySettings.generated.h"

class UBSGameUserSettings;
class UBSWidget_IntSelector;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_GameplaySettings : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnActivated() override;
	
	virtual void NativeOnDeactivated() override;
	
	virtual void NativeOnSettingsRefreshed();
	
	UPROPERTY(Transient)
	UBSGameUserSettings* UserSettings;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_CameraOffset;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_ShowOwnName;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_ShowOtherNames;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_ShowValues;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_ShowPercentages;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_IntSelector* IntSelector_DisplayPerformance;
};
