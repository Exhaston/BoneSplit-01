// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Base/BSWRoot.h"
#include "BSGameplayRootWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSGameplayRootWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta=(BindWidget))
	UCommonActivatableWidgetStack* WidgetStack;
};
