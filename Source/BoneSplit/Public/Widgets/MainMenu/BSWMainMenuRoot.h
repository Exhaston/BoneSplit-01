// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSWMainMenuRoot.generated.h"

class UCommonActivatableWidgetStack;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWMainMenuRoot : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	UCommonActivatableWidgetStack* GetWidgetStack() { return WidgetStack; }
	
protected:
	
	UPROPERTY(meta=(BindWidget))
	UCommonActivatableWidgetStack* WidgetStack;
};
