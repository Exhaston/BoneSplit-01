// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSEquipmentDropWidget.generated.h"

class UBSEquipmentEffect;
class UCommonLazyImage;
class UCommonRichTextBlock;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BONESPLIT_API UBSEquipmentDropWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	//TODO: Add granted ability images
	
	virtual void InitializeEquipmentDropWidget(const UBSEquipmentEffect* EquipmentEffectCDO);
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* EquipmentNameText;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* EquipmentDescriptionText;
	
	UPROPERTY(meta=(BindWidget))
	UCommonLazyImage* EquipmentIcon;
	
};
