// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CommonUserWidget.h"
#include "BSAttributeText.generated.h"

class UCommonTextBlock;

UCLASS()
class BONESPLIT_API UBSAttributeText : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	virtual void SetAttributeText();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayAttribute Attribute;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Multiplier = 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Suffix;
	
	UPROPERTY(meta=(BindWidget))
	UCommonTextBlock* StatText;
};
