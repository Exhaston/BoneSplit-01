// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSWUserConfirmContext.generated.h"

class UCommonRichTextBlock;
class UBSWButtonBase;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWUserConfirmContext : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	virtual void InitializeUserConfirmWidget(FText InContextText, FText InConfirmText, FText InCancelText, bool bRemoveOnConfirm);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRemoveOnUserConfirm = true;
	
	FSimpleMulticastDelegate OnConfirmed;
	FSimpleMulticastDelegate OnCancelled;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* ContextText;
	
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* ConfirmButton;
	
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* CancelButton;
};
