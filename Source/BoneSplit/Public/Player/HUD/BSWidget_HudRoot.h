// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSWidget_HudRoot.generated.h"

class UCommonActivatableWidgetStack;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWidget_HudRoot : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta=(BindWidget))
	UCommonActivatableWidgetStack* WidgetStack;
};
