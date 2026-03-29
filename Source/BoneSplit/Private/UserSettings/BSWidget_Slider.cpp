// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_Slider.h"

#include "CommonRichTextBlock.h"
#include "Components/Slider.h"

void UBSWidget_Slider::NativeConstruct()
{
	Super::NativeConstruct();
	
	Slider_Value->MouseUsesStep = true;
	
	Slider_Value->OnValueChanged.RemoveDynamic(this, &UBSWidget_Slider::NativeOnSliderValueChanged);
	Slider_Value->OnValueChanged.AddDynamic(this, &UBSWidget_Slider::NativeOnSliderValueChanged);
}

void UBSWidget_Slider::SetSliderValue(const float NewValue)
{
	bSettingValueProgrammatically = true;
	Slider_Value->SetValue(NewValue);
	Text_Value->SetText(FText::AsNumber(NewValue));
	bSettingValueProgrammatically = false;
}

void UBSWidget_Slider::NativeOnSliderValueChanged(const float NewValue)
{
	if (bSettingValueProgrammatically)
		return;

	Text_Value->SetText(FText::AsNumber(NewValue));
	OnSliderValueChanged.Broadcast(NewValue);
}
