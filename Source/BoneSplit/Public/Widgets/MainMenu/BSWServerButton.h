// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BSWServerButton.generated.h"

class UCommonRichTextBlock;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWServerButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializeServerButton(FText InSessionName, FText InSessionUserCount, FText InSessionPingText);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(BindWidget))
	UCommonRichTextBlock* SessionNameText;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(BindWidget))
	UCommonRichTextBlock* SessionCountText;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(BindWidget))
	UCommonRichTextBlock* SessionPingText;
};
