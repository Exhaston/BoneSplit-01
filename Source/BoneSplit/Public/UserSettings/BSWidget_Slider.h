// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSWidget_Slider.generated.h"

class USlider;
class UCommonRichTextBlock;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BONESPLIT_API UBSWidget_Slider : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnSliderValueChanged, float NewValue);
	
	FBSOnSliderValueChanged OnSliderValueChanged;
	
	virtual void SetSliderValue(float NewValue);
	
	UPROPERTY(meta=(BindWidget))
	USlider* Slider_Value;
	
	UFUNCTION()
	void NativeOnSliderValueChanged(float NewValue);
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* Text_Value;
	
	bool bSettingValueProgrammatically = false;
};
