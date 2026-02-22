// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSWUserConfirmContext.h"

#include "CommonRichTextBlock.h"
#include "Widgets/Base/BSWButtonBase.h"

void UBSWUserConfirmContext::NativeConstruct()
{
	Super::NativeConstruct();
	
	ConfirmButton->OnClicked().AddWeakLambda(this, [this]()
	{
		OnConfirmed.Broadcast();
		if (bRemoveOnUserConfirm) RemoveFromParent();
	});
	CancelButton->OnClicked().AddWeakLambda(this, [this]()
	{
		OnCancelled.Broadcast();
		if (bRemoveOnUserConfirm) RemoveFromParent();
	});
}

void UBSWUserConfirmContext::InitializeUserConfirmWidget(FText InContextText, FText InConfirmText, FText InCancelText,
	bool bRemoveOnConfirm)
{
	bRemoveOnUserConfirm = bRemoveOnConfirm;
	ContextText->SetText(InContextText);
	ConfirmButton->ButtonText = InConfirmText;
	CancelButton->ButtonText = InCancelText;
}
