// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonLazyImage.h"
#include "CommonUserWidget.h"
#include "BSBuffIcon.generated.h"

class UOverlay;
class UBSBuffEffect;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSBuffIcon : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializeBuffIcon(const UBSBuffEffect* BuffEffect);
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UCommonLazyImage> BuffIconImage;
};
