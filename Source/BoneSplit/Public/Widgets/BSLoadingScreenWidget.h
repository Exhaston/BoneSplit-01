// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSLoadingScreenWidget.generated.h"

class UCommonRichTextBlock;
class UCommonLazyImage;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSLoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	virtual void LoadingComplete();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonRichTextBlock> MapName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText LoadingCompleteText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonRichTextBlock> StatusText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonLazyImage> SplashImage;
};
