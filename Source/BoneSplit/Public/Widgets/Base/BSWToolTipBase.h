// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSWToolTipBase.generated.h"

class UCommonLazyImage;
class UCommonRichTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWToolTipBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	static UBSWToolTipBase* CreateGenericToolTip(
	APlayerController* OwningPlayer, const FText& Header, const FText& Text, const FText& AltHeader = FText::GetEmpty());
	
	virtual void SetToolTipText(FText Header, FText Text , FText AltHeader = FText::GetEmpty());
	
	virtual void SetToolTipImage(TSoftObjectPtr<UTexture2D> InBrush);
	virtual void SetToolTipAltImage(TSoftObjectPtr<UTexture2D> InBrush);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
	UCommonRichTextBlock* ToolTipHeader;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
	UCommonRichTextBlock* ToolTipAltHeader;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
	UCommonRichTextBlock* ToolTipTextBlock;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
	UCommonLazyImage* ToolTipIcon;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
	UCommonLazyImage* ToolTipIconAlt;
};
