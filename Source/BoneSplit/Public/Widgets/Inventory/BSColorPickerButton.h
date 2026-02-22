// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BSColorPickerButton.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSColorPickerButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeOnClicked() override;
	
	//Currently hard coded for debugging
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ColorIndex = 0;
	
};
