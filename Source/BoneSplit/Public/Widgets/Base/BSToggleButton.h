// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BSToggleButton.generated.h"

class UCommonLazyImage;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSToggleButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	
	virtual void SetState(bool bTrue);
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UCommonLazyImage* CheckedImage;
};
